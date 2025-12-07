package com.qiplat.sweeteditor.core;

public class TouchConfig {
    public final float touchSlop;
    public final long doubleTapTimeout;

    public TouchConfig(float touchSlop, long doubleTapTimeout) {
        this.touchSlop = touchSlop;
        this.doubleTapTimeout = doubleTapTimeout;
    }
}
