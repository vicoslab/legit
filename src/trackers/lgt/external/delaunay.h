
#ifndef __LGT_EXTERNAL_DELAUNAY_H
#define __LGT_EXTERNAL_DELAUNAY_H

#include <vector>

#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;

int delaunay_triangulation(Point2f* points, int size, int** edges);

int convex_hull(Point2f* points, int size, Point2f** hull);

void delaunay_neihgbours(Point2f* points, int size, vector<vector<int> >& neighborhoods);

void fillconvex(Mat& image, Point2f* points, int count, Scalar color);

#endif