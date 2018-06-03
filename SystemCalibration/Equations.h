#pragma once
#include "Line.h"
#include "Plane.h"

/*
	����˼·1��
	1.���㼤��Ͷ��ı궨ͼ��
	2.����Ҫȷ���߶εĵ�ת�����������ϵ
	3.ͨ������ȷ��һ��ֱ�߷ֱ�õ�����λ�߶εķ���
	4.����λ�߶�����ֱ�߷�����ü���ƽ��
	5.���鼤��ƽ���ཻ�����������ת���ߵķ���
*/


/*
	����˼·2���ҵ����ƽ��ķ�������
	��һ��ֱ��Ϊֱ��a����һ��Ϊb
	���裺
	��һ������ֱ��a��ȡһ��A��b��ȡһ��B���õ�����AB
	�ڶ�����ͨ��ֱ��a������b���ķ��̵õ�a�ķ�������������t
	����������������t������AB�Ĳ�ˣ��õ�ƽ�淨����n��
	��A�㣨��B�㣩����ͷ�����n�õ�ƽ��ĵ㷨ʽ���ʽ��

	�㷨ʽ������
	��A���� (x0,y0,z0)�������� n=(r,s,t)��
	�� �㷨ʽΪ r(x-x0)+s(y-y0)+t(z-z0)=0
*/

class Equations
{
public:
	Equations();
	~Equations(void);

	//��ƽ��һ��ʽ���� 
	Plane CreatePlaneEquationGeneral(double A, double B, double C, double D);

	//�ռ��е�2�����ֱ��
	Line CreateLineEquation(CvPoint3D64f point1, CvPoint3D64f point2);
	//�ռ��е�2�߼��㼤��ƽ��
	//Plane CreateLaserPlaneEquation(Line Line1, CvPoint3D32f Line2);	
	//2. �����ཻƽ��Ľ���
	Line CrossPlaneLine(Plane plane1, Plane plane2);

	//��֪�ռ��е�3������㷨��
	Vec3f getPlaneNormal(CvPoint3D64f p1, CvPoint3D64f p2, CvPoint3D64f p3);
	//1. ��֪�ռ��е�3�������ƽ�� point1,point2,point3 Ϊ����������ǹ��ߵĵ㣻 
	Plane CreatePlaneEquation(CvPoint3D64f point1, CvPoint3D64f point2, CvPoint3D64f point3);
};