#include "ConstMicroDistanceSystem.h"
#include "MVSCamera/MvCameraControl.h"
#include "Advmot/AdvMotApi.h"
#include <opencv2/opencv.hpp>
#include <QMessageBox>
#include <QSizePolicy>
#include <vector>

using namespace std;

ConstMicroDistanceSystem::ConstMicroDistanceSystem(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui_ConstMicroDistanceSystem) {
    ui->setupUi(this);

    MV_CC_DEVICE_INFO_LIST stDeviceList;
    memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

    MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
    if(stDeviceList.nDeviceNum == 0) {
        QMessageBox::warning(this, "设备初始化警告", "未找到可用相机", QMessageBox::Ok);
        this->ui->pushButton_switchCam->setEnabled(false);
        this->ui->comboBox_selectCam->addItem(QString("[no cam detected]"));
    }
    for(int i=0; i<stDeviceList.nDeviceNum; i++) {
        char* tmp = (char*) stDeviceList.pDeviceInfo[i]->SpecialInfo.stUsb3VInfo.chModelName;
        this->ui->comboBox_selectCam->addItem(QString::fromUtf8(tmp));
    }

    U32 devNum;
    Acm_GetAvailableDevs(this->devList, 20, &devNum);

    if(devNum == 0) {
        QMessageBox::warning(this, "设备初始化警告", "未找到可用运动控制卡", QMessageBox::Ok);
        this->ui->pushButton_switchCard->setEnabled(false);
        this->ui->comboBox_selectCard->addItem(QString("[no card detected]"));
    }
    for(int i=0; i<devNum; i++) {
        this->ui->comboBox_selectCard->addItem(devList[i].szDeviceName);
    }

    // ui->label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->label->setScaledContents(false);
    this->ui->pushButton_switchGrab->setEnabled(false);
}

ConstMicroDistanceSystem::~ConstMicroDistanceSystem() {
    if(this->imageProcessor != nullptr) this->imageProcessor->stop();
    if(this->imageDetector != nullptr) this->imageDetector->stop();
    if(this->camRecorder != nullptr) this->camRecorder->stop();
    if(this->cam != nullptr) this->cam->closeCam();

    delete ui;
    delete this->cam;
}

void ConstMicroDistanceSystem::cbk(cv::Mat const& image) {
    cv::Mat imgShow; 

    cv::cvtColor(image, imgShow, cv::COLOR_BGR2RGB);
    QImage qimg((uchar*)imgShow.data, imgShow.cols, imgShow.rows, imgShow.step, QImage::Format_RGB888);
    qimg = qimg.scaled(ui->label->size() - QSize(5, 5), Qt::KeepAspectRatio);

    ui->label->setPixmap(QPixmap::fromImage(qimg));
}

void ConstMicroDistanceSystem::onSwitchCamClicked() {
    if(this->cam == nullptr) {
        int index = this->ui->comboBox_selectCam->currentIndex();
        CameraParam params;

        this->cam = new Camera(index);
        this->cam->setParams(params);

        this->cam->registerImageCallback(CallbackFunctionType(std::bind(&ConstMicroDistanceSystem::cbk, this, std::placeholders::_1)));

        this->ui->pushButton_switchCam->setText(QString("关闭相机"));
        this->ui->pushButton_switchGrab->setEnabled(true);
    } else {
        if(this->imageDetector != nullptr) this->imageDetector->stop();
        if(this->imageProcessor != nullptr) this->imageProcessor->stop();
        this->cam->closeCam();
        this->cam = nullptr;
        this->ui->pushButton_switchCam->setText(QString("打开相机"));
        this->ui->pushButton_switchGrab->setText(QString("开始采集"));
        this->ui->pushButton_switchGrab->setEnabled(false);
    }
}

void ConstMicroDistanceSystem::onSwitchCardClicked() {
    if(this->advMotionDevHand == 0) {
        int index = this->ui->comboBox_selectCard->currentIndex();
        DWORD dwDevNum = this->devList[index].dwDeviceNum;
        Acm_DevOpen(dwDevNum, &this->advMotionDevHand);

        Axis spin = Axis(this->advMotionDevHand, 4);
        spin.setPulseOutMode(1);
        Axis vertical = Axis(this->advMotionDevHand, 2);
        Axis tmp = Axis(this->advMotionDevHand, 0);
        this->motionController = new MotionController(spin, vertical, tmp);

        this->ui->pushButton_switchCard->setText(QString("关闭板卡"));
    } else {
        Acm_DevClose(&this->advMotionDevHand);
        this->motionController = nullptr;
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

        this->imageProcessor = new ImageProcessor(this->cam);
        this->imageProcessor->start();
        this->imageDetector = new ImageDetector(this->imageProcessor, 100.0, this->ui);  // @todo: set variable of realWireDia or else
        this->imageDetector->start();

        this->ui->pushButton_switchGrab->setText(QString("停止采集"));
    }
}

void ConstMicroDistanceSystem::onTakePhotoClicked() {
    if(this->cam == nullptr) return;

    cv::Mat image;

    if(this->cam->getOneImageWait(image)){
        QString path = this->photoSavePath + "/" + QString::number(this->photoNum++) + ".jpg";
        cv::imwrite(path.toStdString(), image);
        this->ui->statusBar->showMessage("已拍摄照片", 500);
    } else {
        QMessageBox::critical(this, "错误", "缓冲区内无图像，请先开始采集。", QMessageBox::Ok);
    }
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
        this->camRecorder = new CamRecorder(this->cam, path, this->ui);
        this->camRecorder->start();
        this->ui->pushButton_switchRecord->setText(QString("停止录制"));
        this->is_recording = true;
    }
}

inline double getDisByLineLine(const cv::Vec3f line1, const cv::Vec3f line2) {
    return abs(line1[2] - line2[2])/sqrt(line1[0]*line1[0] + line1[1]*line1[1]);
}

void ConstMicroDistanceSystem::onTestClicked() {
    printf("Tested!\n");
}

void ConstMicroDistanceSystem::onGetParamsClicked() {
    this->paramsFitter = new ParamsFitter(this->imageDetector, this->motionController);
    this->paramsFitter->run();
    this->feedExecutor = new FeedExecutor(this->motionController, this->paramsFitter, this->imageDetector, 300.0, this->ui);  // @todo: set to ui
}

void ConstMicroDistanceSystem::onDoFeedClicked() {
    this->feedExecutor->doFeed();
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