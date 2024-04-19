#include "FeedExecutor.h"
#include "utils/GeometricCalc.cpp"
#include <fstream>

using namespace GC;

void FeedExecutor::init_pos() {
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
    this->is_init = true;
}

void FeedExecutor::feed_pos() {
    if(!this->is_init) this->init_pos();

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

void FeedExecutor::data_record() {
    std::cout << "DATA RECORDING..." << std::endl;

    std::fstream sout;
    sout.open("C:\\Users\\aicter\\Documents\\data.txt", std::ios::out);

    while(this->is_running) {
        stDetectResult res = this->detector->getDetectRes();
        sout << res.dis_TubeWire << std::endl;
    }

    sout.close();

    std::cout << "END DATA RECORD!" << std::endl;
}

FeedExecutor::FeedExecutor() {}

FeedExecutor::FeedExecutor(MotionController* controller, ParamsFitter *fitter, ImageDetector *detector, const double constDis, Ui_ConstMicroDistanceSystem *ui) 
    : controller(controller), fitter(fitter), detector(detector), ui(ui), constDis(constDis) {
}

FeedExecutor::~FeedExecutor() {
    this->is_running = false;
    this->dataRecordTrd->join();
    this->initPosTrd->join();
    this->feedPosTrd->join();
}

void FeedExecutor::doInit() {
    this->initPosTrd = new std::thread(init_pos, this);
    this->initPosTrd->detach();
}

void FeedExecutor::doFeed() {
    this->feedPosTrd = new std::thread(feed_pos, this);
    this->dataRecordTrd = new std::thread(data_record, this);
}
