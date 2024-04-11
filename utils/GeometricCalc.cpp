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

inline void GetCircle(const Point2f& p1, const Point2f& p2, const Point2f& p3, Point2f& center, double & radius2) {
    double r12 = p1.x * p1.x + p1.y * p1.y;
    double r22 = p2.x * p2.x + p2.y * p2.y;
    double r32 = p3.x * p3.x + p3.y * p3.y;
    double A = p1.x * (p2.y - p3.y) - p1.y * (p2.x - p3.x) + p2.x * p3.y - p3.x * p2.y;
    double B = r12 * (p2.y - p3.y) + r22 * (p3.y - p1.y) + r32 * (p1.y - p2.y);
    double C = r12 * (p2.y - p3.y) + r22 * (p3.y - p1.y) + r32 * (p1.y - p2.y);

    center.x = B / (2 * A);

    center.y = C / (2 * A);

    radius2 = (center.x - p1.x) * (center.x - p1.x) + (center.y - p1.y) * (center.y - p1.y);
}

inline void GetNRand(const int maxV, const int N, std::set<int>& idxs) {
    if (N > maxV) {
        return;
    }

    while(idxs.size() < N) {
        idxs.insert(rand() % maxV);
    }
}

class Fitter {
public:
    static double FitCircleByLeastSquare(const std::vector<Point2f>& pointArray, Point2f& center, double& radius) {
        int N = pointArray.size();
        if (N < 3) {
            return std::numeric_limits<double>::infinity();
        }

        double sumX = 0.0; 
        double sumY = 0.0;
        double sumX2 = 0.0;
        double sumY2 = 0.0;
        double sumX3 = 0.0;
        double sumY3 = 0.0;
        double sumXY = 0.0;
        double sumXY2 = 0.0;
        double sumX2Y = 0.0;

        for (int pId = 0; pId < N; ++pId) {
            sumX += pointArray[pId].x;
            sumY += pointArray[pId].y;

            double x2 = pointArray[pId].x * pointArray[pId].x;
            double y2 = pointArray[pId].y * pointArray[pId].y;
            sumX2 += x2;
            sumY2 += y2;

            sumX3 += x2 * pointArray[pId].x;
            sumY3 += y2 * pointArray[pId].y;
            sumXY += pointArray[pId].x * pointArray[pId].y;
            sumXY2 += pointArray[pId].x * y2;
            sumX2Y += x2 * pointArray[pId].y;
        }

        double C, D, E, G, H;
        double a, b, c;

        C = N * sumX2 - sumX * sumX;
        D = N * sumXY - sumX * sumY;
        E = N * sumX3 + N * sumXY2 - (sumX2 + sumY2) * sumX;
        G = N * sumY2 - sumY * sumY;
        H = N * sumX2Y + N * sumY3 - (sumX2 + sumY2) * sumY;

        a = (H * D - E * G) / (C * G - D * D);
        b = (H * C - E * D) / (D * D - G * C);
        c = -(a * sumX + b * sumY + sumX2 + sumY2) / N;

        center.x = -a / 2.0;
        center.y = -b / 2.0;
        radius = sqrt(a * a + b * b - 4 * c) / 2.0;

        double err = 0.0;
        double e;
        double r2 = radius * radius;
        for (int pId = 0; pId < N; ++pId){
            e = (pointArray[pId] - center).x*(pointArray[pId] - center).x + (pointArray[pId] - center).y*(pointArray[pId] - center).y - r2;
            if (e > err) {
                err = e;
            }
        }
        return err;
    }

    
    /// @brief get a circle by RANSAC
    /// @param pointArray input
    /// @param center output
    /// @param radius output
    /// @param iterNum 
    /// @param e error
    /// @param ratio minimum ratio = target_points_num / total_points_num, 0<ratio<1
    /// @return 
    static double FitCircleByRANSAC(const std::vector<Point2f>& pointArray, Point2f& center, double& radius, const int iterNum = 10, const double e = 4.0, const float ratio = 0.8) {
        const int N = pointArray.size();
		const int targetN = N * ratio;
		int iter = 0;
		std::vector<Point2f> bestInliers;
		while (iter < iterNum) {
			std::set<int> seedIds;
			GetNRand(N, 3, seedIds);  // circle need 3 point
			if (seedIds.size() < 3) {
				break;
			}
			std::vector<Point2f> seedPts;
			for (const int idx : seedIds) {
				seedPts.push_back(pointArray[idx]);
			}
			Point2f seedCenter;
			double seedR2 = 0.0;
			GetCircle(seedPts[0], seedPts[1], seedPts[2], seedCenter, seedR2);

			std::vector<Point2f> maybeInliers;
			for (const Point2f pt : pointArray) {
				if (std::abs((pt.x - seedCenter.x) * (pt.x - seedCenter.x) + (pt.y - seedCenter.y) * (pt.y - seedCenter.y) - seedR2) < e) {
					maybeInliers.push_back(pt);
				}
			}

			if (maybeInliers.size() > targetN) {
				// it show the inliers is enough
				return FitCircleByLeastSquare(maybeInliers, center, radius);
			}
			else {
				if (maybeInliers.size() > bestInliers.size()) {
					bestInliers.swap(maybeInliers);
					for (const Point2f pt : seedPts) {
						bestInliers.push_back(pt);
					}
				}
			}

			++iter;
		}
		return FitCircleByLeastSquare(bestInliers, center, radius);
    }

};

}