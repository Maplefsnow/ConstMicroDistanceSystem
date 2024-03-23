#pragma once
#include "ui_ConstMicroDistanceSystem.h"
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QString>
#include "module/Axis.h"
#include "module/Camera.h"
#include "tasks/CamRecorder.h"
#include "tasks/ImageProcessor.h"
#include "tasks/ImageDetector.h"

using CallbackFunctionType = std::function<void(cv::Mat const&)>;

class ConstMicroDistanceSystem : public QMainWindow {
    Q_OBJECT
    
public:
    ConstMicroDistanceSystem(QWidget* parent = nullptr);
    ~ConstMicroDistanceSystem();

private:
    Ui_ConstMicroDistanceSystem* ui;
    Camera* cam = nullptr;

    CamRecorder* camRecorder;
    ImageProcessor* imageProcessor;
    ImageDetector* imageDetector;

    void cbk(cv::Mat const& image);

    QString photoSavePath = QDir::currentPath() + "/Saved/photo";
    QString videoSavePath = QDir::currentPath() + "/Saved/video";
    int photoNum = 1;
    int videoNum = 1;
    bool is_recording = false;

public slots:
    void onSwitchCamClicked();
    void onSwitchCamGrabClicked();
    void onPhotoLocationTriggered();
    void onVideoLocationTriggered();
    void onTakePhotoClicked();
    void onSwitchRecordClicked();
};