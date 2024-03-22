#pragma once

#include "ImageProcessor.h"
#include "utils/ThreadSafeQueue.h"
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
    ImageDetector(ImageProcessor* processor);
    ~ImageDetector();
    void start() { this->run(); this->is_running = true; };
    void stop() { this->is_running = false; };
    bool status() { return this->is_running; };
    void pushDetectResBuffer(stDetectResult res) { this->detectRes.push(res); };

private:
    void run();

private:
    ImageProcessor* processor;
    bool is_running = false;
    ThreadSafeQueue<stDetectResult> detectRes = ThreadSafeQueue<stDetectResult>(5);
};
