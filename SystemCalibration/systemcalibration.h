#ifndef SYSTEMCALIBRATION_H
#define SYSTEMCALIBRATION_H
# pragma warning (disable:4819)

#include <QtWidgets/QMainWindow>
#include <iostream>
#include "qstring.h"
#include "qvector.h"
#include "qimage.h"

#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\calib3d.hpp>
#include "cvut.h"
#include "ui_systemcalibration.h"

#include"Equations.h"

using namespace std;
using namespace cv;
using namespace cvut;

class SystemCalibration : public QMainWindow
{
	Q_OBJECT

public:
	SystemCalibration(QWidget *parent = 0);
	~SystemCalibration();

	void	 OnBnClickedButtonNext();
	void	 OnBnClickedButtonPre();
	void	 OpenCalibrationPicture();
	void	 BeginCalibration();
	void	 BeginCalibration2();
	void	 BeginSystemCalib();
	void	 BackToHome();
	void	 GetFeaturePoint();
	void	 GetEquationCoeff1();
	void	 GetEquationCoeff2();

	void completeAndEvaluateAndSave(int & chessRow, int& chessColumn);
	
	void loadAndShowImage           (int index, const QString &cfileFullName);												  //������ʾͼƬ
	void readChessboardPara         (float& cellHeight, float &cellWidth, int &chessRow, int &chessColumn);
	
	bool connerDetection			(CvSize& image_size, const CvSize&board_size, cvut::Seq<CvPoint2D32f>& image_points_seq);  //�ǵ���
	void cameraCalibration			(CvSize& image_size, const CvSize& board_size,											   //����궨
									 cvut::Seq<CvPoint2D32f>& image_points_seq, Matrix<double>& intrinsic_matrix,
									 Matrix<double>& distortion_coeffs,        Matrix<double>& rotation_vectors,
									 Matrix<double>& translation_vectors,      Matrix<int>& point_counts,
									 Matrix<double>& image_points,     Matrix<double>& object_points);
	void evaluateCalibResult        (Matrix<int>& point_counts,        Matrix<double>& objects_points,
									 Matrix<double>& rotation_vectors, Matrix<double>& translation_vectors,
									 Matrix<double>& intrinsic_matrix, Matrix<double>& distortion_coeffs,
									 Matrix<double>& image_points);															//���۱궨���
	void saveCalibrationResult	    (Matrix<double>& rotation_vectors, Matrix<double>& translation_vectors, 
									 Matrix<double>& intrinsic_matrix, Matrix<double>& distortion_coeffs);
	void saveCalibrationResult2		(Matrix<double>& rotation_vectors, Matrix<double>& translation_vectors,
									 Matrix<double>& intrinsic_matrix, Matrix<double>& distortion_coeffs);
	void showInternalAndDistortion  (Matrix<double>& intrinsic_matrix, Matrix<double>& distortion_coeffs,
								     QLabel* distortionLabelShow,      QLabel*internalLabelShow);						    //��ʾ���
	void showRotationAndTranslation (Matrix<double>& intrinsic_matrix, Matrix<double>& distortion_coeffs,
								     QLabel* translationLabelShow, QLabel*rotationLabelShow);

	vector<CvPoint2D32f>	advancedThreWeight(IplImage *imageView);
	int  adaptThreshold		(IplImage *pic, int width, int j);
	void drawPoint			(IplImage* image, vector<CvPoint2D32f> allpoints);
	//void greyCenter(IplImage* image, vector<CvPoint2D32f>& allPoints);
	void getLaserPlane		(Plane& plane);
	void getLaserFitPlane	(Plane& plane);
	void showLine			(Line &line);
	void showLine2			(Line &line);
	void cvFitPlane			(const CvMat* points, double *plane);

	QImage cIplImgToQImg(IplImage* cvimage);			 //IplImageתQImage
	void   testShow33   (Matrix<double>& steroR, Matrix<double>& steroT, QLabel* translationLabelShow, QLabel*rotationLabelShow);
	void   testShow31   (Matrix<double>& steroR, Matrix<double>& steroT, QLabel* translationLabelShow, QLabel*rotationLabelShow);
private:
	Ui::SystemCalibrationClass ui;

	CvSize      image_size;           //ͼ���С
	QStringList filePathList;		  //�ļ�·���б�
	QStringList cornerFilePathList;   //�ǵ�·���б�
	QStringList imageNameList;        //����ͼ������
	string cornerDir;			      //�ǵ㱣��Ŀ¼
	int    imageNum;				  //����ͼƬ����
	int    picIndex;	              //��ǰ��ʾ���ǵڼ���ͼ��
	bool   isCorner;                    //��ǰ��ʾ��ͼ���Ƿ�Ϊ�ǵ�ͼ��
	bool   isBeginCalibration2;
	bool   isBeginCalibration;          //�Ƿ�ʼ�궨���1
	bool   isBeginSystemCalib;          //�Ƿ�ϵͳ�궨
	bool   isClibrationCamer1;          //���1�궨�ɹ�
	bool   isClibrationCamer2;          //���2�궨�ɹ�

	float  cellHeight;
	float  cellWidth;
	int    chessRow;
	int    chessColumn;

	Matrix<double> *intrinsic_matrix2 ;			  //������ڲ�������
	Matrix<double> *intrinsic_matrix ; 

	vector<CvPoint2D32f> allFeaturePoints;
	Equations eqations;   
	int featurePointNum = 1;					 //���������
	int  isPlaneOk = 0;
	int  isPlaneOk1 = 0;
	bool flag1 = true;
	bool flag2 = true;

	stringstream ss;
	vector<string> vecErr;
	string data = "";
};


#endif // SYSTEMCALIBRATION_H
