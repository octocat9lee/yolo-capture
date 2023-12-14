#ifndef SPD_LOG_H
#define SPD_LOG_H

#include "common_type.h"
#include "spdlog/spdlog.h"

#include <memory>

class SpdLog {
public:
    static SpdLog &Instance() {
        static SpdLog instance;
        return instance;
    }

    void Init(const SpdLogConfig &config);

    std::shared_ptr<spdlog::logger> Logger();

    // Disable copy/move constructors and assignment operators
    SpdLog(const SpdLog &) = delete;
    SpdLog(SpdLog &&) = delete;
    SpdLog &operator=(const SpdLog &) = delete;
    SpdLog &operator=(SpdLog &&) = delete;

private:
    SpdLog() = default;

    std::shared_ptr<spdlog::logger> logger_ptr_;

    void SetLevel(const std::string &level);
};

#define SPD_TRACE(...) SpdLog::Instance().Logger().get()->trace(__VA_ARGS__)
#define SPD_DEBUG(...) SpdLog::Instance().Logger().get()->debug(__VA_ARGS__)
#define SPD_INFO(...) SpdLog::Instance().Logger().get()->info(__VA_ARGS__)
#define SPD_WARN(...) SpdLog::Instance().Logger().get()->warn(__VA_ARGS__)
#define SPD_ERROR(...) SpdLog::Instance().Logger().get()->error(__VA_ARGS__)
#define SPD_CRITICAL(...) SpdLog::Instance().Logger().get()->critical(__VA_ARGS__)

#endif // SPD_LOG_H
