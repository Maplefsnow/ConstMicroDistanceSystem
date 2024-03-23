#pragma once

#include "module/Camera.h"
#include "utils/ThreadSafeQueue.h"
#include <opencv2/opencv.hpp>

using namespace cv;


class ImageProcessor {
public:
    ImageProcessor();
    ImageProcessor(Camera* cam);
    ~ImageProcessor();
    void start() { this->is_running = true; this->run(); };
    void stop() { this->is_running = false; };
    bool status() { return this->is_running; };

    void pushImageBuffer(cv::Mat image) { this->processedImage.push(image); };
    cv::Mat getOneImageWait();

private:
    void run();

private:
    Camera* cam;
    bool is_running = false;
    ThreadSafeQueue<cv::Mat> processedImage = ThreadSafeQueue<cv::Mat>(5);
};