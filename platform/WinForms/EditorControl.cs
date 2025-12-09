using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Text;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms.Design;

namespace SweetEditor {
	public class EditorFontLoader : IDisposable {
		[DllImport("gdi32.dll", EntryPoint = "AddFontResourceEx", SetLastError = true)]
		private static extern int AddFontResourceEx(string lpszFilename, uint fl, IntPtr pdv);

		[DllImport("gdi32.dll", EntryPoint = "RemoveFontResourceEx", SetLastError = true)]
		private static extern int RemoveFontResourceEx(string lpszFilename, uint fl, IntPtr pdv);

		private const uint FR_PRIVATE = 0x10;
		private readonly PrivateFontCollection fontCollection = new PrivateFontCollection();
		private string fontFilePath;

		public string LoadFont(string path) {
			fontFilePath = Path.GetFullPath(path);
			if (!File.Exists(fontFilePath)) {
				throw new FileNotFoundException("Font file not found", fontFilePath);
			}
			int added = AddFontResourceEx(fontFilePath, FR_PRIVATE, IntPtr.Zero);
			if (added == 0) {
				Console.WriteLine("Warning: Failed to register font with GDI.");
			}
			fontCollection.AddFontFile(fontFilePath);
			return fontCollection.Families[fontCollection.Families.Length - 1].Name;
		}

		public Font CreateFont(float size, FontStyle style = FontStyle.Regular) {
			if (fontCollection.Families.Length == 0)
				throw new InvalidOperationException("No font loaded.");

			return new Font(fontCollection.Families[0], size, style);
		}

		public void Dispose() {
			fontCollection?.Dispose();
			if (!string.IsNullOrEmpty(fontFilePath)) {
				RemoveFontResourceEx(fontFilePath, FR_PRIVATE, IntPtr.Zero);
			}
		}
	}

	[Designer("System.Windows.Forms.Design.ControlDesigner, System.Design")]
	public class EditorControl : Control {
		private EditorCore editorCore;
		private Font regularFont = new Font("Consolas", 11f, FontStyle.Regular);
		private Font boldFont = new Font("Consolas", 11f, FontStyle.Bold);
		private Font italicFont = new Font("Consolas", 11f, FontStyle.Italic);
		private Font boldItalicFont = new Font("Consolas", 11f, FontStyle.Bold | FontStyle.Italic);
		private Graphics textGraphics;
		private EditorRenderModel? renderModel;

		public EditorControl() {
			InitializeComponent();
		}

		public EditorControl(IContainer container) {
			container.Add(this);
			InitializeComponent();
		}

		public void LoadDocument(Document document) {
			editorCore.LoadDocument(document);
			RebuildRenderModelAndInvalidate();
		}

		private void InitializeComponent() {
			SetStyle(ControlStyles.OptimizedDoubleBuffer |
					 ControlStyles.AllPaintingInWmPaint |
					 ControlStyles.UserPaint |
					 ControlStyles.ResizeRedraw, true);
			this.DoubleBuffered = true;
			this.Font = regularFont;
			this.TabStop = true;
			if (IsDesignMode()) {
				return;
			}
			textGraphics = CreateGraphics();
			textGraphics.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;
			editorCore = new EditorCore(20.0f, 300, OnMeasureText, OnGetFontMetrics);
		}

		protected override void OnPaint(PaintEventArgs e) {
			base.OnPaint(e);
			if (this.renderModel == null) {
				return;
			}
			e.Graphics.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;
			e.Graphics.SmoothingMode = SmoothingMode.AntiAlias;

			EditorRenderModel renderModel = (EditorRenderModel)this.renderModel;
			List<VisualLine> lines = renderModel.VisualLines;
			if (lines != null) {
				foreach (var line in lines) {
					foreach (var run in line.Runs) {
						DrawVisualRun(e.Graphics, run);
					}
				}
			}
		}

		protected override void OnResize(EventArgs e) {
			base.OnResize(e);
			if (IsDesignMode()) {
				return;
			}
			editorCore.SetViewport(this.ClientSize.Width, this.ClientSize.Height);
			RebuildRenderModelAndInvalidate();
		}

		protected override void OnKeyDown(KeyEventArgs e) {
			bool handled = false;
			/*switch (e.KeyCode) {
				case Keys.Left: editorCore.MoveCaretLeft(e.Shift); handled = true; break;
				case Keys.Right: editorCore.MoveCaretRight(e.Shift); handled = true; break;
				case Keys.Up: editorCore.MoveCaretUp(e.Shift); handled = true; break;
				case Keys.Down: editorCore.MoveCaretDown(e.Shift); handled = true; break;
				case Keys.Home: editorCore.MoveToLineStart(e.Shift); handled = true; break;
				case Keys.End: editorCore.MoveToLineEnd(e.Shift); handled = true; break;
				case Keys.PageUp: editorCore.PageUp(e.Shift); handled = true; break;
				case Keys.PageDown: editorCore.PageDown(e.Shift); handled = true; break;
				case Keys.Back: editorCore.Backspace(); handled = true; break;
				case Keys.Delete: editorCore.Delete(); handled = true; break;
				case Keys.Enter: editorCore.InsertText("\n"); handled = true; break;
				case Keys.Tab:
					editorCore.InsertText("\t");
					handled = true;
					break;
				case Keys.Z when e.Control:
					editorCore.Undo();
					handled = true;
					break;
				case Keys.Y when e.Control:
					editorCore.Redo();
					handled = true;
					break;
			}*/
			if (handled) {
				e.Handled = true;
				Invalidate();
			}
			base.OnKeyDown(e);
		}

		protected override void OnKeyPress(KeyPressEventArgs e) {
			if (!char.IsControl(e.KeyChar) || e.KeyChar == '\t' || e.KeyChar == '\n') {
				//editorCore.InsertText(e.KeyChar.ToString());
				e.Handled = true;
				RebuildRenderModelAndInvalidate();
			}

			base.OnKeyPress(e);
		}

		protected override void OnMouseDown(MouseEventArgs e) {
			Focus();
			if (e.Button == MouseButtons.Left) {
				GestureResult gestureResult = editorCore.HandleGestureEvent(new GestureEvent {
					Type = EventType.MOUSE_DOWN,
					Points = [new PointF(e.X, e.Y)]
				});
				
				RebuildRenderModelAndInvalidate();
			}
			base.OnMouseDown(e);
		}

		protected override void OnMouseWheel(MouseEventArgs e) {
			if ((Control.ModifierKeys & Keys.Control) != 0) {
				float delta = e.Delta > 0 ? 1.1f : 0.9f;
			} else {
				int lines = e.Delta > 0 ? -3 : 3;
				// TODO 滚动
			}
			RebuildRenderModelAndInvalidate();
			base.OnMouseWheel(e);
		}

		private void DrawVisualRun(Graphics g, VisualRun visualRun) {
			var brush = new SolidBrush(Color.Black);
			var text = editorCore.GetVisualRunText(visualRun.TextId);
			g.DrawString(text, regularFont, brush, visualRun.X, visualRun.Y);
			brush.Dispose();
		}

		private void RebuildRenderModelAndInvalidate() {
			renderModel = editorCore.BuildRenderModel();
			Invalidate();
		}

		private float OnMeasureText(string text, int styleId) {
			return textGraphics.MeasureString(text, regularFont).Width;
		}

		private void OnGetFontMetrics(IntPtr arrPtr, int length) {
			int designAscent = regularFont.FontFamily.GetCellAscent(regularFont.Style);
			int designDescent = regularFont.FontFamily.GetCellDescent(regularFont.Style);
			int designEmHeight = regularFont.FontFamily.GetEmHeight(regularFont.Style);
			float dpiScaleY = textGraphics.DpiY / 96f;
			float emSizeInPoints = regularFont.SizeInPoints;
			float pixelAscent = designAscent * emSizeInPoints * dpiScaleY / designEmHeight;
			float pixelDescent = designDescent * emSizeInPoints * dpiScaleY / designEmHeight;
			float pixelEmHeight = designEmHeight * emSizeInPoints * dpiScaleY / designEmHeight;
			//int designLineSpacing = regularFont.FontFamily.GetLineSpacing(regularFont.Style);
			//float pixelLineSpacing = designLineSpacing * emSizeInPoints * dpiScaleY / designEmHeight;
			float[] metrics = [-pixelAscent, pixelDescent];
			Marshal.Copy(metrics, 0, arrPtr, metrics.Length);
		}

		private static bool IsDesignMode() {
			if (LicenseManager.UsageMode == LicenseUsageMode.Designtime) {
				return true;
			}
			string processName = System.Diagnostics.Process.GetCurrentProcess().ProcessName.ToLower();
			if (processName.Contains("devenv") ||          // Visual Studio 主进程
				processName.Contains("smsvc") ||           // 某些服务进程
				processName.Contains("designtoolsserver")) // .NET Core WinForms 设计器进程
			{
				return true;
			}
			return false;
		}
	}
}
