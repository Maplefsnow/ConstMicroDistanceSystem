#pragma once
#include "ui_ConstMicroDistanceSystem.h"
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QString>
#include "module/Axis.h"
#include "module/Camera.h"

using CallbackFunctionType = std::function<void(cv::Mat const&)>;

class ConstMicroDistanceSystem : public QMainWindow {
    Q_OBJECT
    
public:
    ConstMicroDistanceSystem(QWidget* parent = nullptr);
    ~ConstMicroDistanceSystem();

private:
    Ui_ConstMicroDistanceSystem* ui;
    Camera* cam;
    void cbk(cv::Mat const& image);
    QString photoSavePath = "./Saved/photo";
    QString videoSavePath = "./Saved/video";

public slots:
    void onSwitchCamGrabClicked();
    void onPhotoLocationTriggered();
    void onTakePhotoClicked();
};