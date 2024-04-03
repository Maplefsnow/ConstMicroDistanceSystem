#include "ConstMicroDistanceSystem.h"
#include "MVSCamera/MvCameraControl.h"
#include "Advmot/AdvMotApi.h"
#include <opencv2/opencv.hpp>
#include <QMessageBox>

ConstMicroDistanceSystem::ConstMicroDistanceSystem(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui_ConstMicroDistanceSystem) {
    ui->setupUi(this);

    int nRet = MV_OK;

    MV_CC_DEVICE_INFO_LIST stDeviceList;
    memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

    MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
    if(stDeviceList.nDeviceNum == 0) {
        QMessageBox::warning(this, "设备初始化警告", "未找到可用相机", QMessageBox::Ok);
        this->ui->pushButton_switchCam->setEnabled(false);
    }
    for(int i=0; i<stDeviceList.nDeviceNum; i++) {
        this->ui->comboBox_selectCam->addItem(QString::number(i));
    }

    U32 devNum;
    Acm_GetAvailableDevs(this->devList, 20, &devNum);

    if(devNum == 0) {
        QMessageBox::warning(this, "设备初始化警告", "未找到可用运动控制卡", QMessageBox::Ok);
        this->ui->pushButton_switchCard->setEnabled(false);
    }
    for(int i=0; i<devNum; i++) {
        this->ui->comboBox_selectCard->addItem(devList[i].szDeviceName);
    }
}

ConstMicroDistanceSystem::~ConstMicroDistanceSystem()
{
    this->imageProcessor->stop();
    this->imageDetector->stop();
    this->camRecorder->stop();
    this->cam->closeCam();

    delete ui;
    delete this->cam;
}

void ConstMicroDistanceSystem::cbk(cv::Mat const& image) {
    cv::Mat imgShow; 

    cv::cvtColor(image, imgShow, cv::COLOR_BGR2RGB);
    QImage qimg((uchar*)imgShow.data, imgShow.cols, imgShow.rows, imgShow.step, QImage::Format_RGB888);

    ui->label->setPixmap(QPixmap::fromImage(qimg).scaled(ui->label->size(), Qt::KeepAspectRatio));
}

void ConstMicroDistanceSystem::onSwitchCamClicked() {
    if(this->cam == nullptr) {
        int index = this->ui->comboBox_selectCam->currentIndex();
        CameraParam params;

        this->cam = new Camera(index);
        this->cam->setParams(params);

        this->cam->registerImageCallback(CallbackFunctionType(std::bind(&ConstMicroDistanceSystem::cbk, this, std::placeholders::_1)));

        this->imageProcessor = new ImageProcessor(this->cam);
        this->imageProcessor->start();
        this->imageDetector = new ImageDetector(this->imageProcessor);
        this->imageDetector->start();

        this->ui->pushButton_switchCam->setText(QString("关闭相机"));
    } else {
        this->imageDetector->stop();
        this->imageProcessor->stop();
        this->cam->closeCam();
        this->cam = nullptr;
        this->ui->pushButton_switchCam->setText(QString("打开相机"));
    }
}

void ConstMicroDistanceSystem::onSwitchCardClicked() {
    if(this->advMotionDevHand == 0) {
        int index = this->ui->comboBox_selectCard->currentIndex();
        DWORD dwDevNum = this->devList[index].dwDeviceNum;
        Acm_DevOpen(dwDevNum, &this->advMotionDevHand);
        this->ui->pushButton_switchCard->setText(QString("关闭板卡"));
    } else {
        Acm_DevClose(&this->advMotionDevHand);
        this->ui->pushButton_switchCard->setText(QString("打开板卡"));
    }
}

void ConstMicroDistanceSystem::onSwitchCamGrabClicked()
{
    if(this->cam == nullptr) return;

    if(this->cam->getGrabStatus()) {
        this->cam->stopGrab();
        this->ui->pushButton_switchGrab->setText(QString("开始采集"));
    } else {
        this->cam->startGrab();
        this->ui->pushButton_switchGrab->setText(QString("停止采集"));
    }
}

void ConstMicroDistanceSystem::onTakePhotoClicked() {
    if(this->cam == nullptr) return;

    QString path = this->photoSavePath + "/" + QString::number(this->photoNum++) + ".jpg";

    cv::Mat image;
    if(!this->cam->getOneImageOrFail(image)){
        QMessageBox::critical(this, "错误", "缓冲区内无图像，请先开始采集。", QMessageBox::Ok);
        return;
    }

    cv::imwrite(path.toStdString(), this->cam->getOneImageWait());
}

void ConstMicroDistanceSystem::onSwitchRecordClicked() {
    if(this->cam == nullptr) return;

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

void ConstMicroDistanceSystem::onTestClicked() {
    Axis axis2 = Axis(devHand, 2);

    Axis axis4 = Axis(devHand, 4);
    axis4.setPulseOutMode(1);

    MotionController motioncontroller = MotionController(axis4, axis2);

    motioncontroller.spinRel(1000);
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