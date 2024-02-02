#include "MVSCamera/MvCameraControl.h"
#include <opencv2/opencv.hpp>

using CallbackFunctionType = std::function<void(cv::Mat const&)>;

struct CameraParam {
    int width = 800;
    int height = 600;
    float exposureTime = 30000.0;
    char* exposureAuto = "Once";
    int triggerMode = 0;
    char* pixelFormat = "Mono8";
};


class Camera {
public:
    Camera();
    Camera(int index);
    ~Camera();

    void startGrab();
    void registerImageCallback(void(*cbk)(unsigned char* pData, MV_FRAME_OUT_INFO_EX *pstFrameInfo, void* pUser));
    void registerImageCallback(const CallbackFunctionType &cbk);
    void setParams(CameraParam params);

    void* getHandle() { return this->handle; };
    unsigned int getPayloadSize() { return this->payloadSize; };
    CallbackFunctionType cbk;

private:
    void* handle;
    unsigned int payloadSize;
};