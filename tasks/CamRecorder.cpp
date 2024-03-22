#include "CamRecorder.h"

void record(Camera* cam, QString path, int fps, void* pUser) {
    CamRecorder* camRecorder = (CamRecorder*) pUser;

    cv::Size size(cam->getImageWidth(), cam->getImageHeight());

    cv::VideoWriter writer(path.toStdString(), cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, size);

    while(camRecorder->status()) {
        writer.write(cam->getOneImageWait());
        cv::waitKey(30);
    }

    writer.release();
}

CamRecorder::CamRecorder() {}

CamRecorder::CamRecorder(Camera* cam, QString path, int fps = 20) : cam(cam), path(path), fps(fps) {}

CamRecorder::~CamRecorder() {
    this->is_running = false;
}

void CamRecorder::run() {
    std::thread recordTrd(record, this->cam, this->path, this->fps, this);
    recordTrd.detach();
}

