package com.qiplat.sweeteditor.core;

import com.google.gson.annotations.SerializedName;

public class VisualRun {
    @SerializedName("type")
    public VisualRunType type;

    @SerializedName("x")
    public float x;

    @SerializedName("y")
    public float y;

    @SerializedName("text_id")
    public long textId;

    @SerializedName("style_id")
    public int styleId;
}
