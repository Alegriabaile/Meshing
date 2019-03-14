//
// Created by ale on 19-3-6.
//
#include <iostream>
#include <vector>
#include <algorithm>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class Solution {
public:
    int divide(int dividend, int divisor)
    {
        if(dividend == 0)
            return 0;

        long long int result = 0;
        long long int dividendl(dividend), divisorl(divisor);
        bool sign = false;
        if((dividendl<0) ^ (divisor<0))
            sign = true;

        dividendl = abs(dividendl);
        divisorl = abs(divisorl);

        int shift = 0;
        while(dividendl > divisorl<<shift)
        {
            shift++;
        }
        while(shift>=0)
        {
            if(dividendl >= divisorl<<shift)
            {
                result += (long long int)1<<shift;//默认为int
                dividendl -= divisorl<<shift;
            }
            shift--;
        }

        if(sign)
            result = -result;

        if(result > INT_MAX || result < INT_MIN)
            return INT_MAX;


        return result;
    }

};


int main()
{
/*
    Point v1(1, 0);
    Point v2(0, 1);
    Point v3;
    cout<<v1.cross(v2)<<", "<<v2.cross(v1)<<endl;


    list<int> ls{1,2,3, 4, 5};

    list<int>::iterator it_begin = ls.begin();
    list<int>::iterator it_end = ls.end();

    cout<<"*it_begin "<<*it_begin<<endl;
    cout<<"*it_end "<<*it_end<<endl;
    cout<<"*std::prev(it_begin) "<<*std::prev(it_begin)<<endl;
    cout<<"*std::next(it_end) "<<*std::next(it_end)<<endl;

    cout<<endl;
    for(list<int>::iterator it = ls.begin(); it!= ls.end(); ++it)
    {
        if(std::prev(it) == ls.end())
            cout<<" std::pre(it) ==  ls.end()"<<endl;
        cout<<"*it: "<<*it<<" *it->pre: "<<*std::prev(it)
        <<" *it->pre->pre: "<<*std::prev(std::prev(it))
        <<" *it->pre->pre->pre: "<<*std::prev(std::prev(std::prev(it)))<<endl;
    }

    for(list<int>::iterator it = ls.begin(); it!= ls.end(); ++it)
    {
        if(std::next(it) == ls.end())
            cout<<" std::next(it) ==  ls.end()"<<endl;
        cout<<"*it: "<<*it<<" *it->next: "<<*std::next(it)
        <<" *it->next->next: "<<*std::next(std::next(it))
        <<" *it->next->next->next: "<<*std::next(std::next(std::next(it)))
        <<endl;
    }
    cout<<endl;

*/
    /*
    int scalar(100);
    Point offset(200, 200);
    //vector<Point> vertices{Point(0, 0)*scalar+offset, Point(1, 0)*scalar+offset, Point(2, 1)*scalar+offset, Point(1, 2)*scalar+offset, Point(0, 1)*scalar+offset};

vector<Point> vertices{
        Point(0,0), Point(1,0), Point(2,1), Point(3.0), Point(4,2),
        Point(2,4), Point(3,2), Point(0,1), Point(2,1)
    };
    vector<Point> vertices{
            Point(3,1), Point(1,3), Point(-1,2), Point(1,2),
            Point(1,1), Point(0,0), Point(1,0)};

    cout<<"vertices: "<<endl;
    for(int i=0; i<vertices.size(); ++i)
        cout<<vertices[i];
    cout<<endl;

    vector<Point> triangles;
    triangulateSimplePolygon(vertices, triangles);
    cout<<"triangles: "<<endl;
    for(int i=0; i<triangles.size(); ++i)
        cout<<triangles[i];
    cout<<endl;

    Mat image(800, 800, CV_8UC3, Scalar(255,255,255));
    for(int i=0; i<triangles.size()/3; ++i)
    {
        cv::line(image, triangles[i*3+0]*scalar+offset, triangles[i*3+1]*scalar+offset, cv::Scalar(0,0,255), 1, CV_AA);
        cv::line(image, triangles[i*3+1]*scalar+offset, triangles[i*3+2]*scalar+offset, cv::Scalar(0,0,255), 1, CV_AA);
        cv::line(image, triangles[i*3+2]*scalar+offset, triangles[i*3+0]*scalar+offset, cv::Scalar(0,0,255), 1, CV_AA);
        cv::circle(image, triangles[i*3+0]*scalar+offset, 2, Scalar(0, 0, 0), -1, FILLED);
        cv::circle(image, triangles[i*3+1]*scalar+offset, 2, Scalar(0, 0, 0), -1, FILLED);
        cv::circle(image, triangles[i*3+2]*scalar+offset, 2, Scalar(0, 0, 0), -1, FILLED);
    }

    flip(image, image, 0);
    imshow("image", image);
    waitKey();*/

    Solution solution;
    cout<<solution.divide(-2147483648, -1);
    return 0;
}