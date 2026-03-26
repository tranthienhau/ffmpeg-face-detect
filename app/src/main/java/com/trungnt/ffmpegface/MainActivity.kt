package com.trungnt.ffmpegface

import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.view.SurfaceHolder
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {
    private lateinit var surfaceView: VideoSurfaceView
    private lateinit var urlInput: EditText
    private lateinit var startBtn: Button
    private lateinit var stopBtn: Button
    private lateinit var faceCountText: TextView
    private var isStreaming = false

    private val handler = Handler(Looper.getMainLooper())
    private val faceCountUpdater = object : Runnable {
        override fun run() {
            if (isStreaming) {
                val count = NativeBridge.getFaceCount()
                faceCountText.text = "Faces detected: $count"
                handler.postDelayed(this, 200)
            }
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        surfaceView = findViewById(R.id.surface_view)
        urlInput = findViewById(R.id.url_input)
        startBtn = findViewById(R.id.btn_start)
        stopBtn = findViewById(R.id.btn_stop)
        faceCountText = findViewById(R.id.face_count)

        urlInput.setText("rtsp://10.182.100.68:8554/stream")

        surfaceView.holder.addCallback(object : SurfaceHolder.Callback {
            override fun surfaceCreated(holder: SurfaceHolder) {}
            override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {}
            override fun surfaceDestroyed(holder: SurfaceHolder) {
                stopStreaming()
            }
        })

        startBtn.setOnClickListener {
            val holder = surfaceView.holder
            if (holder.surface.isValid) {
                val url = urlInput.text.toString()
                isStreaming = NativeBridge.startStream(url, holder.surface)
                updateUI()
                if (isStreaming) {
                    handler.post(faceCountUpdater)
                }
            }
        }

        stopBtn.setOnClickListener {
            stopStreaming()
        }
    }

    private fun stopStreaming() {
        NativeBridge.stopStream()
        isStreaming = false
        handler.removeCallbacks(faceCountUpdater)
        faceCountText.text = "Faces detected: 0"
        updateUI()
    }

    private fun updateUI() {
        startBtn.isEnabled = !isStreaming
        stopBtn.isEnabled = isStreaming
    }

    override fun onDestroy() {
        super.onDestroy()
        stopStreaming()
    }
}
