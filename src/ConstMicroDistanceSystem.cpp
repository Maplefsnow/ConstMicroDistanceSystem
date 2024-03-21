#include "ConstMicroDistanceSystem.h"
#include "MVSCamera/MvCameraControl.h"
#include "Advmot/AdvMotApi.h"
#include <opencv2/opencv.hpp>
#include <QMessageBox>

ConstMicroDistanceSystem::ConstMicroDistanceSystem(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui_ConstMicroDistanceSystem)
{
    ui->setupUi(this);

    CameraParam params;

    try
    {
        this->cam = new Camera(0);
        this->cam->setParams(params);
    }
    catch(const char* e)
    {
        std::cerr << e << '\n';
        QMessageBox::information(this, "qwq", "qwq");
    }
    
    this->cam->registerImageCallback(CallbackFunctionType(std::bind(&ConstMicroDistanceSystem::cbk, this, std::placeholders::_1)));

    this->imageProcessor = new ImageProcessor(this->cam);
    this->imageProcessor->start();
}

ConstMicroDistanceSystem::~ConstMicroDistanceSystem()
{
    this->imageProcessor->stop();

    delete ui;
    delete this->cam;
}

void ConstMicroDistanceSystem::cbk(cv::Mat const& image) {
    cv::Mat imgShow; 

    cv::cvtColor(image, imgShow, cv::COLOR_BGR2RGB);
    QImage qimg((uchar*)imgShow.data, imgShow.cols, imgShow.rows, imgShow.step, QImage::Format_RGB888);

    ui->label->setPixmap(QPixmap::fromImage(qimg));
}

void ConstMicroDistanceSystem::onSwitchCamGrabClicked() {
    if(this->cam->getGrabStatus()) {
        this->cam->stopGrab();
        this->ui->pushButton_switchGrab->setText(QString("开始采集"));
    } else {
        this->cam->startGrab();
        this->ui->pushButton_switchGrab->setText(QString("停止采集"));
    }
}

void ConstMicroDistanceSystem::onTakePhotoClicked() {
    QString path = this->photoSavePath + "/" + QString::number(this->photoNum++) + ".jpg";

    cv::Mat image;
    if(!this->cam->getOneImageOrFail(image)){
        QMessageBox::critical(this, "错误", "缓冲区内无图像，请先开始采集。", QMessageBox::Ok);
        return;
    }

    cv::imwrite(path.toStdString(), this->cam->getOneImageWait());
}

void ConstMicroDistanceSystem::onSwitchRecordClicked() {
    if(this->is_recording) {
        this->camRecorder->stop();
        this->camRecorder = nullptr;
        this->ui->pushButton_switchRecord->setText(QString("开始录制"));
        this->is_recording = false;

        QMessageBox::information(this, "录制完成", "录制完成，视频文件存放在 " + this->videoSavePath, QMessageBox::Ok);
    } else {
        QString path = this->videoSavePath + "/" + QString::number(this->videoNum++) + ".avi";
        this->camRecorder = new CamRecorder(this->cam, path);
        this->camRecorder->start();
        this->ui->pushButton_switchRecord->setText(QString("停止录制"));
        this->is_recording = true;
    }
}

void ConstMicroDistanceSystem::onPhotoLocationTriggered() {
    QString path = QFileDialog::getExistingDirectory(this, "选择保存文件夹", QDir::homePath());
    if(!path.isEmpty()) {
        this->photoSavePath = path;
    } else {
        return ;
    }
}

void ConstMicroDistanceSystem::onVideoLocationTriggered() {
    QString path = QFileDialog::getExistingDirectory(this, "选择保存文件夹", QDir::homePath());
    if(!path.isEmpty()) {
        this->videoSavePath = path;
    } else {
        return ;
    }
}