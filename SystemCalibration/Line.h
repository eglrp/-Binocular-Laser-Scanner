#pragma once

#include <iostream>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>

using namespace std;
using namespace cv;

class Line
{
public:
	Line(void);
	Line(double A, double B, double C, CvPoint3D64f point);
	~Line(void);

	void setA(double A)
	{
		this->A = A;
	}
	void setB(double B)
	{
		this->B = B;
	}
	void setC(double C)
	{
		this->C = C;
	}
	double getA()
	{
		return this->A;
	}
	double getB()
	{
		return this->B;
	}
	double getC()
	{
		return this->C;
	}
	CvPoint3D64f getPoint()
	{
		return this->point;
	}
	void setPoint(CvPoint3D64f point)
	{
		this->point = point;
	}

private:
	//ֱ��һ��ʽ����ΪA*(x-x0)+B*(y-y0)+C(z-z0) = 0,��������Ϊn = (A, B, C)
	double A;
	double B;
	double C;
	CvPoint3D64f point;
};