#include "Plane.h"

Plane::Plane(void)
{

}

Plane::~Plane(void)
{

}

//ƽ��㷨ʽ����
Plane::Plane(double A, double B, double C, CvPoint3D64f point)
{
	this->A = A;
	this->B = B;
	this->C = C;
	this->point = point;
}

//ƽ��һ��ʽ����
Plane::Plane(double A, double B, double C, double D)
{
	this->AA = AA;
	this->BB = BB;
	this->CC = CC;
	this->DD = DD;
}

