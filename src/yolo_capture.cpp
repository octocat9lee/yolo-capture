#include "yolo_capture.h"
#include "spd_log.h"

#include <fstream>

const float INPUT_WIDTH = 640.0;
const float INPUT_HEIGHT = 640.0;
const float SCORE_THRESHOLD = 0.2;
const float NMS_THRESHOLD = 0.4;
const float CONFIDENCE_THRESHOLD = 0.4;

const std::vector<cv::Scalar> colors = {
    cv::Scalar(255, 255, 0),
    cv::Scalar(0, 255, 0),
    cv::Scalar(0, 255, 255),
    cv::Scalar(255, 0, 0)};

YoloCapture::YoloCapture() : is_capturing_(false)
{
}

YoloCapture::~YoloCapture()
{
    SPD_TRACE("destructor yolo capture, device code: {}", yolo_config_.device_code);
    if (is_capturing_)
    {
        Stop();
    }
}

void YoloCapture::Start(const YoloCaptureConfig &config)
{
    yolo_config_ = config;
    if (!is_capturing_)
    {
        is_capturing_ = true;
        SPD_TRACE("start yolo capture, device code: {}", yolo_config_.device_code);
        cap_thread_ = std::thread(&YoloCapture::Run, this);
    }
}

void YoloCapture::LoadClassList()
{
    std::ifstream ifs("../config/classes.txt");
    std::string line;
    while (getline(ifs, line))
    {
        class_name_.push_back(line);
    }
}

void YoloCapture::LoadNet(cv::dnn::Net &net, bool is_cuda)
{
    if (is_cuda)
    {
        // https://github.com/opencv/opencv/issues/17852
        int device_count = cv::cuda::getCudaEnabledDeviceCount();
        SPD_DEBUG("cuda device count: {}", device_count);
        cv::cuda::setDevice(device_count - 1);
    }

    net = cv::dnn::readNet("../config/yolov5s.onnx");
    if (is_cuda)
    {
        SPD_DEBUG("running on CUDA");
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA_FP16);
    }
    else
    {
        SPD_DEBUG("running on CPU");
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    }
}

void YoloCapture::SaveFrameAsJpg(const cv::Mat &frame)
{
    auto time_point = std::chrono::system_clock::now();
    auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(time_point);
    auto time_since_epoch = ms.time_since_epoch();
    long long milliseconds = time_since_epoch.count();
    std::ostringstream oss;
    oss << std::setw(15) << std::setfill('0') << std::to_string(milliseconds) << ".jpg";
    std::string filename = oss.str();
    cv::imwrite(filename, frame);
}

cv::Mat YoloCapture::FormatYolov5(const cv::Mat &source)
{
    int col = source.cols;
    int row = source.rows;
    int _max = MAX(col, row);
    cv::Mat result = cv::Mat::zeros(_max, _max, CV_8UC3);
    source.copyTo(result(cv::Rect(0, 0, col, row)));
    return result;
}

void YoloCapture::Detect(cv::dnn::Net &net, const cv::Mat &image, std::vector<YoloDetection> &output)
{
    cv::Mat blob;
    auto input_image = FormatYolov5(image);
    cv::dnn::blobFromImage(input_image, blob, 1. / 255., cv::Size(INPUT_WIDTH, INPUT_HEIGHT), cv::Scalar(), true, false);
    net.setInput(blob);
    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());

    float x_factor = input_image.cols / INPUT_WIDTH;
    float y_factor = input_image.rows / INPUT_HEIGHT;

    const int dimensions = 85;
    const int rows = 25200;

    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    float *data = (float *)outputs[0].data;
    for (int i = 0; i < rows; ++i)
    {
        float confidence = data[4];
        if (confidence >= CONFIDENCE_THRESHOLD)
        {
            float *classes_scores = data + 5;
            cv::Mat scores(1, class_name_.size(), CV_32FC1, classes_scores);
            cv::Point class_id;
            double max_class_score;
            minMaxLoc(scores, 0, &max_class_score, 0, &class_id);
            if (max_class_score > SCORE_THRESHOLD)
            {
                confidences.push_back(confidence);
                class_ids.push_back(class_id.x);
                float x = data[0];
                float y = data[1];
                float w = data[2];
                float h = data[3];
                int left = int((x - 0.5 * w) * x_factor);
                int top = int((y - 0.5 * h) * y_factor);
                int width = int(w * x_factor);
                int height = int(h * y_factor);
                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }
        data += 85;
    }

    std::vector<int> nms_result;
    cv::dnn::NMSBoxes(boxes, confidences, SCORE_THRESHOLD, NMS_THRESHOLD, nms_result);
    for (int i = 0; i < nms_result.size(); i++)
    {
        int idx = nms_result[i];
        YoloDetection result;
        result.class_id = class_ids[idx];
        result.confidence = confidences[idx];
        result.box = boxes[idx];
        output.push_back(result);
    }
}

void YoloCapture::DrawDetection(const cv::Mat &frame, const std::vector<YoloDetection> &detections)
{
    for (int i = 0; i < detections.size(); ++i)
    {
        auto detection = detections[i];
        auto box = detection.box;
        auto classId = detection.class_id;
        const auto color = colors[classId % colors.size()];
        cv::rectangle(frame, box, color, 3);
        cv::rectangle(frame, cv::Point(box.x, box.y - 20), cv::Point(box.x + box.width, box.y), color, cv::FILLED);
        cv::putText(frame, class_name_[classId].c_str(), cv::Point(box.x, box.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
    }
    // SaveFrameAsJpg(frame);
}

void YoloCapture::Stop()
{
    SPD_TRACE("stop yolo capture, device code: {}", yolo_config_.device_code);
    if (is_capturing_)
    {
        is_capturing_ = false;
        SPD_TRACE("join yolo capture ended, device code: {}", yolo_config_.device_code);
        cap_thread_.join();
    }
}

void YoloCapture::Run()
{
    // FIXME:
    // If adjust the net parameter as the class member, the program will crash.
    // terminate called after throwing an instance of 'cv::dnn::cuda4dnn::csl::CUDAException'
    cv::dnn::Net net;
    LoadClassList();
    LoadNet(net, yolo_config_.is_cuda);

    SPD_DEBUG("stream url: {}", yolo_config_.stream_url);
    cv::VideoCapture capture(yolo_config_.stream_url, cv::CAP_FFMPEG, yolo_config_.ffmepg_props);
    if (!capture.isOpened())
    {
        SPD_ERROR("open stream url failed: {}", yolo_config_.stream_url);
    }

    SPD_DEBUG("open stream url success: {}", yolo_config_.stream_url);

    int skip_frames = 0;
    int total_frames = 0;

    cv::Mat frame;
    while (is_capturing_)
    {
        capture.read(frame);
        if (frame.empty())
        {
            SPD_DEBUG("read stream url failed: {}", yolo_config_.stream_url);
            continue;
        }
        skip_frames++;
        if (skip_frames == 10)
        {
            skip_frames = 0;
            total_frames++;
            std::vector<YoloDetection> detections;
            Detect(net, frame, detections);
            DrawDetection(frame, detections);
            SPD_DEBUG("device code: {}, detect frame: {}", yolo_config_.device_code, total_frames);
        }
    }
    
    capture.release();
    SPD_DEBUG("yolo capture ended, code: {}, total detect frame: {}", yolo_config_.device_code, total_frames);
}
