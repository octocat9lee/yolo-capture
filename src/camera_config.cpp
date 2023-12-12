#include "camera_config.h"
#include "csv.h"
#include "spd_log.h"

#include <random>
#include <vector>
#include <algorithm>

void CameraConfig::Init()
{
    // read camera config for csv file
    io::CSVReader<3> in("../config/camera.csv");
    in.read_header(io::ignore_missing_column, "model", "code", "url");
    std::string model_name;
    std::string device_code;
    std::string stream_url;
    while (in.read_row(model_name, device_code, stream_url))
    {
        if (!(model_name.empty() || device_code.empty() || stream_url.empty()))
        {
            CameraInfos camera_info;
            camera_info.yolo_model_name = model_name;
            camera_info.device_code = device_code;
            camera_info.stream_url = stream_url;
            camera_list_.push_back(camera_info);
        }
        else
        {
            SPD_ERROR("camera config error, model_name: {}, device_code: {}, stream_url: {}",
                      model_name, device_code, stream_url);
        }
    }

    // 对输入的摄像头列表进行随机排序，防止每次启动程序时，摄像头的顺序都一样
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(camera_list_.begin(), camera_list_.end(), g);

    // print camera list to log
    for(const auto &camera_info : camera_list_)
    {
        SPD_INFO("camera config, model_name: {}, device_code: {}, stream_url: {}",
                 camera_info.yolo_model_name, camera_info.device_code, camera_info.stream_url);
    }
}

std::vector<CameraInfos> CameraConfig::GetCameraList()
{
    return camera_list_;
}
