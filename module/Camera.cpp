#include "Camera.h"
#include <string>
#include <stdio.h>

#define throwError(x, msg) \
	{\
		ret = x;\
		if (ret != MV_OK) {\
            char errorMsg[1000];\
            sprintf(errorMsg, "%s [nRet: 0x%x]", msg, ret); \
			ret = 0;\
			throw errorMsg;\
		}\
	}

int ret = 0;

Camera::Camera() {
}

Camera::Camera(int index) {
    int nRet = MV_OK;
    MV_CC_DEVICE_INFO_LIST stDeviceList;
    memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

    throwError(MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList), "Enum camera devices fail!");

    if(stDeviceList.nDeviceNum <= 0) throw "Find no devices!";
    if(index > stDeviceList.nDeviceNum-1) throw "No such device! Total num: " + stDeviceList.nDeviceNum;

    throwError(MV_CC_CreateHandle(&this->handle, stDeviceList.pDeviceInfo[index]), "Create device handle fail!");
}

Camera::~Camera() {

}

void Camera::open() {
    throwError(MV_CC_OpenDevice(this->handle), "Open device fail!");
}

void Camera::setParams(int triggerMode, char* pixelFormat) {
    throwError(MV_CC_SetEnumValue(this->handle, "TriggerMode", triggerMode), "Set TriggerMode fail!");
    throwError(MV_CC_SetEnumValueByString(this->handle, "PixelFormat", pixelFormat), "Set PixelFormat fail!");
}
