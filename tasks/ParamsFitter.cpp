#include "ParamsFitter.h"
#include <thread>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

void doFit(ImageDetector *detector, MotionController* motionController, ParamsFitter* fitter) {
    cv::Mat canvas = cv::Mat::zeros(cv::Size(1000,1000), CV_8UC3);

    double pxToUm = detector->getPxToUm();

    const int point_num = 16;
    stDetectResult detectRes[point_num]; vector<cv::Point2f> points;
    for(int i=0; i<point_num; i++) {
        motionController->spinRel(2*3.1415/double(point_num));
        detectRes[i] = detector->getDetectRes();
        points.push_back(detectRes[i].tubeCenter);
        cv::circle(canvas, detectRes[i].tubeCenter, 2, cv::Scalar(0,0,255), 2);
        cv::imshow("fitter", canvas); cv::waitKey(1);
        // fitter->ui->progressBar_fit->setValue(100/point_num*(i+1));
    }

    const int pt_len = points.size();

    cv::Mat A(pt_len, 3, CV_32FC1);
    cv::Mat b(pt_len, 1, CV_32FC1);

    // 下面的两个 for 循环初始化 A 和 b
    for (int i = 0; i < pt_len; i++)
    {
        float *pData = A.ptr<float>(i);

        pData[0] = points[i].x * 2.0f;
        pData[1] = points[i].y * 2.0f;

        pData[2] = 1.0f;
    }

    float *pb = (float *)b.data;

    for (int i = 0; i < pt_len; i++)
    {
        pb[i] = points[i].x * points[i].x + points[i].y * points[i].y;
    }

    // 下面的几行代码就是解超定方程的最小二乘解
    cv::Mat A_Trans;
    transpose(A, A_Trans);

    cv::Mat Inv_A;
    invert(A_Trans * A, Inv_A);

    cv::Mat res = Inv_A * A_Trans * b;

    // 取出圆心和半径
    float x = res.at<float>(0, 0);
    float y = res.at<float>(1, 0);
    float r = (float)sqrt(x * x + y * y + res.at<float>(2, 0));
    stDetectResult detectResNow = detector->getDetectRes();

    float x0 = detectResNow.tubeCenter.x;
    float y0 = detectResNow.tubeCenter.y;

    stMotionParams params;
    params.fit_radius = r * pxToUm;
    // params.alpha = -atan((y0-y)/(x0-x));
    params.alpha = (y0>y) ? M_PI-atan((y0-y)/(x0-x)) : M_PI-atan((y0-y)/(x0-x));  // 
    fitter->pushMotionParams(params);

    cv::circle(canvas, cv::Point2f(x, y), r, cv::Scalar(255,0,255), 1);
    cv::circle(canvas, cv::Point2f(x, y), 2, cv::Scalar(0,255,255), 2);
    cv::circle(canvas, cv::Point2f(x0, y0), 2, cv::Scalar(0,255,0), 2);

    cv::imshow("fitter", canvas); cv::waitKey(0);
}

ParamsFitter::ParamsFitter(ImageDetector *detector, MotionController* motionController) : 
    detector(detector), motionController(motionController) {
}

ParamsFitter::~ParamsFitter() {
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
    thread fitTrd(doFit, this->detector, this->motionController, this);
    fitTrd.detach();
}
