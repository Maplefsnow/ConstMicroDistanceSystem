#include "MotionController.h"
#include <iostream>

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

MotionController::MotionController(Axis spin, Axis vertical) {
    this->axisSpin = spin;
    this->axisVertical = vertical;

    ULONG qwq = Acm_GpAddAxis(&this->groupHand, this->axisSpin.getHand());
    std::cout << qwq << std::endl;
    Acm_GpAddAxis(&this->groupHand, this->axisVertical.getHand());
}

void MotionController::spinRel(double rad) {
    std::unique_lock<std::mutex> lk(this->spinCvMutex);
    this->spinCv.wait(lk, [this](){ return this->getSpinStatus(); });

    this->spin_ready = false;

    // TODO: convert from rad to PPU
    this->axisSpin.relMove(rad);
    while(this->axisSpin.getAxisStatus() != 1) ;

    this->spin_ready = true;
    this->spinCv.notify_all();
}

void MotionController::spinAbs(double rad) {
    // TODO: convert from rad to PPU
    this->axisSpin.absMove(rad);
}

void MotionController::spin(U32 dir) {
    std::unique_lock<std::mutex> lk(this->spinCvMutex);
    this->spinCv.wait(lk, [this](){ return this->getSpinStatus(); });

    this->spin_ready = false;
    
    this->axisSpin.contiMove(dir);
}

void MotionController::stopSpin() {
    this->axisSpin.stop();
    while(this->axisSpin.getAxisStatus() != 1) ;
    this->spin_ready = true;
}
