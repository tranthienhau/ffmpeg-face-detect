cmake_minimum_required(VERSION 3.22.1)
project("ffmpegface")

# FFmpeg prebuilt libraries
set(FFMPEG_DIR ${CMAKE_SOURCE_DIR}/third_party/ffmpeg)

add_library(avformat SHARED IMPORTED)
set_target_properties(avformat PROPERTIES
    IMPORTED_LOCATION ${FFMPEG_DIR}/lib/${ANDROID_ABI}/libavformat.so)

add_library(avcodec SHARED IMPORTED)
set_target_properties(avcodec PROPERTIES
    IMPORTED_LOCATION ${FFMPEG_DIR}/lib/${ANDROID_ABI}/libavcodec.so)

add_library(avutil SHARED IMPORTED)
set_target_properties(avutil PROPERTIES
    IMPORTED_LOCATION ${FFMPEG_DIR}/lib/${ANDROID_ABI}/libavutil.so)

add_library(swscale SHARED IMPORTED)
set_target_properties(swscale PROPERTIES
    IMPORTED_LOCATION ${FFMPEG_DIR}/lib/${ANDROID_ABI}/libswscale.so)

# OpenCV prebuilt SDK
set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/third_party/opencv/sdk/native/jni)
find_package(OpenCV REQUIRED)

# Native library
add_library(ffmpegface SHARED
    native_bridge.cpp
    rtsp_decoder.cpp
    face_detector.cpp
    video_pipeline.cpp
    frame_converter.cpp
)

target_include_directories(ffmpegface PRIVATE
    ${FFMPEG_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(ffmpegface
    avformat
    avcodec
    avutil
    swscale
    ${OpenCV_LIBS}
    android
    log
    jnigraphics
)
