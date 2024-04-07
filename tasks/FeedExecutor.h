#pragma once

#include "ui_ConstMicroDistanceSystem.h"
#include "tasks/ParamsFitter.h"
#include "tasks/ImageDetector.h"
#include "module/MotionController.h"

class FeedExecutor {
public:
    FeedExecutor();
    FeedExecutor(MotionController* controller, ParamsFitter* fitter, ImageDetector* detector, Ui_ConstMicroDistanceSystem* ui);
    void start() { this->is_running = true; this->run(); };
    void stop() { this->is_running = false; };
    bool status() { return this->is_running; };

private:
    void run();

private:
    bool is_running = false;
    MotionController* controller = nullptr;
    ParamsFitter* fitter = nullptr;
    ImageDetector* detector = nullptr;
    Ui_ConstMicroDistanceSystem* ui;
};