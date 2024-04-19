#include "ParamsFitter.h"
#include "utils/GeometricCalc.cpp"
#include <thread>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;
using namespace GC;

void ParamsFitter::do_fit() {
    cv::Mat canvas = cv::Mat::zeros(cv::Size(1000,1000), CV_8UC3);

    double pxToUm = detector->getPxToUm();

    const int point_num = 16;
    stDetectResult detectRes[point_num]; vector<cv::Point2f> points;
    for(int i=0; i<point_num; i++) {
        motionController->spinRel(2*M_PI/double(point_num));
        while(!motionController->getSpinStatus()) ;
        detectRes[i] = detector->getDetectRes();
        points.push_back(detectRes[i].tubeCenter);
#ifdef OPENCV_SHOW_IMAGES
        cv::circle(canvas, detectRes[i].tubeCenter, 2, cv::Scalar(0,0,255), 2);
        cv::imshow("fitter", canvas); cv::waitKey(1);
        // fitter->ui->progressBar_fit->setValue(100/point_num*(i+1));
#endif
    }

    Point2f center; double x, y, r = 0.0;
    Vec3f cir = Fitter::fitCircleByLeastSquareGradDescent(points);
    x = cir[0];
    y = cir[1];
    r = cir[2];

    stDetectResult detectResNow = detector->getDetectRes();

    float x0 = detectResNow.tubeCenter.x;
    float y0 = detectResNow.tubeCenter.y;

    stMotionParams params;
    params.fit_radius = r * pxToUm;
    params.alpha = (x0<x) ? M_PI-atan((y0-y)/(x0-x)) : -atan((y0-y)/(x0-x));
    this->pushMotionParams(params);

#ifdef OPENCV_SHOW_IMAGES
    cv::circle(canvas, cv::Point2f(x, y), r, cv::Scalar(255,0,255), 1);
    cv::circle(canvas, cv::Point2f(x, y), 2, cv::Scalar(0,255,255), 2);
    cv::circle(canvas, cv::Point2f(x0, y0), 2, cv::Scalar(0,255,0), 2);

    cv::imshow("fitter", canvas); cv::waitKey(0);
#endif
}

ParamsFitter::ParamsFitter(ImageDetector *detector, MotionController* motionController) : 
    detector(detector), motionController(motionController) {
}

ParamsFitter::~ParamsFitter() {
    this->fitTrd->join();
}

stMotionParams ParamsFitter::getMotionParams() {
    stMotionParams params;
    if(this->params_queue.try_pop(params)){
        this->motionParams = params;
        return params;
    } else {
        return this->motionParams;
    }
}

void ParamsFitter::run() {
    this->fitTrd = new std::thread(do_fit, this);
}
