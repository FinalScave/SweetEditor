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

import com.qiplat.sweeteditor.core.Document;
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
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        int width = MeasureSpec.getSize(widthMeasureSpec);
        int height = MeasureSpec.getSize(heightMeasureSpec);
        setMeasuredDimension(width, height);
        mEditorCore.setViewport(width, height);
        postInvalidate();
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        EditorCore.GestureResult result = mEditorCore.handleGestureEvent(event);
        Log.d(TAG, "result: " + result);
        return true;
    }

    @Override
    protected void onDraw(@NonNull Canvas canvas) {
        mEditorCore.buildRenderModel();
    }

    public void loadDocument(Document document) {
        mEditorCore.loadDocument(document);
    }

    public void setTypeface(Typeface typeface) {
        mTextMeasurer.setTypeface(typeface);
        mEditorCore.resetMeasurer();
    }

    public void setTextSize(float textSize) {
        mTextMeasurer.setTextSize(textSize);
    }

    public void setScale(float scale) {
        mTextMeasurer.setScale(scale);
    }

    private void initView(Context context) {
        mBackgroundPaint = new Paint();
        mTextPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        mCursorPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        mLinePaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        mTextMeasurer = new TextMeasurer(mTextPaint);

        int scaledTouchSlop = ViewConfiguration.get(context).getScaledTouchSlop();
        TouchConfig touchConfig = new TouchConfig(scaledTouchSlop, 300);
        EditorConfig editorConfig = new EditorConfig(touchConfig);
        mEditorCore = new EditorCore(editorConfig, mTextMeasurer);

        setTypeface(Typeface.create(Typeface.MONOSPACE, Typeface.NORMAL));
    }
}
