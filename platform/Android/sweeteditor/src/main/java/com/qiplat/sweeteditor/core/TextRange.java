package com.qiplat.sweeteditor.core;

import androidx.annotation.NonNull;

public class TextRange {
    public TextPosition start = TextPosition.NONE;
    public TextPosition end = TextPosition.NONE;

    public TextRange() {
    }

    public TextRange(TextPosition start, TextPosition end) {
        this.start = start;
        this.end = end;
    }

    @NonNull
    @Override
    public String toString() {
        return "TextRange{" +
                "start=" + start +
                ", end=" + end +
                '}';
    }
}
