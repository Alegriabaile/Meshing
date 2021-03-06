//
// Created by ale on 19-3-10.
//
//# define DEBUG_FLOOD_FILL_WITHOUT_HOLES

#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

static bool isConnected(const double& baseVal, const double& newVal, const double& lowDiff, const double& highDiff)
{
    double minVal = (baseVal - lowDiff) > 1e-5 ? (baseVal-lowDiff) : 1e-5;
    double maxVal = (baseVal - highDiff) < 1e5 ? ( baseVal + highDiff) : 1e5;
    if( 1e-5 < baseVal && baseVal < 1e5 &&
        minVal <= newVal && newVal <= maxVal)
        return true;
    
    return false;
}

static int floodFillUp(vector<Point>& points, const Mat& gray, const Point& curPoint, const double& seedVal, const int& radius, const double& lowDiff, const double& highDiff, const bool isStrict = false)
{
    if(radius >0 && curPoint.y-1>=0)
    {
        double curVal = gray.at<float>(curPoint.y, curPoint.x);
        double nextVal = gray.at<float>(curPoint.y-1, curPoint.x);
        if(isConnected(curVal, nextVal, lowDiff, highDiff) && (!isStrict || isConnected(seedVal, nextVal, lowDiff, highDiff)))
        {
            points.push_back(Point(curPoint.x, curPoint.y-1));
            return floodFillUp(points, gray, Point(curPoint.x, curPoint.y-1), seedVal, radius-1, lowDiff, highDiff, isStrict)+1;
        }
    }

    return 0;
}

static int floodFillDown(vector<Point>& points, const Mat& gray, const Point& curPoint, const double& seedVal, const int& radius, const double& lowDiff, const double& highDiff, const bool isStrict = false)
{
    if(radius >0 && curPoint.y+1<gray.rows)
    {
        double curVal = gray.at<float>(curPoint.y, curPoint.x);
        double nextVal = gray.at<float>(curPoint.y+1, curPoint.x);
        if(isConnected(curVal, nextVal, lowDiff, highDiff) && (!isStrict || isConnected(seedVal, nextVal, lowDiff, highDiff)))
        {
            points.push_back(Point(curPoint.x, curPoint.y+1));
            return floodFillDown(points, gray, Point(curPoint.x, curPoint.y+1), seedVal, radius-1, lowDiff, highDiff, isStrict)+1;
        }
    }

    return 0;
}

static int floodFillLeft(vector<Point>& points, const Mat& gray, const Point& curPoint, const double& seedVal, const int& radius, const double& lowDiff, const double& highDiff, const bool isStrict = false)
{
    if(radius >0 && curPoint.x-1>=0)
    {
        double curVal = gray.at<float>(curPoint.y, curPoint.x);
        double nextVal = gray.at<float>(curPoint.y, curPoint.x-1);
        if(isConnected(curVal, nextVal, lowDiff, highDiff) && (!isStrict || isConnected(seedVal, nextVal, lowDiff, highDiff)))
        {
            points.push_back(Point(curPoint.x-1, curPoint.y));
            return floodFillLeft(points, gray, Point(curPoint.x-1, curPoint.y), seedVal, radius-1, lowDiff, highDiff, isStrict)+1;
        }
    }

    return 0;
}

static int floodFillRight(vector<Point>& points, const Mat& gray, const Point& curPoint, const double& seedVal, const int& radius, const double& lowDiff, const double& highDiff, const bool isStrict = false)
{
    if(radius >0 && curPoint.x+1<gray.cols)
    {
        double curVal = gray.at<float>(curPoint.y, curPoint.x);
        double nextVal = gray.at<float>(curPoint.y, curPoint.x+1);
        if(isConnected(curVal, nextVal, lowDiff, highDiff) && (!isStrict || isConnected(seedVal, nextVal, lowDiff, highDiff)))
        {
            points.push_back(Point(curPoint.x+1, curPoint.y));
            return floodFillRight(points, gray, Point(curPoint.x+1, curPoint.y), seedVal, radius-1, lowDiff, highDiff, isStrict)+1;
        }
    }

    return 0;
}

int floodFillWithoutHoles(const Mat& gray, Mat& mask, const Point& seed, const double lowDiff, const double highDiff, const bool isStrict = false)
{
    if(gray.empty())
        return -1;
    if(gray.channels() != 1)
        return -2;
    if(gray.type() != CV_32FC1)
        return -3;
    if(seed.y<0 || seed.y>gray.rows-1 || seed.x<0 || seed.y>gray.cols-1)
        return -4;

    if(mask.channels()!=1 || mask.size()!=gray.size()+Size(2,2))
    //mask = Mat(gray.size()+Size(2,2), CV_8UC1, Scalar(0));
    mask = Mat(gray.size(), CV_8UC1, Scalar(0));
    const double seedVal = gray.at<float>(seed.y, seed.x);
    const int radius = gray.rows<gray.cols?gray.rows/16:gray.cols/16;
    vector<Point> base_line;
    base_line.reserve(radius*2+1);
    base_line.push_back(seed);

    floodFillUp(base_line, gray, seed, seedVal, radius, lowDiff, highDiff, isStrict);
    floodFillDown(base_line, gray, seed, seedVal, radius, lowDiff, highDiff, isStrict);

    vector<Point> results;
    results.assign(base_line.begin(), base_line.end());
    for(vector<Point>::const_iterator iter = base_line.cbegin();
        iter != base_line.cend(); ++iter)
    {
        floodFillLeft(results, gray, *iter, seedVal, radius, lowDiff, highDiff, isStrict);
        floodFillRight(results, gray, *iter, seedVal, radius, lowDiff, highDiff, isStrict);
    }

    for(vector<Point>::const_iterator iter = results.cbegin();
        iter != results.cend(); ++iter)
    {
        //mask.at<uchar>((*iter).y+1, (*iter).x+1) = 255;
        mask.at<uchar>((*iter).y, (*iter).x) = 255;
    }
#ifdef DEBUG_FLOOD_FILL_WITHOUT_HOLES
    cout<<"seed: "<<seed<<endl;
    cout<<"base_line: "<<endl;
    for(vector<Point>::const_iterator iter = base_line.cbegin();
        iter != base_line.cend(); ++iter)
    {
        cout<<(*iter)<<"\t";
    }
    cout<<endl;
    cout<<"results.size(): "<<results.size()<<endl<<endl;
    //imshow("mask", mask);
    //waitKey();
#endif

    return 0;
}