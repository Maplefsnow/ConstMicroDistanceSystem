#pragma once

#include "module/Camera.h"
#include "utils/ThreadSafeQueue.h"
#include <opencv2/opencv.hpp>

using namespace cv;


struct stDetectResult {
    cv::Point2f tubeCenter;
    float tubeRadius;
    cv::Vec3f wireUpEdge;
    cv::Vec3f wireDownEdge;
};


class ImageProcessor {
public:
    ImageProcessor();
    ImageProcessor(Camera* cam);
    ~ImageProcessor();
    void start() { this->run(); this->is_running = true; };
    void stop() { this->is_running = false; };
    bool status() { return this->is_running; };

    void pushImageBuffer(cv::Mat image) { this->processedImage.push(image); };
    void pushDetectResBuffer(stDetectResult res) { this->detectRes.push(res); };
    cv::Mat getOneImageWait();

private:
    void run();

private:
    Camera* cam;
    bool is_running = false;
    ThreadSafeQueue<cv::Mat> processedImage = ThreadSafeQueue<cv::Mat>(5);
    ThreadSafeQueue<stDetectResult> detectRes = ThreadSafeQueue<stDetectResult>(5);
};