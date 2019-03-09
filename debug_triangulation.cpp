//
// Created by ale on 19-3-5.
//

#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

//全局变量声明部分
Mat g_srcImage, g_dstImage, g_grayImage, g_maskImage, g_depthImage;//定义原始图、目标图、灰度图、掩膜图
int g_nFillMode = 1;//漫水填充的模式
int g_nLowDifference = 20, g_nUpDifference = 20;//负差最大值，正差最大值
int g_nConnectivity = 4;//表示floodFill函数标识符低八位的连通值
bool g_bIsColor = true;//是否为彩色图的标识符布尔值
bool g_bUseMask = false;//是否显示掩膜窗口的布尔值
int g_nNewMaskVal = 255;//新的重新绘制的像素值

//===============【onMouse()函数】=======================
static void onMouse(int event, int x, int y, int, void *) {
    //若鼠标左键没有按下，便返回
    if (event != EVENT_LBUTTONDOWN)
        return;

    //-----------------【<1>调用floodFill函数之前的参数准备部分】-------------
    Point seed = Point(x, y);
    int LowDifference = g_nFillMode == 0 ? 0 : g_nLowDifference;
    int UpDifference = g_nFillMode == 0 ? 0 : g_nUpDifference;

    //标识符的0~7位为g_nConnectivity，8~15位为g_nNewMaskVal左移8位的值，16~23位为CV_FLOODFILL_FIXED_RANGE或者0
    int flags = g_nConnectivity + (g_nNewMaskVal << 8) + (g_nFillMode == 1 ? FLOODFILL_FIXED_RANGE : 0);

    //随机生成BGR值
    int b = (unsigned)theRNG() & 255;//随机返回一个0~255之间的值
    int g = (unsigned)theRNG() & 255;
    int r = (unsigned)theRNG() & 255;
    Rect ccomp;//定义重绘区域的最小边界矩阵区域

    Scalar newVal = g_bIsColor ? Scalar(b, g, r) : Scalar(r*0.299 + g * 0.587 + b * 0.114);

    Mat dst = g_bIsColor ? g_dstImage : g_grayImage;//目标图的赋值
    int area;

    //threshold(g_maskImage, g_maskImage, 1, 128, THRESH_BINARY);//阈值化，大于1的设置为128，小于1的设置为0
    g_maskImage = Scalar::all(0);//将mask所有元素设置为0
    area = floodFill(dst, g_maskImage, seed, newVal, &ccomp, Scalar(LowDifference, LowDifference, LowDifference),Scalar(UpDifference, UpDifference, UpDifference),flags);

    imshow("mask", g_maskImage);

    Mat mask_draw = g_maskImage.clone();
    Mat color_draw = g_srcImage.clone();
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(mask_draw, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(-1, -1));
    //for (int i = 0; i < hierarchy.size(); i++)
    //{
    //Scalar color = Scalar(rand() % 255, rand() % 255, rand() % 255);
    //drawContours(color_draw, contours, i, color, CV_FILLED, 8, hierarchy);
    Scalar color = Scalar(0, 0, 255);


    vector<vector<Point>> contours_poly(contours.size());
    cv::Mat rdp = g_srcImage.clone();
    cout<<endl<<"rdp: "<<endl;
    //RamerDouglasPeucker(pointList, 20.0, pointListOut);
    /*for(size_t i=0;i< pointListOut.size()-1;i++)
    {
        //cout << pointListOut[i].x << "," << pointListOut[i].y << endl;
        cv::line(rdp, pointListOut[i], pointListOut[i+1], cv::Scalar(0,0,255), 1, CV_AA);
        cout<<pointListOut[i]<<","<< pointListOut[i+1]<<"\t";
    }*/
    for(int i=0; i<contours_poly.size(); ++i)
    {
        cv::approxPolyDP(contours[i], contours_poly[i], 5, true);
    }
    for(int i=0; i<contours_poly.size(); ++i)
    {
        color = Scalar((unsigned)theRNG() & 255, (unsigned)theRNG() & 255, (unsigned)theRNG() & 255);
        drawContours(rdp, contours_poly, i, color, 1, 8, hierarchy);
        drawContours(color_draw, contours, i, color, 1, 8, hierarchy);
    }
    cout<<endl;
    //}
    cout<<"hierarchy.size(): "<<hierarchy.size()<<endl;
    imshow("contours", color_draw);
    imshow("rdp", rdp);

    imshow("效果图", dst);
    cout << area << " 个像素被重新绘制\n";
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
        imshow("效果图", g_bIsColor ? g_dstImage : g_grayImage);

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