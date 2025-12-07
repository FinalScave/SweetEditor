package com.qiplat.sweeteditor.core;

import com.google.gson.annotations.SerializedName;

import java.util.List;

public class VisualLine {
    @SerializedName("logical_line")
    public int logicalLine;

    @SerializedName("runs")
    public List<VisualRun> runs;
}
