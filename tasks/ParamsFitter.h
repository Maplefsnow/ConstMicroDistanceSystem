#pragma once

#include <opencv2/opencv.hpp>
#include "ImageDetector.h"
#include "ui_ConstMicroDistanceSystem.h"
#include "module/MotionController.h"
#include "utils/ThreadSafeQueue.h"
#include <vector>

struct stMotionParams {
    double alpha;
    double fit_radius;  // um
};


class ParamsFitter {
public:
    ParamsFitter() {};
    ParamsFitter(ImageDetector* detector, MotionController* motionController);
    ~ParamsFitter();

    stMotionParams getMotionParams();
    void pushMotionParams(stMotionParams params) { this->params_queue.push(params); this->motionParams = params; };
    void run();

private:
    void do_fit();

private:
    Ui_ConstMicroDistanceSystem* ui;
    ImageDetector* detector;
    MotionController* motionController;

    stMotionParams motionParams;
    ThreadSafeQueue<stMotionParams> params_queue = ThreadSafeQueue<stMotionParams>(1);
    std::thread* fitTrd = nullptr;
};
