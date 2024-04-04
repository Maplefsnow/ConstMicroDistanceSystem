#include "MotionController.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

#define throwError(x) \
	{\
		ULONG ret = x;\
		if (ret != 0) {\
			char errorMsg[100];\
			Acm_GetErrorMessage(ret, errorMsg, 100);\
			ret = 0;\
			throw errorMsg;\
		}\
	}

MotionController::MotionController(Axis spin, Axis feed, Axis tmp) {
    this->axisSpin = spin;
    this->axisFeed = feed;
    this->axisTmp = tmp;

    throwError(Acm_GpAddAxis(&this->groupHand, this->axisSpin.getHand()));
    throwError(Acm_GpAddAxis(&this->groupHand, this->axisTmp.getHand()));
    throwError(Acm_GpAddAxis(&this->groupHand, this->axisFeed.getHand()));

    throwError(Acm_SetF64Property(this->groupHand, PAR_GpVelLow, 1000));
    throwError(Acm_SetF64Property(this->groupHand, PAR_GpVelHigh, 1000));
}

void MotionController::spinRel(double rad) {
    std::unique_lock<std::mutex> lk(this->spinCvMutex);
    this->spinCv.wait(lk, [this](){ return this->getSpinStatus(); });

    this->spin_ready = false;

    this->axisSpin.relMove(rad * (50000/M_PI));
    while(this->axisSpin.getAxisStatus() != 1) ;

    this->spin_ready = true;
    this->spinCv.notify_all();
}

void MotionController::spinAbs(double rad) {
    std::unique_lock<std::mutex> lk(this->spinCvMutex);
    this->spinCv.wait(lk, [this](){ return this->getSpinStatus(); });

    this->spin_ready = false;

    this->axisSpin.absMove(rad * (50000/M_PI));
    while(this->axisSpin.getAxisStatus() != 1) ;

    this->spin_ready = true;
    this->spinCv.notify_all();
}

void MotionController::spin(U32 dir) {
    std::unique_lock<std::mutex> lk(this->spinCvMutex);
    this->spinCv.wait(lk, [this](){ return this->getSpinStatus(); });

    this->spin_ready = false;
    
    this->axisSpin.contiMove(dir);
}

void MotionController::spinSetCmdPos(double rad) {
    this->axisSpin.setCmdPos(rad * (50000/M_PI));
}

void MotionController::stopSpin() {
    this->axisSpin.stop();
    while(this->axisSpin.getAxisStatus() != 1) ;
    this->spin_ready = true;
}

void MotionController::feedRel(double um) {
    std::unique_lock<std::mutex> lk(this->feedCvMutex);
    this->feedCv.wait(lk, [this](){ return this->getFeedStatus(); });

    this->feed_ready = false;

    this->axisFeed.relMove(um * 5);
    while(this->axisFeed.getAxisStatus() != 1) ;

    this->feed_ready = true;
    this->feedCv.notify_all();
}

void MotionController::feedAbs(double um) {
    std::unique_lock<std::mutex> lk(this->feedCvMutex);
    this->feedCv.wait(lk, [this](){ return this->getFeedStatus(); });

    this->feed_ready = false;

    this->axisFeed.absMove(um * 5);
    while(this->axisFeed.getAxisStatus() != 1) ;

    this->feed_ready = true;
    this->feedCv.notify_all();
}

void MotionController::feedSetCmdPos(double um) {
    this->axisFeed.setCmdPos(um * 5);
}

void MotionController::stopFeed() {
    this->axisFeed.stop();
    while(this->axisSpin.getAxisStatus() != 1) ;
    this->spin_ready = true;
}

void MotionController::addPath(U16 MoveCmd, U16 MoveMode, F64 FH, F64 FL, PF64 EndPoint_DataArray, PF64 CenPoint_DataArray, PU32 ArrayElements) {
    throwError(Acm_GpAddPath(this->groupHand, MoveCmd, MoveMode, FH, FL, EndPoint_DataArray, CenPoint_DataArray, ArrayElements));
}

void MotionController::movePath() {
    throwError(Acm_GpMovePath(this->groupHand, NULL));
}

void MotionController::moveCircle(PF64 CenterArray, PF64 EndArray, PU32 pArrayElements, I16 Direction) {
    throwError(Acm_GpMoveCircularRel(this->groupHand, CenterArray, EndArray, pArrayElements, Direction));
}

void MotionController::move3DHelixRel(PF64 CenterArray, PF64 EndArray, PU32 pArrayElements, I16 Direction) {
    throwError(Acm_GpMoveHelixRel(this->groupHand, CenterArray, EndArray, pArrayElements, Direction));
}
