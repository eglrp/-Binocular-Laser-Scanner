//�߽ṹ�����������ȡ������Ӧ��ֵ��
#pragma once
//#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "highgui.h"
#include <cv.h>
#include <cxcore.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
using namespace cv;
using namespace std;
class AdeptThreshold
{
public:
	AdeptThreshold(void);
	~AdeptThreshold(void);
	//����
	void DrawPoint(IplImage*image,vector<CvPoint2D32f> allPoints);
	//�ߴ����
	IplImage* ResizeImage(IplImage* img); 
	//����ÿһ�е�����Ӧ��ֵ   
	int adaptThreshold(IplImage* pic,int height,int j);
	int AdeptThreshold::adaptThresholdVertical(IplImage* pic,int width,int j, int& maxDataPos);
	//ȥ���ٹ���
	vector<CvPoint2D32f> NoiseRemove(IplImage* image,vector<CvPoint2D32f> allPoints,vector<CvPoint2D32f> allPoints2);
	//����Ӧ��ֵ���Ľ����ķ�
	vector<CvPoint2D32f> advancedAdeThreWeight(IplImage  *pic);//����
	vector<CvPoint2D32f> AdeptThreshold::advancedAdeThreWeightVertical(IplImage  *pic);//����
	// ͼ���ֵ
	void ImageInterpolation(IplImage* image, IplImage  *background,IplImage* imageInter);
	// �����ȷ�
	vector<CvPoint2D32f> HighLight(IplImage  *pic);
	//����Ӧ��ֵ��������Ϊ�Ҷ�ͼ��
	vector<CvPoint2D32f> AdeptThreshold::advancedAdeThreWeightGray(IplImage *pic,IplImage *gray);
};

