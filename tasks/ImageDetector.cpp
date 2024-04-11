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

void getWireEdges(vector<Point> wireContour, vector<Point> tubeContour, Vec3f& upEdge, Vec3f& downEdge) {
    vector<Vec3f> wireEdges;  // up and down edges of the wire (no order)
    for(int i=0; i<4; i++) {
        Point2f p1 = wireContour[i]; Point2f p2 = wireContour[(i+1)%4];
        float dis = sqrt(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2));
        if(dis > 200.0){  // hack
            wireEdges.push_back(getLineByPoint(p1, p2));
        }
    }

    if(getDisByLinePoint(wireEdges[0], tubeContour[0]) > getDisByLinePoint(wireEdges[1], tubeContour[0])) {
        upEdge = wireEdges[0];
        downEdge = wireEdges[1];
    } else {
        upEdge = wireEdges[1];
        downEdge = wireEdges[0];
    }
}


RotatedRect getTubeFitEllipse(vector<Point> tubeContour, int imageWidth, int imageHeight) {
    vector<Point> tubeConvex;
    convexHull(tubeContour, tubeConvex);
    for(vector<Point>::iterator it = tubeConvex.begin(); it != tubeConvex.end();) {
        if((*it).x < 10 || (*it).y < 10 || imageWidth-(*it).x < 10 || imageHeight-(*it).y < 10) {
            it = tubeConvex.erase(it);
        } else {
            ++it;
        }
    }

    return fitEllipseAMS(tubeConvex);
}

/// @brief 使用最小二乘法 + 梯度下降法拟合圆
/// @param tubeContour 
/// @param imageWidth 
/// @param imageHeight 
/// @return vec3f(x, y, r)
Vec3f getTubeFitCircle(vector<Point> tubeContour, int imageWidth, int imageHeight) {
    vector<Point> tubeConvex;
    convexHull(tubeContour, tubeConvex);
    for(vector<Point>::iterator it = tubeConvex.begin(); it != tubeConvex.end();) {
        if((*it).x < 10 || (*it).y < 10 || imageWidth-(*it).x < 10 || imageHeight-(*it).y < 10) {
            it = tubeConvex.erase(it);
        } else {
            ++it;
        }
    }

    ////////////////////////////////////////////////////////////////////

    const int pt_len = tubeConvex.size();

    Mat A(pt_len, 3, CV_32FC1);
    Mat b(pt_len, 1, CV_32FC1);

    // 下面的两个 for 循环初始化 A 和 b
    for (int i = 0; i < pt_len; i++)
    {
        float *pData = A.ptr<float>(i);

        pData[0] = tubeConvex[i].x * 2.0f;
        pData[1] = tubeConvex[i].y * 2.0f;

        pData[2] = 1.0f;
    }

    float *pb = (float *)b.data;

    for (int i = 0; i < pt_len; i++)
    {
        pb[i] = tubeConvex[i].x * tubeConvex[i].x + tubeConvex[i].y * tubeConvex[i].y;
    }

    // 下面的几行代码就是解超定方程的最小二乘解
    Mat A_Trans;
    transpose(A, A_Trans);

    Mat Inv_A;
    invert(A_Trans * A, Inv_A);

    Mat res = Inv_A * A_Trans * b;

    // 取出圆心和半径
    float x = res.at<float>(0, 0);
    float y = res.at<float>(1, 0);
    float r = (float)sqrt(x * x + y * y + res.at<float>(2, 0));

    ///////////////////////////////////////////////////////////////////////

    const int lr = 1;           // learning rate
    const int iters = pt_len;   // iteration times

    vector<float> losses(pt_len);	// 每次迭代后的 loss 值
    vector<float> min_loss(pt_len);	// 每次迭代后的最小 loss
    vector<float> root_val(pt_len);	// 每次迭代中的开平方值, 方便以后使用

    for (int i = 0; i < iters; i++) {
        float loop_loss = 0;

        for (int j = 0; j < pt_len; j++) {
            // 这里第一次迭代的 x, y, r 是最小二乘的结果, 第二次迭代开始就是修正后的结果
            root_val[j] = sqrt((tubeConvex[j].x - x) * (tubeConvex[j].x - x) + (tubeConvex[j].y - y) * (tubeConvex[j].y - y));

            const float loss = root_val[j] - r;

            losses[j] = loss;
            loop_loss += fabs(loss);
        }

        min_loss[i] = loop_loss;

        // 如果 loss 值不再减小, 就提前结束
        if (i > 0 && min_loss[i] > min_loss[i - 1]) break;

        // 下面三个是梯度值
        float gx = 0;
        float gy = 0;
        float gr = 0;
        
        for (int j = 0; j < pt_len; j++) {
            // 在计算梯度时要先计算偏导数, 再将 x 代数公式得到
            float gxi = (x - tubeConvex[j].x) / root_val[j];
            if (losses[j] < 0) gxi *= (-1);

            float gyi = (y - tubeConvex[j].y) / root_val[j];
            if (losses[j] < 0) gyi *= (-1);

            float gri = -1;
            if (losses[j] < 0) gri = 1;

            gx += gxi;
            gy += gyi;
            gr += gri;
        }

        gx /= pt_len;
        gy /= pt_len;
        gr /= pt_len;

        x -= (lr * gx);
        y -= (lr * gy);
        r -= (lr * gr);
    }

    return Vec3f(x, y, r);
}

void detect(ImageProcessor* processor, void* pUser, double realWireDia, Ui_ConstMicroDistanceSystem* ui) {
    ImageDetector* detector = (ImageDetector*) pUser;

    Mat src;
    stDetectResult result;

    while(detector->status()) {
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

        getWireEdges(wireContour, tubeContour, result.wireUpEdge, result.wireDownEdge);

        vector<Point> tubeConvex;  // convex of the tube contour, ignore points on image edges
        convexHull(tubeContour, tubeConvex);
        for(vector<Point>::iterator it = tubeConvex.begin(); it != tubeConvex.end();) {
            if((*it).x < 10 || (*it).y < 10 || imageWidth-(*it).x < 10 || imageHeight-(*it).y < 10) {
                it = tubeConvex.erase(it);
            } else {
                ++it;
            }
        }

        Vec3f tubeCircle = getTubeFitCircle(tubeContour, imageWidth, imageHeight);

        result.tubeCenter = Point2f(tubeCircle[0], tubeCircle[1]);
        result.tubeRadius = tubeCircle[2];  // px

        result.dis_TubeCenterWire = getDisByLinePoint(result.wireDownEdge, result.tubeCenter);  // px
        result.dis_TubeWire = result.dis_TubeCenterWire - result.tubeRadius;  // px

        double pxToUm = realWireDia/getDisByLineLine(result.wireUpEdge, result.wireDownEdge);
        detector->setPxToUm(pxToUm);

        result.tubeRadius *= pxToUm;  // um
        result.dis_TubeCenterWire *= pxToUm;  // um
        result.dis_TubeWire *= pxToUm;  // um
    
        detector->pushDetectResBuffer(result);

#ifdef OPENCV_SHOW_IMAGES
        Mat canvas = Mat::zeros(Size(1000, 1000), CV_8UC3);
        cvtColor(src, src, COLOR_GRAY2BGR);

        for(Point p : tubeConvex) {
            circle(canvas, p, 3, Scalar(0, 0, 255), 3);
        }
        circle(canvas, Point(tubeCircle[0], tubeCircle[1]), tubeCircle[2], Scalar(255, 0, 0), 2);
        circle(canvas, Point(tubeCircle[0], tubeCircle[1]), 2, Scalar(0, 255, 0), 2);

        imshow("detect", canvas);
        waitKey(5);
#endif

        ui->label_distance->setText(QString::number(result.dis_TubeWire) + " um");
    }

    // destroyWindow("detect");
}


ImageDetector::~ImageDetector() {
    this->is_running = false;
}

ImageDetector::ImageDetector(ImageProcessor* processor, double realWireDia, Ui_ConstMicroDistanceSystem* ui) 
    : processor(processor), ui(ui), realWireDia(realWireDia) {}

stDetectResult ImageDetector::getDetectRes() {
    stDetectResult res;
    this->detectRes.wait_and_pop(res);
    return res;
}

void ImageDetector::run() {
    std::thread detectTrd(detect, this->processor, this, this->realWireDia, this->ui);
    detectTrd.detach();
}
