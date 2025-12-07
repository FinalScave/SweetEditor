package com.qiplat.sweeteditor.core;

import com.google.gson.annotations.SerializedName;

import java.util.List;

public class EditorRenderModel {
    @SerializedName("current_line")
    public PointF currentLine;

    @SerializedName("lines")
    public List<VisualLine> lines;

    @SerializedName("cursor")
    public Cursor cursor;

    @SerializedName("guide_lines")
    public List<GuideLine> guideLines;
}
