#pragma once

#include "ui_ConstMicroDistanceSystem.h"
#include "module/Camera.h"
#include <opencv2/opencv.hpp>
#include <QString>
#include <thread>

class CamRecorder {
public:
    CamRecorder();
    CamRecorder(Camera* cam, QString path, Ui_ConstMicroDistanceSystem* ui, int fps = 20);
    ~CamRecorder();
    void start() { this->is_running = true; this->run(); };
    void stop() { this->is_running = false; };
    bool status() { return this->is_running; };

private:
    void run();

private:
    Camera* cam;
    QString path;
    int fps = 20;
    bool is_running = false;
    Ui_ConstMicroDistanceSystem* ui;
};