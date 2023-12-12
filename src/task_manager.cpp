#include "task_manager.h"
#include "spd_log.h"

TaskManager::TaskManager()
{
}

void TaskManager::Start()
{
    // https://forum.opencv.org/t/videocapture-open-lag-with-rtsp/10855
    std::vector<int> props;
    props.push_back(cv::CAP_PROP_OPEN_TIMEOUT_MSEC);
    props.push_back(30000);
    props.push_back(cv::CAP_PROP_READ_TIMEOUT_MSEC);
    props.push_back(10000);

    SPD_TRACE("start yolo capture task manager");
    YoloCaptureConfig config;
    config.is_cuda = true;
    config.ffmepg_props = props;
    config.device_code = std::string("51231132");
    config.stream_url = std::string("rtsp://admin:admin12345@10.0.5.64:554/h264/ch33/main/av_stream");

    YoloCapturePtr cap = std::make_shared<YoloCapture>();
    yolo_captures_.push_back(cap);
    cap->Start(config);

    YoloCapturePtr cap2 = std::make_shared<YoloCapture>();
    yolo_captures_.push_back(cap2);
    config.device_code = std::string("51231122");
    config.stream_url = std::string("rtsp://admin:xlgc8888@10.0.5.195:554/h264/ch33/main/av_stream");
    cap2->Start(config);
}

void TaskManager::Stop()
{
    SPD_TRACE("stop all yolo captures");
    for (auto &cap : yolo_captures_)
    {
        cap->Stop();
    }
    yolo_captures_.clear();
}

TaskManager::~TaskManager()
{
}