package com.qiplat.sweeteditor;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewConfiguration;

import androidx.annotation.NonNull;

import com.qiplat.sweeteditor.core.EditorConfig;
import com.qiplat.sweeteditor.core.EditorCore;
import com.qiplat.sweeteditor.core.TextMeasurer;
import com.qiplat.sweeteditor.core.TouchConfig;

public class SweetEditor extends View {
    private static final String TAG = SweetEditor.class.getSimpleName();
    private EditorCore mEditorCore;
    private TextMeasurer mTextMeasurer;
    private Paint mBackgroundPaint;
    private Paint mTextPaint;
    private Paint mCursorPaint;
    private Paint mLinePaint;

    public SweetEditor(Context context) {
        super(context);
        initView(context);
    }

    public SweetEditor(Context context, AttributeSet attrs) {
        super(context, attrs);
        initView(context);
    }

    public SweetEditor(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initView(context);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        EditorCore.GestureResult result = mEditorCore.handleGestureEvent(event);
        Log.d(TAG, "result: " + result);
        return true;
    }

    @Override
    protected void onDraw(@NonNull Canvas canvas) {
        Paint.FontMetrics fm = mTextPaint.getFontMetrics();
        canvas.drawText("你好", 0, fm.descent - fm.ascent, mTextPaint);
        canvas.drawText("你好", 100, fm.descent - fm.ascent, mTextPaint);
    }

    public void setTypeface(Typeface typeface) {
        mTextMeasurer.setTypeface(typeface);
    }

    public void setTextSize(float textSize) {
        mTextMeasurer.setTextSize(textSize);
    }

    public void setScale(float scale) {
        mTextMeasurer.setScale(scale);
    }

    private void initView(Context context) {
        int scaledTouchSlop = ViewConfiguration.get(context).getScaledTouchSlop();
        TouchConfig touchConfig = new TouchConfig(scaledTouchSlop, 300);
        EditorConfig editorConfig = new EditorConfig(touchConfig);
        mEditorCore = new EditorCore(editorConfig);
        mBackgroundPaint = new Paint();
        mTextPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        mCursorPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        mLinePaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        mTextMeasurer = new TextMeasurer(mTextPaint);
    }
}
