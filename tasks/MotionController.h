#pragma once

#include "utils/ThreadSafeQueue.h"
#include "module/Axis.h"
#include "ImageDetector.h"


class MotionController {
public:
    MotionController() {};
    MotionController(ImageDetector* detector) : detector(detector) {};

private:
    ImageDetector* detector;
};