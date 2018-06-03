//ƽ�㷨
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
class Plane
{

private:
	//ƽ��㷨ʽ����A*(x-x0)+B*(y-y0)+C*(z-z0)=0;
	//������Ϊn=(A,B,C)
	double A;
	double B;
	double C;
	CvPoint3D32f point;//(x0,y0,z0)
	//ƽ���һ��ʽ����A*x+B*y+C*z+D=0;
	double AA;
	double BB;
	double CC;
	double DD;
public:
	//ƽ��㷨ʽ����
	Plane(double A,double B,double C,CvPoint3D32f point);
	//ƽ��һ��ʽ����
	Plane(double AA,double BB,double CC,double DD);
	Plane(void);
	~Plane(void);
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
	//һ��ʽ����
	void setAA(double AA){
		this->AA=AA;
	}
	void setBB(double BB){
		this->BB=BB;
	}
	void setCC(double CC){
		this->CC=CC;
	}
	void setDD(double DD){
		this->DD=DD;
	}
	double getAA(){
		return this->AA;
	}
	double getBB(){
		return this->BB;
	}
	double getCC(){
		return this->CC;
	}
	double getDD(){
		return this->DD;
	}
};

