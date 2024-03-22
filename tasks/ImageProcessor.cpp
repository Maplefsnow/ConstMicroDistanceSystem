#include "ImageProcessor.h"
#include <thread>
#include <vector>

using namespace cv;
using namespace std;


Mat getBinary(const Mat src) {
    Mat img_blur;
    GaussianBlur(src, img_blur, Size(7, 7), 0);

    Mat binary;
    adaptiveThreshold(img_blur, binary, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 11, 4);

    Mat filter;
    bilateralFilter(binary, filter, 21, 9, 9);

    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(20, 20));

    Mat img_erode;
    erode(filter, img_erode, kernel);

    Mat img_dilate;
    dilate(img_erode, img_dilate, kernel);

    bitwise_not(img_dilate, img_dilate);

    return img_dilate;
}

void process(Camera* cam, void* pUser) {
    ImageProcessor* processor = (ImageProcessor*) pUser;

    Mat src;

    while(processor->status()) {
        src = cam->getOneImageWait();

        cvtColor(src, src, COLOR_BGR2GRAY);
        resize(src, src, Size(), 0.5, 0.5);

        Mat binary = getBinary(src);

        imshow("processor", binary);
        waitKey(1);

        processor->pushImageBuffer(binary);
    }
}

ImageProcessor::ImageProcessor() {}

ImageProcessor::ImageProcessor(Camera* cam) : cam(cam) {} 

ImageProcessor::~ImageProcessor() {
    this->is_running = false;
}

void ImageProcessor::run() {
    std::thread processTrd(process, this->cam, this);
    processTrd.detach();
}

Mat ImageProcessor::getOneImageWait() {
    Mat image;
    this->processedImage.wait_and_pop(image);
    return image;
}