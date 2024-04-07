#pragma once

#include "ImageProcessor.h"
#include "utils/ThreadSafeQueue.h"
#include "ui_ConstMicroDistanceSystem.h"
#include <thread>
#include <vector>
#include <opencv2/opencv.hpp>


struct stDetectResult {
    cv::Point2f tubeCenter;
    float tubeRadius;
    
    cv::Vec3f wireUpEdge;
    cv::Vec3f wireDownEdge;

    float dis_TubeWire;
    float dis_TubeCenterWire;
};

class ImageDetector {
public:
    ImageDetector() {} ;
    ImageDetector(ImageProcessor* processor, Ui_ConstMicroDistanceSystem* ui);
    ~ImageDetector();
    void start() { this->is_running = true; this->run(); };
    void stop() { this->is_running = false; };
    bool status() { return this->is_running; };

    void pushDetectResBuffer(stDetectResult res) { this->detectRes.push(res); };
    stDetectResult getDetectRes();

private:
    void run();

private:
    ImageProcessor* processor;
    bool is_running = false;
    ThreadSafeQueue<stDetectResult> detectRes = ThreadSafeQueue<stDetectResult>(5);
    Ui_ConstMicroDistanceSystem* ui;
};
