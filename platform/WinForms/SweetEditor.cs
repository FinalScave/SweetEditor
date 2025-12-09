using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using static SweetEditor.EditorCore;

namespace SweetEditor {

	public class Document {
		private const string DLL_NAME = "sweeteditor.dll";
		internal IntPtr nativeHandle;

		public Document(string text) {
			nativeHandle = _CreateDocument(text);
		}

		[DllImport(DLL_NAME, EntryPoint = "create_document_from_utf8", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
		private static extern IntPtr _CreateDocument(string text);
	}

	public class EditorCore : IDisposable {
		private const string DLL_NAME = "sweeteditor.dll";
		private static bool exceptionHandlerInitialized = false;
		private readonly IntPtr nativeHandle;
		private MeasureTextWidth textMeasurer;
		private GCHandle textMeasurerGCHandle;

		public EditorCore(float touchSlop, long doubleTapTimeout, MeasureTextWidth measureTextWidth) {
			if (!exceptionHandlerInitialized) {
				_InitUnhandledExceptionHandler();
				exceptionHandlerInitialized = true;
			}
			textMeasurer = measureTextWidth;
			textMeasurerGCHandle = GCHandle.Alloc(textMeasurer);
			nativeHandle = _CreateEditorCore(touchSlop, doubleTapTimeout, textMeasurer);
		}

		public void SetViewport(int width, int height) {
			_SetViewport(nativeHandle, width, height);
		}

		public void LoadDocument(Document document) {
			_LoadDocument(nativeHandle, document.nativeHandle);
		}

		public string BuildRenderModel() {
			IntPtr utf8Ptr = _BuildRenderModel(nativeHandle);
			string json = Marshal.PtrToStringAnsi(utf8Ptr);
			_FreeCString(utf8Ptr);
			return json;
		}

		public string GetVisualRunText(long textId) {
			IntPtr utf8Ptr = _GetVisualRunText(nativeHandle, textId);
			string text = Marshal.PtrToStringAnsi(utf8Ptr);
			_FreeCString(utf8Ptr);
			return text;
		}

		public void Dispose() {
			if (textMeasurerGCHandle.IsAllocated) {
				textMeasurerGCHandle.Free();
			}
		}

		[UnmanagedFunctionPointer(CallingConvention.StdCall)]
		public delegate float MeasureTextWidth(string text, bool isBold);

		[DllImport(DLL_NAME, EntryPoint = "init_unhandled_exception_handler", CallingConvention = CallingConvention.Cdecl)]
		private static extern void _InitUnhandledExceptionHandler();

		[DllImport(DLL_NAME, EntryPoint = "create_editor", CallingConvention = CallingConvention.Cdecl)]
		private static extern IntPtr _CreateEditorCore(float touchSlop, long doubleTapTimeout, MeasureTextWidth measureTextWidth);

		[DllImport(DLL_NAME, EntryPoint = "set_editor_viewport", CallingConvention = CallingConvention.Cdecl)]
		private static extern IntPtr _SetViewport(IntPtr handle, int width, int height);

		[DllImport(DLL_NAME, EntryPoint = "set_editor_document", CallingConvention = CallingConvention.Cdecl)]
		private static extern IntPtr _LoadDocument(IntPtr handle, IntPtr documentHandle);

		[DllImport(DLL_NAME, EntryPoint = "build_editor_render_model", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
		private static extern IntPtr _BuildRenderModel(IntPtr handle);

		[DllImport(DLL_NAME, EntryPoint = "get_editor_visual_run_text", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
		private static extern IntPtr _GetVisualRunText(IntPtr handle, long textId);

		[DllImport(DLL_NAME, EntryPoint = "free_c_string", CallingConvention = CallingConvention.Cdecl)]
		private static extern void _FreeCString(IntPtr cstringPtr);
	}

	[Designer(typeof(EditorControl))]
	public class EditorControl : Control {
		private EditorCore editorCore;
		

		public EditorControl() {
			Console.WriteLine("EditorPanel Constructor");
			InitializeComponent();
			Console.WriteLine("EditorPanel Initilized");
		}

		public EditorControl(IContainer container) {
			container.Add(this);
			InitializeComponent();
		}

		public void LoadDocument(Document document) {
			editorCore.LoadDocument(document);
		}

		public string BuildRenderModel() {
			return editorCore.BuildRenderModel();
		}

		private void InitializeComponent() {
			editorCore = new EditorCore(20.0f, 300, OnMeasureText);
			editorCore.SetViewport(720, 1280);
			this.SizeChanged += (sender, e) => {
				Console.WriteLine("SizeChanged, width: " + Width + ", height: " + Height);
				editorCore.SetViewport(this.Width, this.Height);
			};
		}

		private float OnMeasureText(string text, bool isBold) {
			return 0;
		}
	}
}
