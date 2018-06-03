//��ʽ��
#pragma once
//#include "stdafx.h"
#include <iostream>
#include <map>
#include <vector>
#include <cmath>
#include<fstream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "Plane.h"
#include "Line.h"
//#include "BackgroudEquations.h"
using namespace cv;
using namespace std;

class Equations
{
public:
	Equations();//int width,int height,double f,double M
	~Equations(void);
	int width;//ͼƬ���
	int height;//ͼƬ����
	//double M;//
	//�궨���
	CvPoint3D32f leftcenter;
	CvPoint3D32f rightcenter;

	
	//�洢ͼ����ȡ�Ķ�ά������
	vector<CvPoint2D32f> allPointsleft;//��ͼ���м����	
	vector<CvPoint2D32f> allPointsright;//��ͼ���м����
	vector<CvPoint2D32f> matchPoints;//��ͼ�е�ƥ���
	//vector<CvPoint2D32f> allCalRightPoints;//�洢����õ�����ͼ��Ӧ������
	//CvPoint2D32f *allCalRightPoints;

	//�洢�����ĵ���ά������
	vector<CvPoint3D32f> allPoints3d;
	//double Tx;
	//double cx;
	//double cy;
	//BackgroudEquations backgroundEquation;//��ȡ�����������
	double R[3][3];
	double T[3];
	float f;    //����(����)
	float fL;    //����(����)��
	float fR;    //����(����)��
	float M;   //Tx(mm)
	float dx, dy; //�ֱ����x,y�᷽��һ�����صĿ��(mm/����)
	CvMat *RotationMat;//��ת����
	CvMat *TransationMat;//ƽ�ƾ���

	// ����ʵ�ʿռ��е�3����  ����ƽ�� point1,point2,point3 Ϊ����������ǹ��ߵĵ㣻 
	Plane CreatePlaneEquation(CvPoint3D32f point1,CvPoint3D32f point2,CvPoint3D32f point3);
	//��ƽ��һ��ʽ���� 
	Plane CreatePlaneEquationGeneral(double A,double B,double C,double D);
	//�ռ��е�2�����ֱ��
	Line CreatLineEquation(CvPoint3D32f point1, CvPoint3D32f point2);
	//�����ཻƽ��Ľ���
	Line CrossPlaneLine(Plane plane1,Plane plane2);
	//�����ཻ�߽��㣻
	CvPoint3D32f CrossLinePoint(Line line1,Line line2);
	//�����ཻ�󽻵�
	CvPoint3D32f Equations::PlaneAndLine(Plane plane,Line line);
	//������ά�㵽ԭ��ľ��룻
	double Equations::Point3D_O(CvPoint3D32f point);
	//������ͼ�еĵ� ʹ��allLeftPoints����allCalRightPoints
	void CalculateRightPoint(Plane rightPlane);//CvPoint3D32f leftPoint,CvPoint3D32f &rightPoint
	//������ͼ��ƽ�淽��
	Plane CalRightPicEquation();
	// ͼ���϶�ά��ת��ά��
	void Point2DTo3D(CvPoint2D32f point,double f,CvPoint3D32f &point3d);
	//��ռ���������֮��ľ��룻
	float Point3D_Point3D(CvPoint3D32f point1,CvPoint3D32f point2);
	//������ϵ�µĵ�ת��������ϵ��
	CvPoint3D32f RightAxisToLeft(CvPoint3D32f point);
	//������ϵ�µĵ�ת��������ϵ��
	CvPoint3D32f Equations::LeftAxisToRight(CvPoint3D32f point);
	// ��ƥ���������ά����
	void CalRightPointDifY2(int dif);
	void XIANGSHIPoint2();
	void Cal3dPoints();
	//д���ļ�
	fstream fst;
	void SimpleFileWrite(int ne,int fa,double scale);
	//��ͬyֵ�����ҵ�
	void CalRightPointY(void);
	//ֱ�߼���С����
	CvPoint3D32f MinLineDis(Line line1, Line line2);
	//������
	CvPoint3D32f CalCrossLine(Line line1, Line line2);
	CvPoint3D32f CalCrossLine2(Line line1, Line line2);
	//Yֵ��ֵ��ƥ���
	void CalRightPointDifY(int dif);
	//��תƽ�ƾ�����ƥ���
	void CalRightPoint();
	//��������ά��,����Ϊ������Ӧ�㣬����Ӧ������Ľ��࣬���Ϊ��ά��
	CvPoint3D32f Cal3DpointsBy2Points(CvPoint2D32f point1,CvPoint2D32f point2,float f1,float f2);
	void XIANGSHIPoint();
	Mat disp; //�Ӳ�ͼ
	void InitMat();
};


