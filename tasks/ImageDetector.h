#pragma once

#include "ImageProcessor.h"
#include "utils/ThreadSafeQueue.h"
#include "ui_ConstMicroDistanceSystem.h"
#include <thread>
#include <mutex>
#include <vector>
#include <opencv2/opencv.hpp>


struct stDetectResult {
    cv::Point2f tubeCenter;
    float tubeRadius;  // um
    
    cv::Vec3f wireUpEdge;
    cv::Vec3f wireDownEdge;

    float dis_TubeWire;  // um
    float dis_TubeCenterWire;  // um
};

class ImageDetector {
public:
    ImageDetector() {} ;
    ImageDetector(ImageProcessor* processor, double realWireDia, Ui_ConstMicroDistanceSystem* ui);
    ~ImageDetector();
    void start() { this->is_running = true; this->run(); };
    void stop() { this->is_running = false; };
    bool status() { return this->is_running; };

    void setPxToUm(double val)  { std::unique_lock<std::mutex> lk(this->mtxPxToUm); this->pxToUm = val; };
    double getPxToUm()          { std::unique_lock<std::mutex> lk(this->mtxPxToUm); return this->pxToUm; };

    void pushDetectResBuffer(stDetectResult res) { this->detectRes.push(res); };
    stDetectResult getDetectRes();

private:
    void run();
    void detect();

private:
    ImageProcessor* processor;
    bool is_running = false;
    ThreadSafeQueue<stDetectResult> detectRes = ThreadSafeQueue<stDetectResult>(5);
    Ui_ConstMicroDistanceSystem* ui;
    double realWireDia = 100.0;
    double pxToUm = 0.0;

    std::mutex mtxPxToUm;
    std::thread* detectTrd = nullptr;
};
