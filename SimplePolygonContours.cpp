//
// Created by ale on 19-3-11.
//

#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

static Point GetPointUp(const Mat& mask, const Point& current)
{
    if(current.y-1 >= 0 && mask.at<uchar>(current.y-1, current.x)>0)
        return GetPointUp(mask, Point(current.x, current.y-1));

    return current;
}
static Point GetPointDown(const Mat& mask, const Point& current)
{
    if(current.y+1 < mask.rows && mask.at<uchar>(current.y+1, current.x)>0)
        return GetPointDown(mask, Point(current.x, current.y+1));

    return current;
}
static Point GetPointLeft(const Mat& mask, const Point& current)
{
    if(current.x-1 >= 0 && mask.at<uchar>(current.y, current.x-1)>0)
        return GetPointLeft(mask, Point(current.x-1, current.y));

    return current;
}
static Point GetPointRight(const Mat& mask, const Point& current)
{
    if(current.x+1 < mask.cols && mask.at<uchar>(current.y, current.x+1)>0)
        return GetPointRight(mask, Point(current.x+1, current.y));

    return current;
}

int antiClockContour(const Mat& mask, const Point& seed, vector<Point>& contour)
{
    if(mask.empty())
        return -1;
    if(mask.type() != CV_8UC1)
        return -2;
    if( seed.x>mask.cols-1 || seed.x<0 ||
        seed.y>mask.rows-1 || seed.y<0 ||
        !(mask.at<uchar>(seed.y, seed.x)>0) )
        return -3;

    contour.clear();

    Point top = GetPointUp(mask, seed);
    Point bottom = GetPointDown(mask, seed);

    for(int i=0; i<=bottom.y-top.y; ++i)
        contour.push_back(GetPointLeft(mask, top+Point(0, i)));
    for(int i=0; i<=bottom.y-top.y; ++i)
        contour.push_back(GetPointRight(mask, bottom-Point(0, i)));

    return 0;
}

/*opencv坐标系
 *   o —— > x
 *   |
 *   v
 *  y
 *  由于与普通坐标系不一致，故在处理向量夹角时要将上下翻转，
 *  等效做法可以将轮廓顶点集顺时针输出
 * */
int clockWiseContour(const Mat& mask, const Point& seed, vector<Point>& contour)
{
    if(mask.empty())
        return -1;
    if(mask.type() != CV_8UC1)
        return -2;
    if( seed.x>mask.cols-1 || seed.x<0 ||
        seed.y>mask.rows-1 || seed.y<0 ||
        !(mask.at<uchar>(seed.y, seed.x)>0) )
        return -3;

    contour.clear();

    Point top = GetPointUp(mask, seed);
    Point bottom = GetPointDown(mask, seed);

    for(int i=0; i<=bottom.y-top.y; ++i)
        contour.push_back(GetPointRight(mask, top+Point(0, i)));
    for(int i=0; i<=bottom.y-top.y; ++i)
        contour.push_back(GetPointLeft(mask, bottom-Point(0, i)));

    return 0;
}

int genSimplePolygonContours(const Mat& mask, const Point& seed, vector<Point>& contour, const bool isClockWise = true)
{
    if(isClockWise)
        clockWiseContour(mask, seed, contour);
    else
        antiClockContour(mask, seed, contour);

    return 0;
}