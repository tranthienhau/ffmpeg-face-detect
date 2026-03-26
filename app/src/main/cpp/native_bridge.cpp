#include <jni.h>
#include <android/native_window_jni.h>
#include "video_pipeline.h"

static VideoPipeline* pipeline = nullptr;

extern "C" {

JNIEXPORT jboolean JNICALL
Java_com_trungnt_ffmpegface_NativeBridge_startStream(
    JNIEnv* env, jobject /* thiz */, jstring rtsp_url, jobject surface
) {
    const char* url = env->GetStringUTFChars(rtsp_url, nullptr);
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);

    if (pipeline) {
        pipeline->stop();
        delete pipeline;
    }
    pipeline = new VideoPipeline();

    bool ok = pipeline->start(url, window);

    env->ReleaseStringUTFChars(rtsp_url, url);
    ANativeWindow_release(window);
    return ok;
}

JNIEXPORT void JNICALL
Java_com_trungnt_ffmpegface_NativeBridge_stopStream(
    JNIEnv* /* env */, jobject /* thiz */
) {
    if (pipeline) {
        pipeline->stop();
        delete pipeline;
        pipeline = nullptr;
    }
}

JNIEXPORT jint JNICALL
Java_com_trungnt_ffmpegface_NativeBridge_getFaceCount(
    JNIEnv* /* env */, jobject /* thiz */
) {
    return pipeline ? pipeline->face_count() : 0;
}

} // extern "C"
