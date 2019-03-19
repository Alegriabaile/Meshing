//
// Created by ale on 19-3-5.
//

#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

extern int floodFillWithoutHoles(const Mat& gray, Mat& mask, const Point& seed, const double lowDiff, const double highDiff);

extern int antiClockContour(const Mat& mask, const Point& seed, vector<Point>& contour);
extern int clockWiseContour(const Mat& mask, const Point& seed, vector<Point>& contour);
extern int triangulateSimplePolygon(const vector<Point>& vertices, vector<Point>& triangles);

//全局变量声明部分
Mat g_srcImage, g_dstImage, g_grayImage, g_maskImage, g_depthImage;//定义原始图、目标图、灰度图、掩膜图

int g_nLowDifference = 20, g_nUpDifference = 20;//负差最大值，正差最大值

//===============【onMouse()函数】=======================
static void onMouse(int event, int x, int y, int, void *) {
    //若鼠标左键没有按下，便返回
    if (event != EVENT_LBUTTONDOWN)
        return;

    Mat mask_draw = g_maskImage.clone();
    Mat color_draw = g_srcImage.clone();
    Mat color_draw_triangle = g_srcImage.clone();
    Point seed = Point(x, y);
    double diff = 0.3*g_depthImage.at<float>(seed.y, seed.x);
    int temp = floodFillWithoutHoles(g_depthImage, mask_draw, seed, diff*20.0/double(g_nLowDifference), diff*20.0/double(g_nUpDifference));
    cout<<"temp: "<<temp<<endl;
    imshow("mask_draw", mask_draw);

    vector<Point> contour;
    //antiClockContour(mask_draw, seed, contour);
    //reverse(contour.begin(), contour.end());
    clockWiseContour(mask_draw, seed, contour);

    vector<Point> contour_approx;
    cv::approxPolyDP(contour, contour_approx, 3, true);
    for(size_t i=0;i< contour_approx.size()-1;i++)
    {
        //cout << pointListOut[i].x << "," << pointListOut[i].y << endl;
        cv::line(color_draw, contour_approx[i], contour_approx[i+1], cv::Scalar(0,0,255), 1, CV_AA);
        cout<<contour_approx[i]<<","<< contour_approx[i+1]<<"\t";
    }
    cout<<endl<<endl;
    cout<<"seed points: "<<seed<<endl;
    //cout<<"point n-4, n-3, n-2, n-1, 0, 1, 2, 3: "
    //<<contour[contour.size()-4]-seed<<", "<<contour[contour.size()-3]-seed<<", "
    //<<contour[contour.size()-2]-seed<<", "<<contour[contour.size()-1]-seed<<", "
    //<<contour[0]-seed<<", "<<contour[1]-seed<<", "<<contour[2]-seed<<", "<<contour[3]-seed<<endl<<endl;
    imshow("color_draw", color_draw);

    vector<Point> triangles;
    triangulateSimplePolygon(contour_approx, triangles);
    for(int i=0; i<triangles.size()/3; ++i)
    {
        cv::line(color_draw_triangle, triangles[i*3+0], triangles[i*3+1], cv::Scalar(0,0,255), 1, CV_AA);
        cv::line(color_draw_triangle, triangles[i*3+1], triangles[i*3+2], cv::Scalar(0,0,255), 1, CV_AA);
        cv::line(color_draw_triangle, triangles[i*3+2], triangles[i*3+0], cv::Scalar(0,0,255), 1, CV_AA);
        cv::circle(color_draw_triangle, triangles[i*3+0], 2, Scalar(0, 0, 0), -1, FILLED);
        cv::circle(color_draw_triangle, triangles[i*3+1], 2, Scalar(0, 0, 0), -1, FILLED);
        cv::circle(color_draw_triangle, triangles[i*3+2], 2, Scalar(0, 0, 0), -1, FILLED);
    }
    cout<<"sum(mask): "<<sum(mask_draw)/255<<"\t contour sizes:"<<contour.size()<<"\t triangle sizes: "<<triangles.size()/3<<endl;
    imshow("color_draw_triangle", color_draw_triangle);
    /*
vector<vector<Point>> contours;
vector<Vec4i> hierarchy;
findContours(mask_draw, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(-1, -1));

vector<vector<Point>> contours_poly(contours.size());
cv::Mat rdp = g_srcImage.clone();
cout<<endl<<"rdp: "<<endl;

for(int i=0; i<contours_poly.size(); ++i)
{
    cv::approxPolyDP(contours[i], contours_poly[i], 5, true);
}
for(int i=0; i<contours_poly.size(); ++i)
{
    Scalar color = Scalar(0,0,255);
    drawContours(rdp, contours_poly, i, color, 1, 8, hierarchy);
    drawContours(color_draw, contours, i, color, 1, 8, hierarchy);
}
cout<<endl;
//}
cout<<"hierarchy.size(): "<<hierarchy.size()<<endl;
imshow("contours", color_draw);
imshow("rdp", rdp);
 */
}

//main()函数
int main(int argc, char** argv) {
    //载入原图
    g_srcImage = imread("/home/ale/CLionProjects/i3d/data/Images/1000.jpg", 1);
    cv::Mat depth = imread("/home/ale/CLionProjects/i3d/data/Depths/1000.png", CV_LOAD_IMAGE_ANYCOLOR|CV_LOAD_IMAGE_ANYDEPTH);
    if (!g_srcImage.data || !depth.data)
    {
        printf("读取g_srcImage错误！\n");
        return false;
    }

    g_srcImage.copyTo(g_dstImage);//复制原图到目标图
    depth.convertTo(g_depthImage, CV_32FC1);

    g_maskImage.create(g_srcImage.rows + 2, g_srcImage.cols + 2, CV_8UC1);//用原图尺寸初始化掩膜mask

    namedWindow("效果图", WINDOW_AUTOSIZE);

    //创建Trackbar
    createTrackbar("负差最大值", "效果图", &g_nLowDifference, 255, 0);
    createTrackbar("正差最大值", "效果图", &g_nUpDifference, 255, 0);

    //鼠标回调函数
    setMouseCallback("效果图", onMouse, 0);

    //循环轮询按键
    while (1)
    {
        //先显示效果图
        imshow("效果图", g_dstImage);

        //获取按键键盘
        int c = waitKey(0);
        //判断ESC是否按下，按下退出
        if (c == 27)
        {
            cout << "程序退出........、\n";
            break;
        }
    }

    return 0;

}