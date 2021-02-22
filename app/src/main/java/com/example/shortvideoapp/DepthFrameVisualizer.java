package com.example.shortvideoapp;

import android.graphics.Bitmap;

public interface DepthFrameVisualizer {
    void onRawDataAvailable(Bitmap bitmap);
}
