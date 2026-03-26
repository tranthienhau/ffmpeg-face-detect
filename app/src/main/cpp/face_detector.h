#pragma once

#include <string>
#include <vector>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>

struct FaceRect {
    int x, y, width, height;
};

class FaceDetector {
public:
    bool init(const std::string& cascade_path);
    std::vector<FaceRect> detect(const cv::Mat& rgb_frame);
    void draw_faces(cv::Mat& frame, const std::vector<FaceRect>& faces);

private:
    cv::CascadeClassifier cascade_;
    cv::Mat gray_;
};
