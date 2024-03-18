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

    try
    {
        this->cam = new Camera(0);
    }
    catch(const char* e)
    {
        std::cerr << e << '\n';
    }
    
    this->cam->registerImageCallback(CallbackFunctionType(std::bind(&ConstMicroDistanceSystem::cbk, this, std::placeholders::_1)));
    this->cam->startGrab();
}

ConstMicroDistanceSystem::~ConstMicroDistanceSystem()
{
    delete ui;
    delete this->cam;
}

void ConstMicroDistanceSystem::cbk(cv::Mat const& image) {
    std::cout << "image get!" << std::endl;

    cv::Mat imgShow;

    cv::resize(image, imgShow, cv::Size(1024, 768));

    cv::imshow("qwq", imgShow);
    cv::waitKey(1);

    cv::cvtColor(image, imgShow, cv::COLOR_BGR2RGB);
    QImage qimg((uchar*)imgShow.data, imgShow.cols, imgShow.rows, imgShow.step, QImage::Format_RGB888);

    

    ui->label->setPixmap(QPixmap::fromImage(qimg));
}