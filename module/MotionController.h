#pragma once

#include "utils/ThreadSafeQueue.h"
#include "module/Axis.h"

#include <thread>
#include <mutex>
#include <condition_variable>

class MotionController {
public:
    MotionController() {};
    MotionController(Axis spin, Axis vertical, Axis tmp);

    void spinRel(double rad);
    void spinAbs(double rad);
    void spin(U32 dir);
    void spinSetCmdPos(double rad);
    void stopSpin();

    void feedRel(double um);
    void feedAbs(double um);
    void feedSetCmdPos(double um);
    void stopFeed();

    void addPath(U16 MoveCmd, U16 MoveMode, F64 FH, F64 FL, PF64 EndPoint_DataArray, PF64 CenPoint_DataArray, PU32 ArrayElements);
    void movePath();
    void moveCircle(PF64 CenterArray, PF64 EndArray, PU32 pArrayElements, I16 Direction);
    void move3DHelixRel(PF64 CenterArray, PF64 EndArray, PU32 pArrayElements, I16 Direction);

    bool getSpinStatus() { return this->spin_ready; };
    bool getFeedStatus() { return this->feed_ready; };

private:
    Axis axisSpin;
    Axis axisFeed;
    Axis axisTmp;
    HAND groupHand = HAND();

    bool spin_ready = true;
    bool feed_ready = false;

    std::condition_variable spinCv;
    std::condition_variable feedCv;
    std::mutex spinCvMutex;
    std::mutex feedCvMutex;
};