#include "FeedExecutor.h"
#include "utils/GeometricCalc.cpp"

using namespace GC;

void execute(MotionController* controller, ParamsFitter *fitter, ImageDetector *detector, Ui_ConstMicroDistanceSystem *ui, const double constDis) {
    const double realWireDia = 100.0;  // um

    stMotionParams motionParams = fitter->getMotionParams();
    stDetectResult detectRes = detector->getDetectRes();

    double detectWireDia = getDisByLineLine(detectRes.wireUpEdge, detectRes.wireDownEdge);
    double pxToReal = realWireDia/detectWireDia;  // decide the ratio scale

    double tubeRadius = detectRes.tubeRadius*pxToReal;  //um
    double fitCircleRadius = motionParams.fit_radius*pxToReal;  // um
    double tubeCenterWireDis = detectRes.dis_TubeCenterWire*pxToReal;  // um

    double feedOriginalDis = tubeRadius + constDis;
    double feedActualDis = tubeCenterWireDis + fitCircleRadius*sin(motionParams.alpha);

    controller->spinSetCmdPos(motionParams.alpha);
    controller->spinAbs(0);
    controller->feedSetCmdPos(feedOriginalDis - feedActualDis);  // reverse direction due to definition
    controller->feedAbs(0);


    F64 centerArr[3], endPosArr[4]; U32 arrAxCnt = 3;
    centerArr[0] = 10 * fitCircleRadius;  // ppu
    centerArr[1] = 0;
    centerArr[2] = 0;

    endPosArr[0] = 0;
    endPosArr[1] = 0;
    endPosArr[2] = 100000;
    endPosArr[3] = 100000;

    controller->move3DHelixRel(centerArr, endPosArr, &arrAxCnt, 0);
}

FeedExecutor::FeedExecutor()
{
}

FeedExecutor::FeedExecutor(MotionController* controller, ParamsFitter *fitter, ImageDetector *detector, Ui_ConstMicroDistanceSystem *ui) 
    : controller(controller), fitter(fitter), detector(detector), ui(ui)
{
}

void FeedExecutor::run()
{
}
