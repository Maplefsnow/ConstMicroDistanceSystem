#include "ImageDetector.h"
#include "utils/GeometricCalc.cpp"

using namespace std;
using namespace cv;
using namespace GC;

void splitContours(vector<vector<Point>> contours, vector<Point>& wireContour, vector<Point>& tubeContour) {
    for(int i=0; i<contours.size(); i++) {
        vector<Point> approxContour;
        approxPolyDP(contours[i], approxContour, 15, true);
        if(approxContour.size() == 4) {
            wireContour = approxContour;
        } else if (approxContour.size() > 4) {
            tubeContour = contours[i];
        }
    }
}

bool getWireEdges(vector<Point> wireContour, vector<Point> tubeContour, Vec3f& upEdge, Vec3f& downEdge) {
    vector<Vec3f> wireEdges;  // up and down edges of the wire (no order)
    for(int i=0; i<4; i++) {
        Point2f p1 = wireContour[i]; Point2f p2 = wireContour[(i+1)%4];
        float dis = sqrt(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2));
        if(dis > 200.0){  // hack
            wireEdges.push_back(getLineByPoint(p1, p2));
        }
    }

    if(wireEdges.size() == 0) return false;

    if(getDisByLinePoint(wireEdges[0], tubeContour[0]) > getDisByLinePoint(wireEdges[1], tubeContour[0])) {
        upEdge = wireEdges[0];
        downEdge = wireEdges[1];
    } else {
        upEdge = wireEdges[1];
        downEdge = wireEdges[0];
    }

    return true;
}

void ImageDetector::detect() {
    Mat src;
    stDetectResult result;

    while(this->is_running && this->processor->status()) {
        src = processor->getOneImageWait();

        int imageWidth = src.cols, imageHeight = src.rows;

        vector<vector<Point>> contours;
        findContours(src, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        if(contours.size() < 2 || contours.size() > 5) continue;  // 滤掉坏帧

        vector<Point> wireContour;  // the wire contour, approx, rectangle shape
        vector<Point> tubeContour;  // the tube contour, precise

        splitContours(contours, wireContour, tubeContour);

        if(wireContour.empty() || tubeContour.empty()) {
            continue;
        }

        if(!getWireEdges(wireContour, tubeContour, result.wireUpEdge, result.wireDownEdge)) continue;

        vector<Point> tubeConvex;  // convex of the tube contour, ignore points on image edges
        convexHull(tubeContour, tubeConvex);
        for(vector<Point>::iterator it = tubeConvex.begin(); it != tubeConvex.end();) {
            if((*it).x < 10 || (*it).y < 10 || imageWidth-(*it).x < 10 || imageHeight-(*it).y < 10) {
                it = tubeConvex.erase(it);
            } else {
                ++it;
            }
        }

        vector<Point2f> tubeConvex2f;
        tubeConvex2f.resize(tubeConvex.size());
        std::copy(tubeConvex.begin(), tubeConvex.end(), tubeConvex2f.begin());

        Vec3f tubeCircle = Fitter::fitCircleByLeastSquareGradDescent(tubeConvex2f);

        result.tubeCenter = Point2f(tubeCircle[0], tubeCircle[1]);
        result.tubeRadius = tubeCircle[2];  // px

        result.dis_TubeCenterWire = getDisByLinePoint(result.wireDownEdge, result.tubeCenter);  // px
        result.dis_TubeWire = result.dis_TubeCenterWire - result.tubeRadius;  // px

        double pxToUm = realWireDia/getDisByLineLine(result.wireUpEdge, result.wireDownEdge);
        this->setPxToUm(pxToUm);

        result.tubeRadius *= pxToUm;  // um
        result.dis_TubeCenterWire *= pxToUm;  // um
        result.dis_TubeWire *= pxToUm;  // um
    
        this->pushDetectResBuffer(result);

#ifdef OPENCV_SHOW_IMAGES
        Mat canvas = Mat::zeros(Size(1000, 1000), CV_8UC3);
        cvtColor(src, src, COLOR_GRAY2BGR);

        vector<vector<Point>> tmp_contours;
        tmp_contours.push_back(wireContour);
        drawContours(canvas, tmp_contours, 0, Scalar(255, 0, 0), 2);

        for(Point p : tubeConvex) {
            circle(canvas, p, 3, Scalar(0, 0, 255), 3);
        }
        circle(canvas, Point(tubeCircle[0], tubeCircle[1]), tubeCircle[2], Scalar(255, 0, 0), 2);
        circle(canvas, Point(tubeCircle[0], tubeCircle[1]), 2, Scalar(0, 255, 0), 2);

        imshow("detect", canvas);
        waitKey(10);
#endif

        ui->label_distance->setText(QString::number(result.dis_TubeWire) + " um");
    }

#ifdef OPENCV_SHOW_IMAGES
    destroyWindow("detect");
#endif
}


ImageDetector::~ImageDetector() {
    this->is_running = false;
    this->detectTrd->join();
}

ImageDetector::ImageDetector(ImageProcessor* processor, double realWireDia, Ui_ConstMicroDistanceSystem* ui) 
    : processor(processor), ui(ui), realWireDia(realWireDia) {}

stDetectResult ImageDetector::getDetectRes() {
    stDetectResult res;
    this->detectRes.wait_and_pop(res);
    return res;
}

void ImageDetector::run() {
    this->is_running = true;
    this->detectTrd = new std::thread(detect, this);
}
