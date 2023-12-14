#include "capture_config.h"

#include <fstream>
#include <iostream>

void CaptureConfig::Init() {
    try {
        std::ifstream f("../config/yolocap.conf");
        config_ = json::parse(f);
    } catch (...) {
        std::cerr << "error parsing config file" << std::endl;
    }
}

SpdLogConfig CaptureConfig::GetSpdLogConfig() {
    SpdLogConfig log_config;
    auto spd_log_config = config_["spd_log_config"];
    log_config.logger_file_path = spd_log_config["logger_file_path"];
    log_config.logger_name = spd_log_config["logger_name"];
    log_config.level = spd_log_config["level"];
    log_config.max_file_size = spd_log_config["max_file_size"];
    log_config.max_files = spd_log_config["max_files"];
    log_config.mt_security = true;
    return log_config;
}

int CaptureConfig::IsCUDA() {
    return config_["is_cuda"];
}
