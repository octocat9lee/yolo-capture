#ifndef CAPTURE_CONFIG_H
#define CAPTURE_CONFIG_H

#include "common_type.h"
#include "nlohmann/json.hpp"

class CaptureConfig {
    using json = nlohmann::json;

public:
    static CaptureConfig &Instance() {
        static CaptureConfig instance;
        return instance;
    }

    void Init();

    SpdLogConfig GetSpdLogConfig();

    int IsCUDA();

    // Disable copy/move constructors and assignment operators
    CaptureConfig(const CaptureConfig &) = delete;
    CaptureConfig(CaptureConfig &&) = delete;
    CaptureConfig &operator=(const CaptureConfig &) = delete;
    CaptureConfig &operator=(CaptureConfig &&) = delete;

private:
    CaptureConfig() = default;

    json config_;
};

#endif // CAPTURE_CONFIG_H
