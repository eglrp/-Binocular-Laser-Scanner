#pragma once
//#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "highgui.h"
#include<map>
#include <cv.h>
#include <cxcore.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
using namespace cv;
using namespace std;
class WeightCenters
{
public:
	WeightCenters(void);
	~WeightCenters(void);
	//����
	void DrawPoint(IplImage*image,vector<CvPoint2D32f> allPoints);
	//���ķ�
	int th;//��ֵ
	float his;//ֱ��ͼ���⻯��ֵ
	//���ķ�����ȡ��������
	void WeightCenterMethod(IplImage  *pic,vector<CvPoint2D32f> &allPoints);
	//ͼ����ǿ
	int ImageStretchByHistogram(IplImage *src,IplImage *dst);
	//��ȡ�궨��
	//canny���ӵĸ���ֵ�͵���ֵ
	int lowthred;
	int highthred; 
	
	IplImage *image;//����Դͼ��
	IplImage *gray;//ת��Ϊ�Ҷ�ͼ��
	IplImage *cannyImage;//cannyͼ��F��ϣ������߶Ծ����Ե�������Ե�ͬʱ���������������ķ������Ǻõı�Ե��ȡ������
	map<float,CvPoint2D32f> leftCenters2dmap;  //�洢��ȡ���ĵ㣻
	map<float,CvPoint2D32f> rightCenters2dmap;  //�洢��ȡ���ĵ㣻
	vector<CvPoint2D32f> leftCenters2d;//������ͼ�����ҵĶ�ά��
	vector<CvPoint2D32f> rightCenters2d;//������ͼ�����ҵĶ�ά��
	int MaxV;    // ȷ�����ĵ������ֵ��ֵ��
	double Minr;  //�жϱ�ֵ��ȷ����Χ����Сֵ��
	double Maxr;  //�жϱ�ֵ�ķ�Χ�����ֵ��
	float Divr;    //�뾶ƫ��ֵ��
	float centerx;//��������
	float centery;
	
	void setImage(IplImage *simg);   //��ͼ��ת���ɵ�ͨ��ͼ�������棻
	//ͨ������ͼ���о���Բ����������ȡ��ʽ������ȡͼ������ĵ�
	//ֱ�Ӵ�canny����ͼ�ж�ȡֵ��
	void FindSrcPoint(map<float,CvPoint2D32f> &sp);
	//canny����
	void CannyContourDeal();
	//�ж���ȡ���Ƿ�Ϊ�ڵ㣻
	bool IsCircleCenterPixel(const CvPoint &p);
	//����Щ��ֿ����õ��㣻
	bool SortFourPoints(map<float, CvPoint2D32f> pointsmap,vector<CvPoint2D32f>&points);
	//��ͼ�еı궨��2d
	bool findPoints(IplImage*left,IplImage*right);
	//�����Ĺ��
	void findMaxPoints(IplImage*left,IplImage*right);
};

