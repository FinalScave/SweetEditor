package com.qiplat.sweeteditor.core;

import android.graphics.Paint;
import android.graphics.Typeface;

public class TextMeasurer {
    private final Paint mTextPaint;
    private float mTextSize = 12;
    private float mScale = 1;

    public TextMeasurer(Paint textPaint) {
        this.mTextPaint = textPaint;
    }

    public void setTypeface(Typeface typeface) {
        mTextPaint.setTypeface(typeface);
    }

    public void setTextSize(float textSize) {
        mTextSize = textSize;
        mTextPaint.setTextSize(mTextSize * mScale);
    }

    public void setScale(float scale) {
        mScale = scale;
        mTextPaint.setTextSize(mTextSize * mScale);
    }

    float measureWidth(String text, boolean isBold) {
        return mTextPaint.measureText(text);
    }

    float getFontHeight() {
        return mTextPaint.getFontMetrics().bottom - mTextPaint.getFontMetrics().top;
    }
}
