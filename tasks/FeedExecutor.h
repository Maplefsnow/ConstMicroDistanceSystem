#pragma once

#include "ui_ConstMicroDistanceSystem.h"
#include "tasks/ParamsFitter.h"
#include "tasks/ImageDetector.h"
#include "module/MotionController.h"

class FeedExecutor {
public:
    FeedExecutor();
    FeedExecutor(MotionController* controller, ParamsFitter* fitter, ImageDetector* detector, const double constDis, Ui_ConstMicroDistanceSystem* ui);

    void doInit();
    void doFeed();

public:
    bool is_init = false;

private:
    MotionController* controller = nullptr;
    ParamsFitter* fitter = nullptr;
    ImageDetector* detector = nullptr;
    Ui_ConstMicroDistanceSystem* ui = nullptr;

    double constDis;
};