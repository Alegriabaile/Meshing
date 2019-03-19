//
// Created by ale on 19-3-12.
//
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

extern int writeToSimpleObj(const Mat& depth, const vector<Point>& triangles, const float& f,
                     const string& filePath, const string& fileName = "temp", const string& texFileName = "texture.jpg");
extern int writeToCustomizedFile(const Mat& depth, const vector<Point>& triangles, const float& f,
                          const string& filePath, const string& fileName = "temp");

extern int floodFillWithoutHoles(const Mat& gray, Mat& mask, const Point& seed, const double lowDiff, const double highDiff, const bool isStrict = false);
extern int clockWiseContour(const Mat& mask, const Point& seed, vector<Point>& contour);
extern int triangulateSimplePolygon(const vector<Point>& vertices, vector<Point>& triangles);

int main()
{
    cv::Mat srcImage = imread("/home/ale/CLionProjects/i3d/casual3d_data3/Images/1000.jpg", 1);
    cv::Mat depthConst = imread("/home/ale/CLionProjects/i3d/casual3d_data3/Depths/1000.png", CV_LOAD_IMAGE_ANYCOLOR|CV_LOAD_IMAGE_ANYDEPTH);
    cv::Mat dstImage, depthImage, maskImage;
    
    if (!srcImage.data || !depthConst.data)
    {
        printf("读取srcImage错误！\n");
        return false;
    }

    srcImage.copyTo(dstImage);//复制原图到目标图
    depthConst.convertTo(depthImage, CV_32FC1);
    maskImage.create(srcImage.rows, srcImage.cols, CV_8UC1);//用原图尺寸初始化掩膜mask


    double minVal, maxVal;
    Point minLoc, maxLoc;
    minMaxLoc(depthImage, &minVal, &maxVal, &minLoc, &maxLoc);
    vector<Point> triangles;
    while(maxVal > 0)
    {
        Mat mask_draw = maskImage.clone();
        double diff = 0.05*depthImage.at<float>(maxLoc.y, maxLoc.x);
        int temp = floodFillWithoutHoles(depthImage, mask_draw, maxLoc, diff, diff, false);
        imshow("mask_draw", mask_draw);

        //将depthImage清零
        for(int i=0; i<mask_draw.rows; ++i)
        {
            for(int j=0; j<mask_draw.cols; ++j)
            {
                if(mask_draw.at<uchar>(i,j) > 0)
                {
                    depthImage.at<float>(i,j) = 0;
                    dstImage.at<Vec3b>(i,j) = Vec3b(0,0,0);
                }
            }
        }

        //生成简单多边形的外轮廓
        vector<Point> contour;
        clockWiseContour(mask_draw, maxLoc, contour);

        //简化轮廓，用更少的点表示离散轮廓
        vector<Point> contour_approx;
        cv::approxPolyDP(contour, contour_approx, 3, false);

        //从简单多边形生成三角形
        vector<Point> triangles_temp;
        triangulateSimplePolygon(contour_approx, triangles_temp);
        for(int i=0; i<triangles_temp.size(); ++i)
            triangles.push_back(triangles_temp[i]);

        //为下一轮作准备，若深度图上的所有点都已经遍历，则 (maxVal>0) == false, 跳出循环
        minMaxLoc(depthImage, &minVal, &maxVal, &minLoc, &maxLoc);
    }

    depthConst.convertTo(depthImage, CV_32FC1);
    writeToSimpleObj(depthImage, triangles, 525, string("./"));
    //writeToCustomizedFile(depthImage, triangles, 525, "./");
    
    return 0;
}
