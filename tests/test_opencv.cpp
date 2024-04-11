#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <queue>
#include <limits>
#include <cmath>

#ifndef IMAGE_PATH
#define IMAGE_PATH "C:\\Users\\13297\\Pictures\\nachoneko-wallpaper.jpg"
#endif

using namespace std;
using namespace cv;

struct Circle {
    float x;
    float y;
    float r;

    friend bool operator<(const Circle a, const Circle b) {
        return a.r < b.r;
    }
};


struct Line {
    Point2i a;
    Point2i b;
};


Mat getFinalEdge(Mat src, Mat &erode_edge) {
    cvtColor(src, src, COLOR_BGR2GRAY);

    Mat img_erode;
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(20, 20));
    erode(src, img_erode, kernel);

    Mat img_dilate;
    dilate(img_erode, img_dilate, kernel);

    Mat img_blur;
    GaussianBlur(img_dilate, img_blur, Size(7, 7), 0);

    Mat edge;
    Canny(img_blur, edge, 10, 40);

    // Mat erode_edge;
    Mat kernel_edge = getStructuringElement(MORPH_ELLIPSE, Size(40, 40));

    dilate(edge, erode_edge, kernel_edge);
    erode(erode_edge, erode_edge, kernel_edge);

    Mat final_edge_blur;
    GaussianBlur(erode_edge, final_edge_blur, Size(15, 15), 0);

    Mat final_edge;
    Canny(erode_edge, final_edge, 0, 20);

    return final_edge;
}


priority_queue<Circle> getCircles(Mat src) {
    priority_queue<Circle> circles_queue;
    vector<Vec3f> circles;

    HoughCircles(src, circles, HOUGH_GRADIENT,  1, 55,
                                                100, 40,
                                                300, 400);

    if(circles.empty()) std::cout << "QAQ!" << std::endl;

    for(auto it : circles) {
        circles_queue.push(Circle{it[0], it[1], it[2]});
    }

    return circles_queue;
}


vector<Line> getLines(Mat src) {
    vector<Line> lines;

    return lines;
}


int main() {
    VideoCapture cap;

    // cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M','J','P','G')); 
    // cap.set(cv::CAP_PROP_FRAME_WIDTH, 640); 
    // cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480); 

    // cap.open("E:\\Maplef_Projects\\CppProjects\\OpenCVTest\\sources\\Video1.avi");
    cap.open(1);
    cap.set(CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(CAP_PROP_FRAME_HEIGHT, 1024);
    cap.set(CAP_PROP_FPS , 15);
    cap.set(CAP_PROP_EXPOSURE, -4);

    Size sizeReturn = Size(cap.get(cv::CAP_PROP_FRAME_WIDTH), cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    VideoWriter writer("E:\\CppProjects\\OpenCVTest\\sources\\result.avi",
                        VideoWriter::fourcc('M', 'J', 'P', 'G'),
                        cap.get(cv::CAP_PROP_FPS),
                        sizeReturn,
                        true);

    if(!cap.isOpened()) {
        std::cout << "FAILED OPEN!" << std::endl;
        return -1;
    }

    Mat frame;
    cap.read(frame);
    while(!frame.empty()) {
        Mat erode_edge;
        Mat edge = getFinalEdge(frame, erode_edge);
        
        priority_queue<Circle> circles = getCircles(edge);
        
        cvtColor(edge, edge, COLOR_GRAY2BGR);

        while(!circles.empty()) {
            Circle cir = circles.top(); circles.pop();
            std::cout << "r: " << cir.r << std::endl;
            circle(frame, Point(cir.x, cir.y), cir.r, Scalar(0, 0, 255), 2);
            circle(frame, Point(cir.x, cir.y), 2, Scalar(0, 0, 255), 2);
        }

        writer.write(frame);

        imshow("edge", frame);
        waitKey(1);
        cap.read(frame);
    }

    

    writer.release();
    cap.release();
    destroyAllWindows();

    return 0;
}