#include "ConstMicroDistanceSystem.h"
#include "MVSCamera/MvCameraControl.h"
#include "Advmot/AdvMotApi.h"
#include "module/Axis.h"

ConstMicroDistanceSystem::ConstMicroDistanceSystem(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui_ConstMicroDistanceSystem)
{
    ui->setupUi(this);
}

ConstMicroDistanceSystem::~ConstMicroDistanceSystem()
{
    delete ui; 
}