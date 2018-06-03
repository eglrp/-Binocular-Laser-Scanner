//#include "stdAfx.h"
#include "WeightCenters.h"
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

WeightCenters::WeightCenters(void)
{
	th=127;//��ʼ��ֵ����Ϊ250
	his=0.5;//ֱ��ͼ��ֵ����Ϊ0.5
	MaxV = 250;//�������ֵ250
	Minr = 5.0;//��С�뾶ֵ
	Maxr = 200.0;//���뾶ֵ
	Divr = 100.f;//
	lowthred =200;
	highthred = 200;
	image=NULL;
	gray=NULL;
}
WeightCenters::~WeightCenters(void)
{
}
//����
void WeightCenters::DrawPoint(IplImage*image,vector<CvPoint2D32f> allPoints)
{
	for(int i=0;i<allPoints.size();i++){	
		cvCircle(image,cvPoint((int)allPoints[i].x,allPoints[i].y),1,CV_RGB(0,255,0),10);
	}
}
//���ķ����ҶȻ�ͼ����ͼ����ǿF��ϵͳ�궨ģ�飬������ȡ�����룬ͼ�����ݣ������飩
void WeightCenters::WeightCenterMethod(IplImage  *pic,vector<CvPoint2D32f> &allPoints){
	allPoints.clear();
	IplImage *gray=cvCreateImage(cvGetSize(pic),IPL_DEPTH_8U,1);
	cvZero(gray);
	//IplImage* dstGrayImage = cvCreateImage(cvGetSize(pic), IPL_DEPTH_8U, 1);
	//cvZero(dstGrayImage);
	//��߱궨��ת��Ϊ�Ҷ�ͼ
	cvCvtColor(pic,gray,CV_BGR2GRAY);
	
	//��������ȡ
	//cvCopy(pic,gray);
	
	//cvShowImage("gray",gray);
	//ImageStretchByHistogram(gray,dstGrayImage);
	//cvShowImage("dstGrayImage",dstGrayImage); 

	int height=pic->height;
	int width=pic->width;
	int data;
	int LeftSum=0;
	int RightSum=0;
	float Avg;
	for(int i=0;i<height; i++)    //�����ꣻF��������
	{
		LeftSum = 0;
		RightSum = 0;
		int tempthre=0;//��ʱ��ֵ
		bool flag=true;int left=0;int right=0;

		for(int j=0;j<width; j++)	  //����һ��ͼƬ��
		{
			data=(uchar)gray->imageData[i*gray->widthStep+j];
			if(data>th)  //F���õ����ֵ��Ҫ����
			{
				if(flag){
					left=j; //��j��������ֵ��ֵ��left��
					flag=false; 
				}
				LeftSum = LeftSum+data*(j+1);
				RightSum = RightSum+data;
				right=j;	//j��ֵright ��left��right��ʾ��ʲô?

			}				
		}
		//printf("%d  %d\n",down,up);
		flag=true;
		if(RightSum>0)//������Ȳ�����10������&&((right-left)<400)
		{
			Avg = (float)LeftSum/RightSum;	
			//show->imageData[(int)Avg*gray->widthStep+j]=255;
			allPoints.push_back(cvPoint2D32f(Avg,i));
		}
	} 
	//DrawPoint(pic,allPoints);
	cvReleaseImage(&gray);
	//cvReleaseImage(&dstGrayImage); //�ͷ�ͼ��
}
//ͼ����ǿ����
int WeightCenters::ImageStretchByHistogram(IplImage *src,IplImage *dst)
{
	//p[]���ͼ������Ҷȼ��ĳ��ָ��ʣ�
	//p1[]��Ÿ����Ҷȼ�֮ǰ�ĸ��ʺͣ�����ֱ��ͼ�任��
	//num[]���ͼ������Ҷȼ����ֵĴ���;

	assert(src->width==dst->width);

	float p[256],p1[256],num[256];
	//�����������
	memset(p,0,sizeof(p));
	memset(p1,0,sizeof(p1));
	memset(num,0,sizeof(num));

	int height=src->height;
	int width=src->width;
	long wMulh = height * width;

	//����ͼ������Ҷȼ����ֵĴ���
	// to do use openmp
	for(int x=0;x<width;x++)
	{
		for(int y=0;y<height;y++)
		{
			uchar v=((uchar*)(src->imageData + src->widthStep*y))[x];
			num[v]++;
		}
	}

	//����ͼ������Ҷȼ��ĳ��ָ���
	for(int i=0;i<256;i++)
	{
		p[i]=num[i]/wMulh;
	}

	//���Ÿ����Ҷȼ�֮ǰ�ĸ��ʺ�
	for(int i=0;i<256;i++)
	{
		for(int k=0;k<=i;k++)
			p1[i]+=p[k];
	}

	//ֱ��ͼ�任
	// to do use openmp
	for(int x=0;x<width;x++)
	{
		for(int y=0;y<height;y++)
		{
			uchar v=((uchar*)(src->imageData + src->widthStep*y))[x];
			((uchar*)(dst->imageData + dst->widthStep*y))[x]= p1[v]*255+this->his;            
		}
	}

	return 0;

}
//----------------------------------------ͼ��ɼ�------------------------------------
void WeightCenters::setImage(IplImage *simg)   //��ͼ��ת���ɵ�ͨ��ͼ�������棻
{
	this->image = simg;
	centerx = image->width/2*1.f - 0.5f;
	centery = image->height/2*1.f - 0.5f;
	//zf = 531*image->width/640*1.f;
	if(gray!=NULL)
	{
		cvReleaseImage(&gray);
		if(cannyImage!=NULL)
		{
			cvReleaseImage(&cannyImage);		
		}
	}
	gray = cvCreateImage(cvGetSize(simg), 8, 1);
	cvCvtColor(simg, gray, CV_BGR2GRAY);
	cannyImage=cvCloneImage(gray);
	cvThreshold(cannyImage,cannyImage,50,255,CV_THRESH_BINARY);
	//cvShowImage("canny",cannyImage);
}
////ֱ�Ӵ�canny����ͼ�ж�ȡֵ��
void WeightCenters::FindSrcPoint(map<float,CvPoint2D32f> &sp)
{
	sp.clear();
	CvMemStorage *storage = cvCreateMemStorage(0);
	CvSeq *seq = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq),sizeof(CvPoint),storage);
	cvFindContours(cannyImage, storage, &seq, sizeof(CvContour), CV_RETR_EXTERNAL);
	//cvDrawContours(cannyImage,seq, CV_RGB(0,0,255),CV_RGB(255,0,0),2,2,8,cvPoint(0,0));
	//cvShowImage("showImage",cannyImage);
	for(; seq; seq = seq->h_next)
	{
		int count = seq->total;
		//����ת��Ϊ����㣻
		CvMat *points_f = cvCreateMat(1, count, CV_32FC2);
		cvCvtSeqToArray(seq, points_f->data.ptr, CV_WHOLE_SEQ);
		CvMat points_i = cvMat(1, count, CV_32SC2, points_f->data.ptr);
		cvConvert(&points_i, points_f);

		//��С�ƽ���������
		double length = cvArcLength(points_f,CV_WHOLE_SEQ,0);  //��С�ƽ�Բ���ĳ��ȣ�
		double area = cvContourArea(points_f);
		area = fabs(area);                                    //����õ��������

		double compare = (length*length)/area;  //����ܳ�ƽ��������ı�ֵ   Լ����4*PI

		double ml = length*640/cannyImage->width;
		if(compare > Maxr || compare < Minr || ml<10 || ml>200)  //��ֵ���������������Ϊ��ǰ��������Բ��״
			continue;
		//��ֵ��ʼ��ԭʼֵ��
		float R = (float)(2*area/length);//����ƽ��������İ뾶��
		CvPoint2D32f centp;
		float radium;
		cvMinEnclosingCircle(points_f, &centp, &radium);

		if(radium < R+Divr*cannyImage->width/640 && radium > R)
		{
			if(IsCircleCenterPixel(cvPoint((int)centp.x, (int)centp.y)))
			{
				sp.insert(make_pair(centp.x, centp));
				//cvCircle(image,cvPoint((int)centp.x, (int)centp.y),1,CV_RGB(255,0,0),30);
			}
		}
		cvReleaseMat(&points_f);
	}
	cvReleaseMemStorage(&storage);
}
bool WeightCenters::IsCircleCenterPixel(const CvPoint &p)
{
	int D  = 255;
	D = (uchar)gray->imageData[p.y*gray->widthStep+p.x];
	if(D<this->MaxV)//if(D>this->MaxV)
		return true;
	else
		return false;
}
void WeightCenters::CannyContourDeal(){
	cvCanny(cannyImage, cannyImage, this->lowthred, this->highthred);
}
//����Щ��ֿ����õ��㣻
bool WeightCenters::SortFourPoints(map<float, CvPoint2D32f> pointsmap,vector<CvPoint2D32f>&points){
	//�������ĸ���֮��ľ��룬����ϴ��������Ϊ��Ե�
	points.clear();
	CvPoint2D32f tempPoint;
	map<float, CvPoint2D32f>::iterator it;
	//����x�Ĵ�Сȡ��Ԫ��
	if(pointsmap.size()==4){//-------------
		int i=0;
		for(it=pointsmap.begin();it!=pointsmap.end();it++){
			tempPoint=it->second;
			points.push_back(tempPoint);
			cvCircle(image,cvPoint((int)tempPoint.x,(int)tempPoint.y),1,CV_RGB(255,i*60,0),30);
			i++;
		}
		cvCircle(image,cvPoint((int)image->width/2,(int)image->height/2),1,CV_RGB(0,0,255),30);
		return true;
	}
	else{
		//cout<<"The number of  points is not 4!"<<endl;
		return false;
	}
}
//��ͼ�еı궨��2d��ϵͳ�궨ģ�飩
bool WeightCenters::findPoints(IplImage*left,IplImage*right){
	//�Ա����������ȡ
	bool isfind=false;
	//��ͼ
	setImage(left);
	CannyContourDeal();   //Canny���㣻
	//cvShowImage("cannyLeft",cannyImage);
	FindSrcPoint(leftCenters2dmap);
	if(SortFourPoints(leftCenters2dmap,leftCenters2d)){
		isfind=true;
	}
	//��ͼ
	setImage(right);
	CannyContourDeal();   //Canny���㣻
	//cvShowImage("cannyRight",cannyImage);
	FindSrcPoint(rightCenters2dmap);

	if(SortFourPoints(rightCenters2dmap,rightCenters2d)){
		if(isfind){
			return true;
		}
		else{
			return false;
		}
	}else{
		return false;
	}
}
//��ͼ���������Ĺ�ߣ���������ͼ�����ݣ�
void WeightCenters::findMaxPoints(IplImage*left,IplImage*right){
	IplImage* leftImage=cvCreateImage(cvSize(left->width,left->height),IPL_DEPTH_8U,1);
	//cvZero()�����þ����ֵ��Ϊ0���г�ʼ�������ã�����˵����
	cvZero(leftImage);
	//��ͼ
	setImage(left);

	int D  = 255;
	//F�������е����ص� �Ҷ�ͼ����0�Ǻڣ�255�ǰ�
	for(int y=0;y<gray->height;y++){
		for(int x=0;x<gray->width;x++){
			D = (uchar)gray->imageData[y*gray->widthStep+x];
			//�õ������ֵ������ֵ
			if(D>this->MaxV){
				leftImage->imageData[y*leftImage->widthStep+x]=255;    //F���ûҶ�ͼƬ�иõ������Ϊ255����ɫ��
			}
		}
	}
	//cvShowImage("left",leftImage);
	cvReleaseImage(&leftImage);
	
	//��ͼ,����ͼ�Ĳ���һ��
	IplImage* rightImage=cvCreateImage(cvSize(left->width,left->height),IPL_DEPTH_8U,1);
	cvZero(rightImage);
	setImage(right);
	D  = 255;
	for(int y=0;y<gray->height;y++){
		for(int x=0;x<gray->width;x++){
			D = (uchar)gray->imageData[y*gray->widthStep+x];
			if(D>this->MaxV){
				rightImage->imageData[y*rightImage->widthStep+x]=255;
				//cvCircle(rightImage,cvPoint(x,y),1,CV_RGB(255,0,0),1);
			}
		}
	}
	//cvShowImage("right",rightImage);
	cvReleaseImage(&rightImage);
}