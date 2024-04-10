#include "FeedExecutor.h"
#include "utils/GeometricCalc.cpp"

using namespace GC;

void init(MotionController* controller, ParamsFitter *fitter, ImageDetector *detector, const double constDis, Ui_ConstMicroDistanceSystem* ui, FeedExecutor* executor) {
    stMotionParams motionParams = fitter->getMotionParams();
    stDetectResult detectRes = detector->getDetectRes();

    double fitCircleRadius = motionParams.fit_radius;
    double tubeRadius = detectRes.tubeRadius;
    double tubeCenterWireDis = detectRes.dis_TubeCenterWire;

    double feedOriginalDis = tubeRadius + constDis;
    double feedActualDis = tubeCenterWireDis + fitCircleRadius*sin(motionParams.alpha);

    controller->spinSetCmdPos(motionParams.alpha);
    controller->spinAbs(0);
    controller->feedSetCmdPos(feedOriginalDis - feedActualDis);  // reverse direction due to definition
    controller->feedAbs(0);

    while(!(controller->getFeedStatus() && controller->getSpinStatus())) ;
    executor->is_init = true;
}

void feedfunc(MotionController* controller, ParamsFitter *fitter, ImageDetector *detector, const double constDis, Ui_ConstMicroDistanceSystem* ui, FeedExecutor* executor) {
    if(!executor->is_init){
        init(controller, fitter, detector, constDis, ui, executor);
    }

    stMotionParams motionParams = fitter->getMotionParams();

    double fitCircleRadius = motionParams.fit_radius;

    F64 centerArr[3], endPosArr[4]; U32 arrAxCnt = 3;
    centerArr[0] = 10 * (fitCircleRadius);  // ppu
    centerArr[1] = 0;
    centerArr[2] = 0;

    endPosArr[0] = 0;
    endPosArr[1] = 0;
    endPosArr[2] = 100000;
    endPosArr[3] = 100000;

    controller->move3DHelixRel(centerArr, endPosArr, &arrAxCnt, 1);
}

FeedExecutor::FeedExecutor() {}

FeedExecutor::FeedExecutor(MotionController* controller, ParamsFitter *fitter, ImageDetector *detector, const double constDis, Ui_ConstMicroDistanceSystem *ui) 
    : controller(controller), fitter(fitter), detector(detector), ui(ui), constDis(constDis) {
}

void FeedExecutor::doInit() {
    std::thread initTrd(init, this->controller, this->fitter, this->detector, this->constDis, this->ui, this);
    initTrd.detach();
}

void FeedExecutor::doFeed() {
    std::thread feedTrd(feedfunc, this->controller, this->fitter, this->detector, this->constDis, this->ui, this);
    feedTrd.detach();
}
