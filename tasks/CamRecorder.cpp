#include "CamRecorder.h"

void CamRecorder::record() {
    cv::Size size(this->cam->getImageWidth(), this->cam->getImageHeight());

    cv::VideoWriter writer(this->path.toStdString(), cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), this->fps, size);

    while(this->is_running) {
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
    this->recordTrd->join();
    this->recordTrd = nullptr;
}

void CamRecorder::run() {
    this->recordTrd = new std::thread(CamRecorder::record, this);}

