#pragma once

#include "ui_ConstMicroDistanceSystem.h"
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QString>
#include "module/Axis.h"
#include "module/Camera.h"
#include "module/MotionController.h"
#include "tasks/CamRecorder.h"
#include "tasks/ImageProcessor.h"
#include "tasks/ImageDetector.h"
#include "tasks/ParamsFitter.h"
#include "tasks/FeedExecutor.h"

using CallbackFunctionType = std::function<void(cv::Mat const&)>;

class ConstMicroDistanceSystem : public QMainWindow {
    Q_OBJECT
    
public:
    ConstMicroDistanceSystem(QWidget* parent = nullptr);
    ~ConstMicroDistanceSystem();

private:
    Ui_ConstMicroDistanceSystem* ui;
    Camera* cam = nullptr;
    DEVLIST devList[20];
    HAND advMotionDevHand = 0;

    MotionController* motionController = nullptr;

    CamRecorder* camRecorder = nullptr;
    ImageProcessor* imageProcessor = nullptr;
    ImageDetector* imageDetector = nullptr;
    ParamsFitter* paramsFitter = nullptr;
    FeedExecutor* feedExecutor = nullptr;

    void cbk(cv::Mat const& image);

    QString photoSavePath = QDir::currentPath() + "/Saved/photo";
    QString videoSavePath = QDir::currentPath() + "/Saved/video";
    int photoNum = 1;
    int videoNum = 1;
    bool is_recording = false;

public slots:
    void onSwitchCamClicked();
    void onSwitchCardClicked();
    void onSwitchCamGrabClicked();
    void onPhotoLocationTriggered();
    void onVideoLocationTriggered();
    void onTakePhotoClicked();
    void onSwitchRecordClicked();
    void onTestClicked();
    void onGetParamsClicked();
    void onDoFeedClicked();
};