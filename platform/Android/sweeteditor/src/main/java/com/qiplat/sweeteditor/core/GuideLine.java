package com.qiplat.sweeteditor.core;

import com.google.gson.annotations.SerializedName;

public class GuideLine {
    @SerializedName("direction")
    public GuideLineDirection direction;

    @SerializedName("start")
    public PointF start;

    @SerializedName("end")
    public PointF end;
}
