#include "ConstMicroDistanceSystem.h"
#include "MVSCamera/MvCameraControl.h"
#include "Advmot/AdvMotApi.h"
#include <opencv2/opencv.hpp>

void callback(cv::Mat image) {

}

ConstMicroDistanceSystem::ConstMicroDistanceSystem(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui_ConstMicroDistanceSystem)
{
    ui->setupUi(this);

    this->cam = new Camera(0);
    cam->registerImageCallback(CallbackFunctionType(std::bind(&ConstMicroDistanceSystem::cbk, this, std::placeholders::_1)));
    cam->startGrab();
}

ConstMicroDistanceSystem::~ConstMicroDistanceSystem()
{
    delete ui; 
}

void ConstMicroDistanceSystem::cbk(cv::Mat const& image) {
    
}