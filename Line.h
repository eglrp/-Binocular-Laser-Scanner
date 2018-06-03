//����
#pragma once
//#include "stdafx.h"
#include <iostream>
#include <map>
#include <vector>
#include <cmath>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;
using namespace std;
class Line
{
public:
	Line(void);
	Line(double A,double B,double C,CvPoint3D32f point);
	~Line(void);
	void setA(double A){
		this->A=A;
	}
	void setB(double B){
		this->B=B;
	}
	void setC(double C){
		this->C=C;
	}
	void setPoint(CvPoint3D32f point){
		this->point=point;
	}
	double getA(){
		return this->A;
	}
	double getB(){
		return this->B;
	}
	double getC(){
		return this->C;
	}
	CvPoint3D32f getPoint(){
		return this->point;
	}
//private:
	//ֱ��һ��ʽ����A*(x-x0)+B*(y-y0)+C*(z-z0)=0;
	//��������Ϊn=(A,B,C)
	//�Գ�ʽ��
	double A;
	double B;
	double C;
	CvPoint3D32f point;
};

