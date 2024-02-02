#pragma once
#include "ui_ConstMicroDistanceSystem.h"
#include <QMainWindow>

class ConstMicroDistanceSystem : public QMainWindow {
    Q_OBJECT
    
public:
    ConstMicroDistanceSystem(QWidget* parent = nullptr);
    ~ConstMicroDistanceSystem();

private:
    Ui_ConstMicroDistanceSystem* ui;
};