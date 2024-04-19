#pragma once

#include "ui_ConstMicroDistanceSystem.h"
#include "tasks/ParamsFitter.h"
#include "tasks/ImageDetector.h"
#include "module/MotionController.h"

class FeedExecutor {
public:
    FeedExecutor();
    FeedExecutor(MotionController* controller, ParamsFitter* fitter, ImageDetector* detector, const double constDis, Ui_ConstMicroDistanceSystem* ui);
    ~FeedExecutor();

    void doInit();
    void doFeed();

private:
    void data_record();
    void init_pos();
    void feed_pos();

private:
    bool is_init = false;
    bool is_running = true;
    MotionController* controller = nullptr;
    ParamsFitter* fitter = nullptr;
    ImageDetector* detector = nullptr;
    Ui_ConstMicroDistanceSystem* ui = nullptr;

    std::thread* dataRecordTrd = nullptr;
    std::thread* feedPosTrd = nullptr;
    std::thread* initPosTrd = nullptr;

    double constDis;
};