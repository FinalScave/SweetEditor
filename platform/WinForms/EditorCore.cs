using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Numerics;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;
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

	public struct PointF {
		[JsonPropertyName("x")]
		public float X { get; set; }
		[JsonPropertyName("y")]
		public float Y { get; set; }

		public PointF() {
			X = 0;
			Y = 0;
		}

		public PointF(float x, float y) {
			X = x;
			Y = y;
		}
	}

	public enum EventType {
		UNDEFINED = 0,
		TOUCH_DOWN = 1,
		TOUCH_POINTER_DOWN = 2,
		TOUCH_MOVE = 3,
		TOUCH_POINTER_UP = 4,
		TOUCH_UP = 5,
		TOUCH_CANCEL = 6,
		MOUSE_DOWN = 7
	}

	public struct GestureEvent {
		public EventType Type { get; set; }
		public List<PointF> Points { get; set; }

		public float[] GetPointsArray() {
			float[] arr = new float[Points.Count * 2];
			for (int i = 0; i < Points.Count; i++) {
				arr[i * 2] = Points[i].X;
				arr[i * 2 + 1] = Points[i].Y;
			}
			return arr;
		}
	}

	public enum GestureType {
		UNDEFINED = 0,
		TAP = 1,
		DOUBLE_TAP = 2,
		LONG_PRESS = 3,
		SCALE = 4,
		SCROLL = 5,
		FAST_SCROLL = 6
	}

	public struct GestureResult {
		[JsonPropertyName("type")]
		public GestureType Type { get; set; }
		[JsonPropertyName("tap_point")]
		public PointF? TapPoint { get; set; }
		[JsonPropertyName("scale")]
		public float Scale { get; set; }
		[JsonPropertyName("scroll_x")]
		public float ScrollX { get; set; }
		[JsonPropertyName("scroll_y")]
		public float ScrollY { get; set; }

		public GestureResult() {
			Type = GestureType.UNDEFINED;
			TapPoint = new PointF();
			Scale = 1f;
		}

		public GestureResult(GestureType type, PointF position) {
			Type = type;
			TapPoint = position;
		}

		public GestureResult(GestureType type, float scale) {
			Type = type;
			Scale = scale;
		}

		public GestureResult(GestureType type, float scrollX, float scrollY) {
			Type = type;
			ScrollX = scrollX;
			ScrollY = scrollY;
		}
	}

	public enum VisualRunType {
		TEXT,
		WHITESPACE,
		NEWLINE,
		INLAY_HINT,
		PHANTOM_TEXT
	}

	public struct VisualRun {
		[JsonPropertyName("type")]
		public VisualRunType Type { get; set; }
		[JsonPropertyName("x")]
		public float X { get; set; }
		[JsonPropertyName("y")]
		public float Y { get; set; }
		[JsonPropertyName("text_id")]
		public long TextId { get; set; }
		[JsonPropertyName("style_id")]
		public int StyleId { get; set; }
	}

	public struct VisualLine {
		[JsonPropertyName("logical_line")]
		public int LogicalLine { get; set; }
		[JsonPropertyName("runs")]
		public List<VisualRun> Runs { get; set; }
	}

	public struct Cursor {
		[JsonPropertyName("position")]
		public PointF Position { get; set; }
		[JsonPropertyName("show_dragger")]
		public bool ShowDragger { get; set; }
	}

	public enum GuideLineDirection {
		HORIZONTAL,
		VERTICAL
	}

	public struct GuideLine {
		[JsonPropertyName("direction")]
		public GuideLineDirection Direction { get; set; }
		[JsonPropertyName("start")]
		public PointF Start { get; set; }
		[JsonPropertyName("end")]
		public PointF End { get; set; }
	}

	public struct EditorRenderModel {
		[JsonPropertyName("current_line")]
		public PointF CurrentLine { get; set; }
		[JsonPropertyName("lines")]
		public List<VisualLine> VisualLines { get; set; }
		[JsonPropertyName("cursor")]
		public Cursor Cursor { get; set; }
		[JsonPropertyName("guide_lines")]
		public List<GuideLine> GuideLines { get; set; }
	}

	public class EditorCore : IDisposable {
		private const string DLL_NAME = "sweeteditor.dll";
		private static bool exceptionHandlerInitialized = false;
		private readonly IntPtr nativeHandle;
		private MeasureTextWidth textMeasurer;
		private GetFontMetrics fontMetrics;
		private GCHandle textMeasurerGCHandle;
		private GCHandle fontMetricsGCHandle;
		private JsonSerializerOptions serializerOptions = new() {
			Converters = { new JsonStringEnumConverter() }
		};

		public EditorCore(float touchSlop, long doubleTapTimeout, MeasureTextWidth measureTextWidth, GetFontMetrics getFontMetrics) {
			if (!exceptionHandlerInitialized) {
				_InitUnhandledExceptionHandler();
				exceptionHandlerInitialized = true;
			}
			textMeasurer = measureTextWidth;
			textMeasurerGCHandle = GCHandle.Alloc(textMeasurer);
			fontMetrics = getFontMetrics;
			fontMetricsGCHandle = GCHandle.Alloc(fontMetrics);
			nativeHandle = _CreateEditorCore(touchSlop, doubleTapTimeout, textMeasurer, fontMetrics);
		}

		public void SetViewport(int width, int height) {
			_SetViewport(nativeHandle, width, height);
		}

		public void LoadDocument(Document document) {
			_LoadDocument(nativeHandle, document.nativeHandle);
		}

		public EditorRenderModel BuildRenderModel() {
			IntPtr utf8Ptr = _BuildRenderModel(nativeHandle);
			string json = Marshal.PtrToStringAnsi(utf8Ptr)!;
			_FreeCString(utf8Ptr);
			return JsonSerializer.Deserialize<EditorRenderModel>(json, serializerOptions);
		}

		public GestureResult HandleGestureEvent(GestureEvent gestureEvent) {
			IntPtr utf8Ptr = _HandleGestureEvent(nativeHandle, (uint)gestureEvent.Type, (uint)gestureEvent.Points.Count, gestureEvent.GetPointsArray());
			string json = Marshal.PtrToStringAnsi(utf8Ptr)!;
			_FreeCString(utf8Ptr);
			Debug.WriteLine(json);
			return JsonSerializer.Deserialize<GestureResult>(json, serializerOptions);
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
		public delegate float MeasureTextWidth(string text, int styleId);

		[UnmanagedFunctionPointer(CallingConvention.StdCall)]
		public delegate void GetFontMetrics(IntPtr arrPtr, int length);

		[DllImport(DLL_NAME, EntryPoint = "init_unhandled_exception_handler", CallingConvention = CallingConvention.Cdecl)]
		private static extern void _InitUnhandledExceptionHandler();

		[DllImport(DLL_NAME, EntryPoint = "create_editor", CallingConvention = CallingConvention.Cdecl)]
		private static extern IntPtr _CreateEditorCore(float touchSlop, long doubleTapTimeout, MeasureTextWidth measureTextWidth, GetFontMetrics getFontMetrics);

		[DllImport(DLL_NAME, EntryPoint = "set_editor_viewport", CallingConvention = CallingConvention.Cdecl)]
		private static extern IntPtr _SetViewport(IntPtr handle, int width, int height);

		[DllImport(DLL_NAME, EntryPoint = "set_editor_document", CallingConvention = CallingConvention.Cdecl)]
		private static extern IntPtr _LoadDocument(IntPtr handle, IntPtr documentHandle);

		[DllImport(DLL_NAME, EntryPoint = "handle_editor_gesture_event", CallingConvention = CallingConvention.Cdecl)]
		private static extern IntPtr _HandleGestureEvent(IntPtr handle, uint type, uint pointerCount, float[] points);

		[DllImport(DLL_NAME, EntryPoint = "build_editor_render_model", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
		private static extern IntPtr _BuildRenderModel(IntPtr handle);

		[DllImport(DLL_NAME, EntryPoint = "get_editor_visual_run_text", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
		private static extern IntPtr _GetVisualRunText(IntPtr handle, long textId);

		[DllImport(DLL_NAME, EntryPoint = "free_c_string", CallingConvention = CallingConvention.Cdecl)]
		private static extern void _FreeCString(IntPtr cstringPtr);
	}
}
