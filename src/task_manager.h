#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "yolo_capture.h"

#include <memory>

class TaskManager
{
    using YoloCapturePtr = std::shared_ptr<YoloCapture>;

public:
    TaskManager();

    void Start();

    void Stop();

    ~TaskManager();

private:
    std::vector<YoloCapturePtr> yolo_captures_;
};

#endif // TASK_MANAGER_H