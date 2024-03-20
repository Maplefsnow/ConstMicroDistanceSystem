#pragma once

#include "MVSCamera/MvCameraControl.h"
#include <opencv2/opencv.hpp>
#include <queue>
#include <mutex>
#include <condition_variable>

using CallbackFunctionType = std::function<void(cv::Mat const&)>;

struct CameraParam {
    int width = 1536;
    int height = 1024;
    int h_binning = 2;
    int v_binning = 2;
    float exposureTime = 30000.0;
    int triggerMode = 0;    
    char* exposureAuto = "Once";
    char* pixelFormat = "Mono8";
};


class Camera {
public:
    Camera();
    Camera(int index);
    ~Camera();

    void startGrab();
    void stopGrab();
    void registerImageCallback(void(*cbk)(unsigned char* pData, MV_FRAME_OUT_INFO_EX *pstFrameInfo, void* pUser));
    void registerImageCallback(const CallbackFunctionType &cbk);
    void setParams(CameraParam params);

    void pushBuffer(cv::Mat image);
    cv::Mat getOneImageWait();
    bool getOneImageOrFail(cv::Mat &image);

    void* getHandle() { return this->handle; };
    int getImageWidth();
    int getImageHeight();
    unsigned int getPayloadSize() { return this->payloadSize; };
    bool getGrabStatus() { return this->is_grabbing; };
    CallbackFunctionType cbk;

private:
    void* handle;
    unsigned int payloadSize;
    bool is_grabbing = false;
    std::queue<cv::Mat> imageBuffer;
    std::mutex buffer_mutex;
    std::condition_variable cond;
};