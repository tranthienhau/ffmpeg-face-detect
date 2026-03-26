package com.trungnt.ffmpegface

import android.content.Context
import android.util.AttributeSet
import android.view.SurfaceView

class VideoSurfaceView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null,
    defStyleAttr: Int = 0
) : SurfaceView(context, attrs, defStyleAttr) {

    init {
        holder.setKeepScreenOn(true)
    }
}
