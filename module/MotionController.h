#pragma once

#include "utils/ThreadSafeQueue.h"
#include "module/Axis.h"

#include <thread>
#include <mutex>
#include <condition_variable>

class MotionController {
public:
    MotionController() {};
    MotionController(Axis spin, Axis vertical);

    void spinRel(double rad);
    void spinAbs(double rad);
    void spin(U32 dir);
    void stopSpin();

    bool getSpinStatus() { return this->spin_ready; };

private:
    Axis axisSpin;
    Axis axisVertical;
    HAND groupHand;

    bool spin_ready = true;
    bool vertical_ready = false;

    std::condition_variable spinCv;
    std::mutex spinCvMutex;
};