#include "Equations.h"


Equations::Equations()
{

}

Equations::~Equations(void)
{

}

Plane Equations::CreatePlaneEquationGeneral(double A, double B, double C, double D)
{
	Plane plane;
	plane.setAA(A);
	plane.setBB(B);
	plane.setCC(C);
	plane.setDD(D);
	return plane;
}

Line Equations::CreateLineEquation(CvPoint3D64f point1, CvPoint3D64f point2)
{
	Line line;
	line.setA(point1.x - point2.y);
	line.setB(point1.z - point2.z);
	line.setC(point1.z - point2.z);
	line.setPoint(point1);
	return line;
}

/*
�ҵ����ƽ��ķ�������
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
Plane CreateLaserPlaneEquation(Line Line1, CvPoint3D64f Line2)
{
	Plane plane;
	return plane;
}


Plane Equations::CreatePlaneEquation(CvPoint3D64f point1, CvPoint3D64f point2, CvPoint3D64f point3)
{
	Plane plane;
	double A, B, C;
	double sum;

	A = (point2.y - point1.y)*(point3.z - point1.z) - (point3.y - point1.y)*(point2.z - point1.z);
	B = (point2.z - point1.z)*(point3.x - point1.x) - (point3.z - point1.z)*(point2.x - point1.x);
	C = (point2.x - point1.x)*(point3.y - point1.y) - (point3.x - point1.x)*(point2.y - point1.y);

	//Vec3f normalLine = getPlaneNormal(point1, point2, point3);
	//A = normalLine[1];
	//B = normalLine[2];
	//C = normalLine[3];

	sum = A*A + B*B + C*C;
	sum = sqrtf(sum);

	plane.setA(A / sum);
	plane.setB(B / sum);
	plane.setC(C / sum);

	plane.setPoint(point2);
	return plane;
}

Line Equations::CrossPlaneLine(Plane plane1, Plane plane2)
{
	Line line;
	CvPoint3D64f point;
	double A1, A2, B1, B2, C1, C2, D1, D2, x1, x2, y1, y2, z1, z2;
	A1 = plane1.getA();
	A2 = plane2.getA();
	B1 = plane1.getB();
	B2 = plane2.getB();
	C1 = plane1.getC();
	C2 = plane2.getC();
	x1 = plane1.getPoint().x;  //����������бʽ
	x2 = plane2.getPoint().x;
	y1 = plane1.getPoint().y;
	y2 = plane2.getPoint().y;
	z1 = plane1.getPoint().z;
	z2 = plane2.getPoint().z;

	double  M = A1*x1 + B1*y1 + C1*z1 - A1;
	double  N = A2*x2 + B2*y2 + C2*z2 - A2;
	
	point.x = 1;
	point.z = (B2*M - B1*N) / (B2*C1 - B1*C2);
	point.y = (M - C1*point.z) / B1;

	line.setA(B1*C2 - B2*C1);  //�������ֱཻ�ߵķ���
	line.setB(A1*C2 - A2*C1);
	line.setC(A1*B2 - A2*B1);
	line.setPoint(point);

	return line;
}

Vec3f Equations::getPlaneNormal(CvPoint3D64f p1, CvPoint3D64f p2, CvPoint3D64f p3)
{
	double A, B, C;
	A = ((p2.y - p1.y)*(p3.z - p1.z) - (p2.z - p1.z)*(p3.y - p1.y));
	B = ((p2.z - p1.z)*(p3.x - p1.x) - (p2.x - p1.x)*(p3.z - p1.z));
	C = ((p2.x - p1.x)*(p3.y - p1.y) - (p2.y - p1.y)*(p3.x - p1.x));

	return Vec3f(A, B, C);
}
