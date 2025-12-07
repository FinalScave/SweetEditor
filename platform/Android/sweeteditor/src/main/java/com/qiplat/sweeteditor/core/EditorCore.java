package com.qiplat.sweeteditor.core;

import android.graphics.PointF;
import android.view.MotionEvent;

import androidx.annotation.NonNull;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import dalvik.annotation.optimization.CriticalNative;
import dalvik.annotation.optimization.FastNative;

public class EditorCore {
    public static final int EVENT_TYPE_UNDEFINED = 0;
    public static final int EVENT_TYPE_TOUCH_DOWN = 1;
    public static final int EVENT_TYPE_TOUCH_POINTER_DOWN = 2;
    public static final int EVENT_TYPE_TOUCH_MOVE = 3;
    public static final int EVENT_TYPE_TOUCH_POINTER_UP = 4;
    public static final int EVENT_TYPE_TOUCH_UP = 5;
    public static final int EVENT_TYPE_TOUCH_CANCEL = 6;
    public static final int GESTURE_TYPE_UNDEFINE = 0;
    public static final int GESTURE_TYPE_TAP = 1;
    public static final int GESTURE_TYPE_DOUBLE_TAP = 2;
    public static final int GESTURE_TYPE_LONG_PRESS = 3;
    public static final int GESTURE_TYPE_SCALE = 4;
    public static final int GESTURE_TYPE_SCROLL = 5;
    public static final int GESTURE_TYPE_FAST_SCROLL = 6;
    private long mNativeHandle;

    public EditorCore(EditorConfig config) {
        this.mNativeHandle = nativeMakeEditorCore(config.touchConfig.touchSlop, config.touchConfig.doubleTapTimeout);
    }

    public GestureResult handleGestureEvent(MotionEvent event) {
        int eventType = getEventTypeInt(event);
        int pointerCount = event.getPointerCount();
        float[] points = new float[pointerCount * 2];
        for (int i = 0; i < pointerCount; i++) {
            points[i * 2] = event.getX(i);
            points[i * 2 + 1] = event.getY(i);
        }
        ByteBuffer data = nativeHandleGestureEvent(mNativeHandle, eventType, pointerCount, points);
        return readGestureResult(data);
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        if (mNativeHandle == 0) {
            return;
        }
        nativeFinalizeEditorCore(mNativeHandle);
        mNativeHandle = 0;
    }

    private static int getEventTypeInt(MotionEvent event) {
        switch (event.getActionMasked()) {
            case MotionEvent.ACTION_DOWN:
                return EVENT_TYPE_TOUCH_DOWN;
            case MotionEvent.ACTION_POINTER_DOWN:
                return EVENT_TYPE_TOUCH_POINTER_DOWN;
            case MotionEvent.ACTION_MOVE:
                return EVENT_TYPE_TOUCH_MOVE;
            case MotionEvent.ACTION_POINTER_UP:
                return EVENT_TYPE_TOUCH_POINTER_UP;
            case MotionEvent.ACTION_UP:
                return EVENT_TYPE_TOUCH_UP;
            case MotionEvent.ACTION_CANCEL:
                return EVENT_TYPE_TOUCH_CANCEL;
            default:
                return EVENT_TYPE_UNDEFINED;
        }
    }

    private static GestureResult readGestureResult(ByteBuffer data) {
        GestureResult result;
        data.order(ByteOrder.nativeOrder());
        int typeInt = data.getInt();
        switch (typeInt) {
            case GESTURE_TYPE_TAP:
            case GESTURE_TYPE_DOUBLE_TAP:
                float tapX = data.getFloat();
                float tapY = data.getFloat();
                result = new GestureResult(
                        typeInt == GESTURE_TYPE_TAP
                                ? GestureType.TAP
                                : GestureType.DOUBLE_TAP,
                        new PointF(tapX, tapY)
                );
                break;
            case GESTURE_TYPE_LONG_PRESS:
                float pressX = data.getFloat();
                float pressY = data.getFloat();
                result = new GestureResult(GestureType.LONG_PRESS, new PointF(pressX, pressY));
                break;
            case GESTURE_TYPE_SCALE:
                float scale = data.getFloat();
                result = new GestureResult(GestureType.SCALE, scale);
                break;
            case GESTURE_TYPE_SCROLL:
            case GESTURE_TYPE_FAST_SCROLL:
                float scrollX = data.getFloat();
                float scrollY = data.getFloat();
                result = new GestureResult(
                        typeInt == GESTURE_TYPE_SCROLL
                                ? GestureType.SCROLL
                                : GestureType.FAST_SCROLL,
                        scrollX,
                        scrollY
                );
                break;
            default:
                result = new GestureResult();
                break;
        }
        return result;
    }

    public enum GestureType {
        UNDEFINED,
        TAP,
        DOUBLE_TAP,
        LONG_PRESS,
        SCALE,
        SCROLL,
        FAST_SCROLL,
    }

    public static class GestureResult {
        public final GestureType type;
        public final PointF tapPoint;
        public final float scale;
        public final float scrollX;
        public final float scrollY;

        public GestureResult() {
            this.type = GestureType.UNDEFINED;
            this.tapPoint = new PointF();
            this.scale = 1;
            this.scrollX = 0;
            this.scrollY = 0;
        }

        public GestureResult(GestureType type, PointF tapPoint) {
            this.type = type;
            this.tapPoint = tapPoint;
            this.scale = 1;
            this.scrollX = 0;
            this.scrollY = 0;
        }

        public GestureResult(GestureType type, float scale) {
            this.type = type;
            this.scale = scale;
            this.tapPoint = new PointF();
            this.scrollX = 0;
            this.scrollY = 0;
        }

        public GestureResult(GestureType type, float scrollX, float scrollY) {
            this.type = type;
            this.scrollX = scrollX;
            this.scrollY = scrollY;
            this.scale = 1;
            this.tapPoint = new PointF();
        }

        @NonNull
        @Override
        public String toString() {
            return "GestureResult{" +
                    "type=" + type +
                    ", tapPoint=" + tapPoint +
                    ", scale=" + scale +
                    ", scrollX=" + scrollX +
                    ", scrollY=" + scrollY +
                    '}';
        }
    }

    @CriticalNative
    private static native long nativeMakeEditorCore(float touchSlop, long doubleTapTimeout);

    @CriticalNative
    private static native void nativeFinalizeEditorCore(long handle);

    @FastNative
    private static native ByteBuffer nativeHandleGestureEvent(long handle, int type, int pointerCount, float[] points);

    static {
        System.loadLibrary("sweeteditor");
    }
}
