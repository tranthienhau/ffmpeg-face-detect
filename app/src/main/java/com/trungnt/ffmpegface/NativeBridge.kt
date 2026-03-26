package com.trungnt.ffmpegface

import android.view.Surface

object NativeBridge {
    init {
        System.loadLibrary("ffmpegface")
    }

    external fun startStream(rtspUrl: String, surface: Surface): Boolean
    external fun stopStream()
    external fun getFaceCount(): Int
}
