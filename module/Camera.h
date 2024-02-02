#include "MvCameraControl.h"


class Camera {
public:
    Camera();
    Camera(int index);
    ~Camera();

    void open();
    void setParams(int triggerMode, char* pixelFormat);

private:
    void* handle;
};