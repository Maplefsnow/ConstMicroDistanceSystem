#pragma once

#include "opencv2/opencv.hpp"

using namespace cv;

namespace GC {

inline Vec3f getLineByPoint(Point2f p1, Point2f p2) {
    float k = (p2.y - p1.y) / (p2.x - p1.x);
    float b = p1.y - k*(p1.x);
    return Vec3f(k, -1.0, b);
}

inline float getDisByLinePoint(Vec3f line, Point2f p) {
    float A = line[0], B = line[1], C = line[2];
    float x = p.x, y = p.y;
    return abs(A*x + B*y + C) / (sqrt(A*A + B*B));
}

inline double getDisByLineLine(const cv::Vec3f line1, const cv::Vec3f line2) {
    return abs(line1[2] - line2[2])/sqrt(line1[0]*line1[0] + line1[1]*line1[1]);
}

}