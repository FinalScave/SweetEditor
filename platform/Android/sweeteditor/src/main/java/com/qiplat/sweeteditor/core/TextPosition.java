package com.qiplat.sweeteditor.core;

import androidx.annotation.NonNull;

public class TextPosition {
    public static final TextPosition NONE = new TextPosition();

    public int line;
    public int column;

    public TextPosition() {
    }

    public TextPosition(int line, int column) {
        this.line = line;
        this.column = column;
    }

    @NonNull
    @Override
    public String toString() {
        return "TextPosition{" +
                "line=" + line +
                ", column=" + column +
                '}';
    }
}
