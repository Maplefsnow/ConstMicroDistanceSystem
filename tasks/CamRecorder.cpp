#include "CamRecorder.h"

void record(Camera* cam, QString path, void* pUser) {
    CamRecorder* camRecorder = (CamRecorder*) pUser;

    cv::Size size(cam->getImageWidth(), cam->getImageHeight());

    cv::VideoWriter writer(path.toStdString(), cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 20, size, false);

    while(camRecorder->status()) {
        std::cout << "Recorder get image!" << std::endl;
        writer.write(cam->getOneImageWait());
        cv::waitKey(30);
    }

    writer.release();
}

CamRecorder::CamRecorder() {}

CamRecorder::CamRecorder(Camera* cam, QString path) {
    this->cam = cam;
    this->path = path;
}

CamRecorder::~CamRecorder() {
    delete this->cam;
}

void CamRecorder::run() {
    std::cout << "Recorder run!" << std::endl;

    std::thread recordTrd(record, this->cam, this->path, this);
    recordTrd.detach();
}

