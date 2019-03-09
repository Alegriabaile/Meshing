//2D implementation of the Ramer-Douglas-Peucker algorithm
//By Tim Sheerman-Chase, 2016
//Released under CC0
//https://en.wikipedia.org/wiki/Ramer%E2%80%93Douglas%E2%80%93Peucker_algorithm

#include <iostream>
#include <cmath>
#include <utility>
#include <vector>
#include <stdexcept>

#include <opencv2/opencv.hpp>

//using namespace cv;
using namespace std;
using namespace cv;
//typedef std::pair<double, double> Point;

double PerpendicularDistance(const Point &pt, const Point &lineStart, const Point &lineEnd)
{
	double dx = lineEnd.x - lineStart.x;
	double dy = lineEnd.y - lineStart.y;

	//Normalise
	double mag = pow(pow(dx,2.0)+pow(dy,2.0),0.5);
	if(mag > 0.0)
	{
		dx /= mag; dy /= mag;
	}

	double pvx = pt.x - lineStart.x;
	double pvy = pt.y - lineStart.y;

	//Get dot product (project pv onto normalized direction)
	double pvdot = dx * pvx + dy * pvy;

	//Scale line direction vector
	double dsx = pvdot * dx;
	double dsy = pvdot * dy;

	//Subtract this from pv
	double ax = pvx - dsx;
	double ay = pvy - dsy;

	return pow(pow(ax,2.0)+pow(ay,2.0),0.5);
}

void RamerDouglasPeucker(const vector<Point> &pointList, double epsilon, vector<Point> &out)
{
	if(pointList.size()<2)
		throw invalid_argument("Not enough points to simplify");

	// Find the point with the maximum distance from line between start and end
	double dmax = 0.0;
	size_t index = 0;
	size_t end = pointList.size()-1;
	for(size_t i = 1; i < end; i++)
	{
		double d = PerpendicularDistance(pointList[i], pointList[0], pointList[end]);
		if (d > dmax)
		{
			index = i;
			dmax = d;
		}
	}

	// If max distance is greater than epsilon, recursively simplify
	if(dmax > epsilon)
	{
		// Recursive call
		vector<Point> recResults1;
		vector<Point> recResults2;
		vector<Point> firstLine(pointList.begin(), pointList.begin()+index+1);
		vector<Point> lastLine(pointList.begin()+index, pointList.end());
		RamerDouglasPeucker(firstLine, epsilon, recResults1);
		RamerDouglasPeucker(lastLine, epsilon, recResults2);
 
		// Build the result list
		out.assign(recResults1.begin(), recResults1.end()-1);
		out.insert(out.end(), recResults2.begin(), recResults2.end());
		if(out.size()<2)
			throw runtime_error("Problem assembling output");
	} 
	else 
	{
		//Just return start and end points
		out.clear();
		out.push_back(pointList[0]);
		out.push_back(pointList[end]);
	}
}

int main()
{
	vector<Point> pointList;
	vector<Point> pointListOut;

	double pts[] = {
			172, 692, 290, 571, 417, 469, 349, 378, 272, 430,
			245, 491, 155, 446, 156, 344, 193, 234, 376, 188,
			583, 147, 545, 87, 649, 98, 662, 150, 614, 188,
			543, 261, 542, 327, 808, 329, 853, 414, 733, 466,
			676, 562, 549, 600, 455, 671, 522, 711, 489, 739,
			344, 763, 345, 690, 309, 669, 235, 684, 172, 692, 172, 692};

	for(int i=0; i<sizeof(pts)/sizeof(double); i=i+2)
		pointList.push_back(Point(pts[i], pts[i+1]));
	/*
	pointList.push_back(Point(1.0, 0.1)*80);
	pointList.push_back(Point(2.0, -0.1)*80);
	pointList.push_back(Point(3.0, 5.0)*80);
	pointList.push_back(Point(4.0, 6.0)*80);
	pointList.push_back(Point(5.0, 7.0)*80);
	pointList.push_back(Point(6.0, 8.1)*80);
	pointList.push_back(Point(7.0, 9.0)*80);
	pointList.push_back(Point(8.0, 9.0)*80);
	pointList.push_back(Point(9.0, 9.0)*80);
	//pointList.push_back(Point(0.0, 0.0)*80);
	*/

	cv::approxPolyDP(pointList, pointListOut, 20, true);
	//RamerDouglasPeucker(pointList, 20.0, pointListOut);

	cv::Mat src(800, 1000, CV_8UC3, cv::Scalar(255, 255, 255));
	cv::Mat rdp = src.clone();

	cout << "src: " << endl;
    for(size_t i=0;i< pointList.size()-1;i++)
    {
        //cout << pointListOut[i].x << "," << pointListOut[i].y << endl;
        cv::line(src, pointList[i], pointList[i+1], cv::Scalar(0,0,255),1,CV_AA);
        cout<<pointList[i]<<","<< pointList[i+1]<<"\t";
    }
    cout<<endl<<"rdp: "<<endl;
	for(size_t i=0;i< pointListOut.size()-1;i++)
	{
		//cout << pointListOut[i].x << "," << pointListOut[i].y << endl;
        cv::line(rdp, pointListOut[i], pointListOut[i+1], cv::Scalar(0,0,255),1,CV_AA);
		cout<<pointListOut[i]<<","<< pointListOut[i+1]<<"\t";
	}
	cout<<endl;

	imshow("src", src);
	imshow("rdp", rdp);

	waitKey();

	return 0;
}
