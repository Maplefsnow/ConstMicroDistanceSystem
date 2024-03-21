#include "ImageProcessor.h"
#include <thread>
#include <vector>

using namespace cv;
using namespace std;


Vec3f getLineByPoint(Point p1, Point p2) {
    float k = (p2.y - p1.y) / (p2.x - p1.x);
    float b = p1.y - k*(p1.x);
    return Vec3f(k, -1.0, b);
}

float getDisByLinePoint(Vec3f line, Point p) {
    float A = line[0], B = line[1], C = line[2];
    float x = p.x, y = p.y;
    return (A*x + B*y + C) / (sqrt(A*A + B*B));
}


Mat getBinary(const Mat src) {
    Mat img_blur;
    GaussianBlur(src, img_blur, Size(7, 7), 0);

    Mat binary;
    adaptiveThreshold(img_blur, binary, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 11, 3);

    Mat filter;
    bilateralFilter(binary, filter, 21, 9, 9);

    Mat img_erode;
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(20, 20));
    erode(filter, img_erode, kernel);

    Mat img_dilate;
    dilate(img_erode, img_dilate, kernel);

    bitwise_not(img_dilate, img_dilate);

    return img_dilate;
}

void process(Camera* cam, void* pUser) {
    ImageProcessor* processor = (ImageProcessor*) pUser;

    Mat src;
    stDetectResult detectResult;

    while(processor->status()) {
        src = cam->getOneImageWait();

        cvtColor(src, src, COLOR_BGR2GRAY);
        resize(src, src, Size(), 0.5, 0.5);

        Mat binary = getBinary(src);

        imshow("binary", binary);

        vector<vector<Point>> contours;
        vector<Point> wireContour;  // the wire contour, approx, rectangle shape
        vector<Point> tubeContour;  // the tube contour, precise
        findContours(binary, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        for(int i=0; i<contours.size(); i++) {
            vector<Point> approxContour;
            approxPolyDP(contours[i], approxContour, 15, true);
            if(approxContour.size() == 4) {
                wireContour = approxContour;
            } else if (approxContour.size() > 4) {
                tubeContour = contours[i];
            }
        }

        vector<Vec3f> lineEdges;  // up and down edges of the wire (no order)
        for(int i=0, j=0; i<4; i++) {
            Point p1 = wireContour[i]; Point p2 = wireContour[(i+1)%4];
            float dis = sqrt(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2));
            if(dis > 300.0){
                lineEdges.push_back(getLineByPoint(p1, p2));
            }
        }

        if(getDisByLinePoint(lineEdges[0], tubeContour[0]) > getDisByLinePoint(lineEdges[1], tubeContour[0])) {
            detectResult.wireUpEdge = lineEdges[0];
            detectResult.wireDownEdge = lineEdges[1];
        } else {
            detectResult.wireUpEdge = lineEdges[1];
            detectResult.wireDownEdge = lineEdges[0];
        }

        cvtColor(src, src, COLOR_GRAY2BGR);
        // drawContours(src, approxContours, -1, Scalar(255, 0, 0), 2);

        imshow("process", src);
        waitKey(1);
    }
}

ImageProcessor::ImageProcessor() {}

ImageProcessor::ImageProcessor(Camera* cam) : cam(cam) {} 

ImageProcessor::~ImageProcessor() {
    delete this->cam;
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