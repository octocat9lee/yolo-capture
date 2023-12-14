#include <chrono>
#include <thread>

#include "camera_config.h"
#include "capture_config.h"
#include "spd_log.h"
#include "task_manager.h"

int main(int argc, char **argv) {
    CaptureConfig::Instance().Init();

    SpdLogConfig log_config = CaptureConfig::Instance().GetSpdLogConfig();

    std::cout << "log level: " << log_config.level << std::endl;

    SpdLog::Instance().Init(log_config);

    SPD_TRACE("start main yolo capture");

    CameraConfig::Instance().Init();

    TaskManager manager;
    manager.Start();
    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    manager.Stop();

    for (int i = 0; i < 5; ++i) {
        SPD_TRACE("main continue running");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    SPD_TRACE("ended main yolo capture");
    return 0;
}
