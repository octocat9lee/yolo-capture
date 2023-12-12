#ifndef COMMON_TYPE_H
#define COMMON_TYPE_H

#include <string>
#include <opencv2/opencv.hpp>

struct SpdLogConfig
{
    std::string logger_file_path;
    std::string logger_name;
    std::string level;
    size_t max_file_size;
    size_t max_files;
    bool mt_security;
};

struct YoloDetection
{
    int class_id;
    float confidence;
    cv::Rect box;
};

struct YoloCaptureConfig
{
    bool is_cuda;
    std::string device_code;
    std::string stream_url;
    std::vector<int> ffmepg_props;
};

struct CameraInfos
{
    std::string yolo_model_name;
    std::string device_code;
    std::string stream_url;
};

#endif // COMMON_TYPE_H
