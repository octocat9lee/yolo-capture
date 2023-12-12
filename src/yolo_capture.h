#ifndef YOLO_CAPTURE_H
#define YOLO_CAPTURE_H

#include "common_type.h"

#include <thread>
#include <string>

class YoloCapture
{
public:
    YoloCapture();

    ~YoloCapture();

    void LoadClassList();

    void LoadNet(cv::dnn::Net &net, bool is_cuda);

    cv::Mat FormatYolov5(const cv::Mat &source);

    void Detect(cv::dnn::Net &net, const cv::Mat &image, std::vector<YoloDetection> &output);

    void DrawDetection(const cv::Mat &image, const std::vector<YoloDetection> &detections);

    void Start(const YoloCaptureConfig &config);

    void Stop();

private:
    void Run();

    void SaveFrameAsJpg(const cv::Mat &frame);

private:
    std::vector<std::string> class_name_;
    YoloCaptureConfig yolo_config_;
    std::thread cap_thread_;
    bool is_capturing_;
};

#endif // YOLO_CAPTURE_H
