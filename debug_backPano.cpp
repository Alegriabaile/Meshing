//
// Created by ale on 19-3-19.
//

#include <vector>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

//pano_image_f.size() == pano_depth_f.size()
void GenerateBackPano(cv::Mat& pano_image_f, cv::Mat& pano_depth_f, cv::Mat& pano_image_b, cv::Mat& pano_depth_b, const int icount = 30)//iterator count)
{
    //init
    cv::Mat image_f(pano_image_f.size()+Size(icount*2, icount*2), CV_8UC3, Scalar(0,0,0));
    cv::Mat image_b(pano_image_f.size()+Size(icount*2, icount*2), CV_8UC3, Scalar(0,0,0));
    cv::Mat depth_f(pano_depth_f.size()+Size(icount*2, icount*2), CV_32FC1, Scalar(0.0f));
    cv::Mat depth_b(pano_depth_f.size()+Size(icount*2, icount*2), CV_32FC1, Scalar(0.0f));

    //image_f(Rect(icount, icount, pano_image_f.cols, pano_image_f.rows)) = pano_image_f.clone();
    //depth_f(Rect(icount, icount, pano_depth_f.cols, pano_depth_f.rows)) = pano_depth_f.clone();
    Mat roi= image_f(Rect(icount, icount, pano_image_f.cols, pano_image_f.rows));
    pano_image_f.copyTo(roi);
    roi = depth_f(Rect(icount, icount, pano_depth_f.cols, pano_depth_f.rows));
    pano_depth_f.copyTo(roi);

    //imshow("pano_image_f", pano_image_f);
    //imshow("pano_depth_f", pano_depth_f);
    imshow("image_f", image_f);
    imshow("depth_f", depth_f);
    //waitKey();

    float d, dl, dr, dt, db;//current depth, left-right-top-bottom depth.
    float diff = 0.05;
    for(int h=0; h<pano_depth_f.rows; ++h)
    {
        for(int w=0; w<pano_depth_f.cols; ++w)
        {
            //只处理有深度值的邻域，否则跳过
            d = depth_f.at<float>(h+icount, w+icount);
            if(!(d > 1e-6f))
                continue;
            dl = depth_f.at<float>(h+icount, w-1+icount);
            dr = depth_f.at<float>(h+icount, w+1+icount);
            dt = depth_f.at<float>(h-1+icount, w+icount);
            db = depth_f.at<float>(h+1+icount, w+icount);
            //left
            if(dl-d>diff*d || !(dl > 1e-6f))
                depth_b.at<float>(h+icount, w-1+icount) = d;
            //right
            if(dr-d>diff*d || !(dr > 1e-6f))
                depth_b.at<float>(h+icount, w+1+icount) = d;
            //top
            if(dt-d>diff*d || !(dt > 1e-6f))
                depth_b.at<float>((h-1+icount, w+icount)) = d;
            //bottom
            if(db-d>diff*d || !(db > 1e-6f))
                depth_b.at<float>((h+1+icount, w+icount)) = d;
        }
    }

    cout<<"begin iterate"<<endl;
    //iterate icount-1
    for(int k = 0; k<icount-1; ++k)
    {
        cv::Mat depth_b_t = depth_b.clone();
        for(int h=1; h<depth_b.rows-1; ++h)
        {
            for(int w=1; w<depth_b.cols-1; ++w)
            {
                //只处理有深度值的邻域，否则跳过
                d = depth_b.at<float>(h, w);
                if(!(d > 1e-6f))
                    continue;
                dl = depth_b.at<float>(h, w-1);
                dr = depth_b.at<float>(h, w+1);
                dt = depth_b.at<float>(h-1, w);
                db = depth_b.at<float>(h+1, w);

                float dl_f = depth_f.at<float>(h, w-1);
                float dr_f = depth_f.at<float>(h, w+1);
                float dt_f = depth_f.at<float>(h-1, w);
                float db_f = depth_f.at<float>(h+1, w);
                //left
                if(!(dl > 1e-6f) || dl>d )//&& d <= depth_f.at<float>(h, w-1))
                    if(dl_f>1e-6f&&dl_f>d || !(dl_f>1e-6f))
                        depth_b_t.at<float>(h, w-1) = d;
                //right
                if(!(dr > 1e-6f) || dr>d )//&& d <= depth_f.at<float>(h, w+1))
                    if(dr_f>1e-6f&&dr_f>d || !(dr_f>1e-6f))
                        depth_b_t.at<float>(h, w+1) = d;
                //top
                if(!(dt > 1e-6f) || dt>d)//&& d <= depth_f.at<float>(h-1, w))
                    if(dt_f>1e-6f&&dt_f>d || !(dt_f>1e-6f))
                        depth_b_t.at<float>(h-1, w) = d;
                //bottom
                if(!(db > 1e-6f) || db>d)//&& d <= depth_f.at<float>(h+1, w))
                    if(db_f>1e-6f&&db_f>d || !(db_f>1e-6f))
                        depth_b_t.at<float>(h+1, w) = d;
            }
        }

        depth_b = depth_b_t;
    }


    //prune
    for(int h=0; h<depth_b.rows; ++h)
    {
        for(int w=0; w<depth_b.cols; ++w)
        {
            //只处理有深度值的邻域，否则跳过
            d = depth_b.at<float>(h, w);
            if(!(d > 1e-6f))
                continue;

            float df = depth_f.at<float>(h, w);

            if(df>1e-6f && df<d)
                depth_b.at<float>(h, w) = 0.0f;
        }
    }
    cv::Mat prune = depth_f>depth_b;
    cv::Mat temp_depth_b;
    depth_b.copyTo(temp_depth_b, prune);
    imshow("temp_depth_b", temp_depth_b);

    //inpainting
    cout<<"begin inpainting"<<endl;
    cv::Mat mask = depth_b>0;
    cout<<sum(mask)<<endl;
    inpaint(image_f, mask, image_b, 5, CV_INPAINT_NS);



    imshow("image_b", image_b);
    imshow("depth_b", depth_b);
    waitKey();
    //color inpainting

}


//main()函数
int main(int argc, char** argv)
{
    //载入原图
    cv::Mat pano_image_f = imread("pano_result.jpg", 1);
    cv::Mat pano_depth_f = imread("pano_result.png", CV_LOAD_IMAGE_ANYCOLOR|CV_LOAD_IMAGE_ANYDEPTH);
    if (!pano_image_f.data || !pano_depth_f.data)
    {
        printf("读取g_srcImage错误！\n");
        return false;
    }

    pano_depth_f.convertTo(pano_depth_f, CV_32FC1);

    double minVal, maxVal;
    minMaxLoc(pano_depth_f, &minVal, &maxVal);
    cout<<minVal<<", "<<maxVal<<endl;
    cv::Mat pano_image_b, pano_depth_b;
    GenerateBackPano(pano_image_f, pano_depth_f, pano_image_b, pano_depth_b);

    return 0;
}
