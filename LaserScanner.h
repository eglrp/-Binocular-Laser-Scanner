#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QTimer>  
#include <time.h> 
#include "ui_BinocularScanner.h"
#include "CameraCapture.h"
#include "Equations.h"
#include "AdeptThreshold.h"
#include "Calibration.h"
#include "Transforms.h"
#include<iostream>
#include<fstream>
#include <string>
#include <vector>
#include"GLMainWidget.h"
#include <QFileDialog>
using namespace std;
namespace Ui {
	class BinocularScannerClass;
}
class LaserScanner : public QMainWindow
{
	Q_OBJECT
private:

	/* constants */
	static const int OBJECT_THRESH = 100;
	static const int LASER_THRESH = 250;
	static const int MAX_WIDTH = 1280;
	static const int MAX_HEIGHT = 1024;
public:
	LaserScanner(QWidget *parent = Q_NULLPTR);
	void OnTimer();

protected:
	void OnInitialize();  //��ʼ��
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent * event);
	void Correction(); //����У��
	void createWindow(char* src, QWidget * picSize);  //��ʾcv::Mat�ķ���
	vector<CvPoint2D32f> getLaserCenter(Mat src,IplImage* transpic,int type);
	void view3d(vector<CvPoint3D32f> allPoints3d);
	void Handle(Mat picL, Mat picR);
	void StereoCalibration();
public slots:

	void OpenCamera();
	void Test();
	void StartScan();
	void SaveImage();
	void StartCalibrationL();
	void StartCalibrationR();
	void SwapLR();
	void ViewPoints();
	void LoadParameter();
private:
	Ui::BinocularScannerClass ui;
	void on_Mouse(int event, int x, int y);
	static void onMouse(int event, int x, int y, int, void* userdata);
	static const int HOUGH_THRESH = 70;			//100
	static const int HOUGH_PARAM1 = 1;			//700
	static const int HOUGH_PARAM2 = 600;		//600
	static const int CANNY_LOWTHRESH = 80;
	static const int CANNY_HIGHTHRESH = 160;
	int tempthre ; //��ֵ
	bool bConnected; //���Ӱ�ť��־
	bool isStartScan; //�Ƿ�ʼɨ��
	bool isSwap; //�Ƿ����ҽ���
	bool isCorrection;//�Ƿ�У��
	bool isLoadParameter; //�Ƿ���ز���
	bool chooseL; //�궨��
	bool chooseR; //�궨��
	bool isCalibratedL;
	bool isCalibratedR;
	GLMainWidget*glWidget;
	QTimer theTimer; //��ʱ��
	CameraCapture*  leftCapture = new CameraCapture;
	CameraCapture*  rightCapture = new CameraCapture;
	Calibration  leftCalibration;
	Calibration  rightCalibration;
	Mat leftMat, rightMat;
	Mat dstL, dstR;
	IplImage *calImage;
	//α��ɫ��ʾ
	Mat rgbleftMat, rgbrightMat;
	IplImage * lplImg_left;  // opencv���ͼƬ����ָ��
	IplImage * lplImg_right;  // opencv���ͼƬ����ָ��
	IplImage * lplImg_disp;  // opencv���ͼƬ����ָ��
	Equations allEquation;
	Transforms *trans= new Transforms();     //ͼ����
	AdeptThreshold adeptThreshold;//����Ӧ��ֵ����ȡ��������
	vector<CvPoint2D32f> allPointsleft;//��ͼ�����м������ĵ�
	vector<CvPoint2D32f> allPointsright;//��ͼ�����м������ĵ�
	vector<CvPoint2D32f> allPointsleft2;//��ͼ�����м������ĵ�
	vector<CvPoint2D32f> allPointsright2;//��ͼ�����м������ĵ�
	vector<CvPoint2D32f> allPointsleft3;//��ͼ�����м������ĵ�
	vector<CvPoint2D32f> allPointsright3;//��ͼ�����м������ĵ�
	vector<CvPoint2D32f> matchPoints;
	vector<CvPoint3D32f> allPointsdisp;


	//д���ļ�
	fstream laserL;
	fstream laserR;
	fstream testClouds;
	fstream laserMatchDIS;
	fstream disp;
	double time_Start, time_End;
	int saveTimes ;
	int xyzMinZ, xyzMaxZ, xyzScale;
	char buffer_left[50];
	char buffer_right[50];
	Size imageSize = Size(MAX_WIDTH, MAX_HEIGHT);
	//Mat rgbImageL, grayImageL;
	//Mat rgbImageR, grayImageR;
	Mat rectifyImageL, rectifyImageR;
	Mat rgbRectifyImageL, rgbRectifyImageR;
	Rect validROIL;//ͼ��У��֮�󣬻��ͼ����вü��������validROI����ָ�ü�֮�������  
	Rect validROIR;
	Mat mapLx, mapLy, mapRx, mapRy;     //ӳ���  
	Mat Rl, Rr, Pl, Pr, Q;              //У����ת����R��ͶӰ����P ��ͶӰ����Q
	Mat xyz;              //��ά����
						  /*
						  ���ȱ궨�õ�����Ĳ���
						  fx 0 cx
						  0 fy cy
						  0 0  1
						  */
	Mat cameraMatrixL = (Mat_<double>(3, 3) << 
		1198.2               ,  0                  , 632.1480,
		0                        , 1194.5         , 487.8904,
		0                        , 0                  , 1                 );


	Mat cameraMatrixR = (Mat_<double>(3, 3) << 
		1196.2, 0                         , 649.8460,
		0                      , 1192.5   , 486.7133,
		0                      , 0                      , 1                  );
	Mat distCoeffL = (Mat_<double>(5, 1) << -0.1853, -0.1255, -0.0027, -0.0015, 0.7990);
	Mat distCoeffR = (Mat_<double>(5, 1) << -0.1731, -0.2295, -0.0012, -0.0018, 1.6306);

	Mat T = (Mat_<double>(3, 1) << -138.5611, -0.4780, 2.1508);//Tƽ������
	Mat rec = (Mat_<double>(3, 1) << -0.01001,0.04790,-0.00219);//rec��ת����
	Mat R = (Mat_<double>(3, 3) <<
		0.9998, 0.0015, 0.0216,
		-0.0017, 0.9999, 0.0100,
		-0.0216, -0.0100, 0.9997);
};
