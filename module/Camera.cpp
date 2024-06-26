#include "Camera.h"
#include <string.h>
#include <stdio.h>

#define throwError(x, msg) \
	{\
		nRet = x;\
		if (nRet != MV_OK) {\
            char errorMsg[1000];\
            sprintf(errorMsg, "%s [nRet: 0x%x]", msg, nRet); \
			nRet = 0;\
			throw errorMsg;\
		}\
	}

int nRet = 0;


void cameraCallback(unsigned char* pData, MV_FRAME_OUT_INFO_EX *pstFrameInfo, void* pUser) {
    Camera* camera = (Camera*) pUser;

    cv::Mat image = cv::Mat(pstFrameInfo->nHeight, pstFrameInfo->nWidth, CV_8UC1, pData);

    camera->cbk(image);
}

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

    MVCC_INTVALUE stParam;
    memset(&stParam, 0, sizeof(MVCC_INTVALUE));
    throwError(MV_CC_GetIntValue(handle, "PayloadSize", &stParam), "Get PayloadSize fail!");
    this->payloadSize = stParam.nCurValue;

    throwError(MV_CC_OpenDevice(this->handle), "Open device fail!");
}

Camera::~Camera() {
    MV_CC_CloseDevice(this->handle);
    MV_CC_DestroyHandle(this->handle);
}

void Camera::registerImageCallback(void(*cbk)(unsigned char* pData, MV_FRAME_OUT_INFO_EX *pstFrameInfo, void* pUser)) {
    throwError(MV_CC_RegisterImageCallBackEx(this->handle, cbk, nullptr), "Register callback function fail!");
}

void Camera::registerImageCallback(const CallbackFunctionType &cbk) {
    this->cbk = cbk;
    MV_CC_RegisterImageCallBackEx(this->handle, cameraCallback, this);
}

void Camera::startGrab() {
    throwError(MV_CC_StartGrabbing(this->handle), "Start grabbing fail!");
}

void Camera::setParams(CameraParam params) {
    throwError(MV_CC_SetWidth(this->handle, params.width), "Set Width fail!");
    throwError(MV_CC_SetHeight(this->handle, params.height), "Set Height fail!");
    throwError(MV_CC_SetExposureTime(this->handle, params.exposureTime), "Set ExposureTime fail!");
    throwError(MV_CC_SetEnumValueByString(this->handle, "ExposureAuto", params.exposureAuto), "Set ExposureAuto fail!");
    throwError(MV_CC_SetTriggerMode(this->handle, params.triggerMode), "Set TriggerMode fail!");
    throwError(MV_CC_SetEnumValueByString(this->handle, "PixelFormat", params.pixelFormat), "Set PixelFormat fail!");
}
