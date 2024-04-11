#include "CamRecorder.h"

void record(Camera* cam, QString path, int fps, void* pUser, Ui_ConstMicroDistanceSystem* ui) {
    CamRecorder* camRecorder = (CamRecorder*) pUser;

    cv::Size size(cam->getImageWidth(), cam->getImageHeight());

    cv::VideoWriter writer(path.toStdString(), cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, size);

    while(camRecorder->status()) {
        cv::Mat image;
        cam->getOneImageWait(image, 0);
        writer.write(image);
        cv::waitKey(30);
    }

    writer.release();
}

CamRecorder::CamRecorder() {}

CamRecorder::CamRecorder(Camera* cam, QString path, Ui_ConstMicroDistanceSystem* ui, int fps) : cam(cam), path(path), fps(fps), ui(ui) {}

CamRecorder::~CamRecorder() {
    this->is_running = false;
}

void CamRecorder::run() {
    std::thread recordTrd(record, this->cam, this->path, this->fps, this, this->ui);
    recordTrd.detach();
}

