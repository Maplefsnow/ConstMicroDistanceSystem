#include "module/Camera.h"
#include <opencv2/opencv.hpp>
#include <QString>
#include <thread>

class CamRecorder {
public:
    CamRecorder();
    CamRecorder(Camera* cam, QString path);
    ~CamRecorder();
    void start() { this->run(); this->is_running = true; };
    void stop() { this->is_running = false; };
    bool status() { return this->is_running; };

private:
    void run();

private:
    Camera* cam;
    QString path;
    bool is_running = false;
};