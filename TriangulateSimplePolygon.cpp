//
// Created by ale on 19-3-8.
//
#include <vector>
#include <list>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

//调试用
//#define DEBUG_TRIANGULATE_SIMPLE_POLYGON

//A:i-1, B:i, C:i+1
static bool isConcave(const Point& A, const Point& B, const Point& C)
{
    //if(v1.cross(v2)>0, 逆时针旋转
    //if(v1.cross(v2)<0, 顺时针旋转
    //AB->BC逆时针则为凸点，否则为凹点
    return ((B - A).cross(C - B) < 0);//顺时针则定为凹点
}

//逆时针三角形ABC与凹点，若凹点在三角形里面，则凹点在AB，BC和CA左边
static bool isInside(const Point& A, const Point& B, const Point& C, const Point& concave)
{
    //if(v1.cross(v2)>0, 逆时针旋转
    if( (B-A).cross(concave-A)>0 && (C-B).cross(concave-B)>0 && (A-C).cross(concave-C)>0)
        return true;

    return false;
}

//A:i-1, B:i, C:i+1; B is an ear tip if all concaves are outside triangle ABC
static bool isEarTip(const Point& A, const Point& B, const Point& C, const vector<bool> & concave, const vector<Point>& vertices)
{
    for(int i=0; i<concave.size(); ++i)
    {
        if(!concave[i])
            continue;
        if(isInside(A, B, C, vertices[i]))
            return false;
    }

    return true;
}

//是不是可以只更新相邻的两个顶点？
static void updateLists(const vector<Point>& vertices, list<int>& indices, vector<bool>& concave, vector<bool>& eartips)
{
    //状态清零
    for(int i=0; i<concave.size(); ++i)
    {
        concave[i] = false;
        eartips[i] = false;
    }
    //寻找凹点
    for(list<int>::iterator iter = indices.begin(); iter!=indices.end(); ++iter)
    {
        list<int>::iterator iter_pre = std::prev(iter);
        list<int>::iterator iter_next = std::next(iter);

        if(iter_pre == indices.end())
            iter_pre = std::prev(iter_pre);
        if(iter_next == indices.end())
            iter_next = std::next(iter_next);

        Point A = vertices[*iter_pre];
        Point B = vertices[*iter];
        Point C = vertices[*iter_next];

        if(isConcave( A, B, C))
            concave[*iter] = true;
    }
    //更新EarTip点
    for(list<int>::iterator iter = indices.begin(); iter!=indices.end(); ++iter)
    {
        list<int>::iterator iter_pre = std::prev(iter);
        list<int>::iterator iter_next = std::next(iter);

        if(iter_pre == indices.end())
            iter_pre = std::prev(iter_pre);
        if(iter_next == indices.end())
            iter_next = std::next(iter_next);

        Point A = vertices[*iter_pre];
        Point B = vertices[*iter];
        Point C = vertices[*iter_next];

        if(!concave[*iter] && isEarTip(A, B, C, concave, vertices))
            eartips[*iter] = true;
    }
}
//只更新相邻的两个顶点？
static void updateLists(const vector<Point>& vertices, const list<int>& indices, const list<int>::iterator& old_next_, vector<bool>& concave, vector<bool>& eartips)
{
    //定位旧节点的前顶点与后顶点
    list<int>::iterator old_pre = std::prev(old_next_);
    list<int>::iterator old_next = old_next;
    if(old_pre == indices.end())
        old_pre = std::prev(old_pre);
    if(old_next == indices.end())
        old_next = std::next(old_next);

    //先将状态清空
    concave[*old_pre] = false;
    concave[*old_next] = false;
    eartips[*old_pre] = false;
    eartips[*old_next] = false;

    //update concave pre
    list<int>::iterator iter_pre1 = std::prev(old_pre);
    list<int>::iterator iter_next1 = old_next;
    if (iter_pre1 == indices.end())
        iter_pre1 = std::prev(iter_pre1);

    Point A1 = vertices[*iter_pre1];
    Point B1 = vertices[*old_pre];
    Point C1 = vertices[*iter_next1];
    if(isConcave( A1, B1, C1))
            concave[*old_pre] = true;

    //update concave next
    list<int>::iterator iter_pre2 = old_pre;
    list<int>::iterator iter_next2 = std::next(old_next);
    if (iter_next2 == indices.end())
        iter_next2 = std::next(iter_next2);

    Point A2 = vertices[*iter_pre2];
    Point B2 = vertices[*old_next];
    Point C2 = vertices[*iter_next2];
    if(isConcave( A2, B2, C2))
        concave[*old_next] = true;

    //更新EarTip点
    if(!concave[*old_pre])
        eartips[*old_pre] = isEarTip(A1, B1, C1, concave, vertices);
    if(!concave[*old_next])
        eartips[*old_next] = isEarTip(A2, B2, C2, concave, vertices);
}

int triangulateSimplePolygon(const vector<Point>& vertices, vector<Point>& triangles)
{
    //vector<Point> triangles;
    triangles.clear();

    list<int> indices(vertices.size());
    list<int>::iterator iter = indices.begin();
    for(int i=0; i<vertices.size(); ++i)
    {
        *iter = i;
        iter++;
    }

    //记录凹点与耳点
    vector<bool> concave(vertices.size(), false);
    vector<bool> eartips(vertices.size(), false);


    list<int>::iterator old_next;
    while(indices.size() >=3)
    {
#ifdef DEBUG_TRIANGULATE_SIMPLE_POLYGON
        cout<<"indices.size(): "<<indices.size()<<endl;

        cout<<"concave indices: ";
        for(int i=0; i<concave.size(); ++i)
        {
            if(concave[i])
                cout<<i<<"\t";
        }
        cout<<endl;

        cout<<"eartips indices: ";
        for(int i=0; i<eartips.size(); ++i)
        {
            if(eartips[i])
                cout<<i<<"\t";
        }
        cout<<endl;

        cout<<"triangles.size(): "<<triangles.size()<<endl;

        imshow("debug", Mat(800, 800, CV_8UC1, Scalar(0)));
        waitKey();
#endif
        updateLists(vertices, indices, concave, eartips);
        for(int i=0; i<eartips.size(); ++i)
        {
            if (eartips[i])
            {
                list<int>::iterator iter = find(indices.begin(), indices.end(), i);
                list<int>::iterator iter_pre = std::prev(iter);
                list<int>::iterator iter_next = std::next(iter);

                if (iter_pre == indices.end())
                    iter_pre = std::prev(iter_pre);
                if (iter_next == indices.end())
                    iter_next = std::next(iter_next);

                triangles.push_back(vertices[*iter_pre]);
                triangles.push_back(vertices[*iter]);
                triangles.push_back(vertices[*iter_next]);

                indices.erase(iter);

#ifdef DEBUG_TRIANGULATE_SIMPLE_POLYGON
                if(iter == indices.end())
                    cout<<"error in find eartips i in indices"<<endl;
                cout<<"iter: "<<*iter<<" i: "<<i;
                cout<<" iter_pre: "<<*iter_pre<<
                " iter_next: "<<*iter_next<<endl<<endl;
#endif
                break;
            }
        }
    }
    indices.clear();
    concave.clear();
    eartips.clear();

    return 0;
}