package com.qiplat.sweeteditor.core;

import com.google.gson.annotations.SerializedName;

public class Cursor {
    @SerializedName("position")
    public PointF position;

    @SerializedName("show_dragger")
    public boolean showDragger;
}
