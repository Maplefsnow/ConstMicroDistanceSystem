#pragma once

#include "module/Camera.h"
#include "utils/ThreadSafeQueue.h"
#include "ui_ConstMicroDistanceSystem.h"
#include <opencv2/opencv.hpp>
#include <thread>


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
    void process();

private:
    Camera* cam;
    bool is_running = false;
    ThreadSafeQueue<cv::Mat> processedImage = ThreadSafeQueue<cv::Mat>(5);
    Ui_ConstMicroDistanceSystem* ui;

    std::thread* processTrd = nullptr;
};