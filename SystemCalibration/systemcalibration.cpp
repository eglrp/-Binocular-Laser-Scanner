#include "systemcalibration.h"
#include "string.h"
#include "qstring.h"
#include "qfiledialog.h"
#include "qlistview.h"
#include "qtreeview.h"
#include "qicon.h"
#include "qpixmap.h"
#include "qstringlist.h"
#include "qmessagebox.h"
#include "qtextcodec.h"
#include "qlabel.h"

Matrix<double> steroR(3, 3);
Matrix<double> steroT(3, 1);

SystemCalibration::SystemCalibration(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	isCorner = false;   
	isBeginCalibration2 = false;
	isBeginCalibration = false;
	isClibrationCamer1 = false;
	isClibrationCamer2 = false;
	//isHandleCamera2 = false;
	isBeginSystemCalib = false;
	picIndex = -1;		
	cornerDir = "";		

	ui.pushButton_NextPicture->setEnabled(false);
	ui.pushButton_PrePicture->setEnabled(false);
	ui.pushButton_BeginCalibration->setEnabled(false);
	ui.pushButton_BeginCalibration_2->setEnabled(false);
	ui.pushButton_CalibrationResult->setEnabled(false);
	ui.stackedWidget->setCurrentIndex(2);

	//����ͼƬ
	connect(ui.pushButton_LoadPicture, &QPushButton::clicked, this, &SystemCalibration::OpenCalibrationPicture);
	connect(ui .action_OpenCalibrationPicture, &QAction::triggered, this, &SystemCalibration::OpenCalibrationPicture);
	
	//ͼƬ�л�
	connect(ui.pushButton_NextPicture, &QPushButton::clicked,this,&SystemCalibration::OnBnClickedButtonNext);
	connect(ui.pushButton_PrePicture, &QPushButton::clicked, this, &SystemCalibration::OnBnClickedButtonPre);
	
	//��ʼ����궨
	connect(ui.pushButton_BeginCalibration,&QPushButton::clicked,this,&SystemCalibration::BeginCalibration);
	connect(ui.pushButton_BeginCalibration_2, &QPushButton::clicked, this, &SystemCalibration::BeginCalibration2);

	//��ʼϵͳ�궨
	connect(ui.pushButton_CalibrationResult,&QPushButton::clicked,this,&SystemCalibration::BeginSystemCalib);

	//��ȡ�������ģ��õ�������
	connect(ui.pushButton_laserCenter1, &QPushButton::clicked, this, &SystemCalibration::GetFeaturePoint);
	connect(ui.pushButton_coefficient1, &QPushButton::clicked, this, &SystemCalibration::GetEquationCoeff1);
	connect(ui.pushButton_coefficient2, &QPushButton::clicked, this, &SystemCalibration::GetEquationCoeff2);

	connect(ui.pushButton_BackToHome, &QPushButton::clicked, this, &SystemCalibration::BackToHome);
	setWindowIcon(QIcon(":/SystemCalibration/testPic/sysicon.jpg"));
	resize(this->width(),this->height());
}

SystemCalibration::~SystemCalibration()
{
	filePathList.clear();
	cornerFilePathList.clear();
	imageNameList.clear();

	cvReleaseMat(&(intrinsic_matrix->cvmat));
	cvReleaseMat(&(intrinsic_matrix2->cvmat));
}

void SystemCalibration::OpenCalibrationPicture()
{
#if 0 //�򿪵���ͼƬ�ļ�
	
	/*QString r_fixedfilename,fixedfilename;

	r_fixedfilename = QFileDialog::getOpenFileName(this,QStringLiteral("�򿪱궨ͼƬ"), "./testPic/","source(*.jpg *.png *.bmp);;");

	QByteArray file_mid = r_fixedfilename.toLocal8Bit();
	fixedfilename = file_mid.data();

	if (!r_fixedfilename.isEmpty())
	{
		filePathArray.append(r_fixedfilename);
	}*/

#endif // 0
	filePathList.clear();

	//1.�򿪶��ͼƬ�ļ�
	QFileDialog* image_dlg = new QFileDialog(this);
	image_dlg->setOption(QFileDialog::DontUseNativeDialog, true);
	image_dlg->setDirectory("./testPic/");
	image_dlg->setBackgroundRole(QPalette::ColorRole::Light);
	image_dlg->setWindowTitle(QStringLiteral("�򿪴��궨ͼƬ"));
	image_dlg->setWindowIcon(QIcon(":/SystemCalibration/testPic/1.jpg"));

	//����������new�Ķ��󶼿�����Ϊ��ǰ���child���ҵ���ͨ��Qstring�������new�Ķ�����new�������ͬʱ�������ҵ�Qstring�� 
	QListView *l = image_dlg->findChild<QListView*>("listView");
	if (l) {
		l->setSelectionMode(QAbstractItemView::MultiSelection);
	}
	QTreeView *t = image_dlg->findChild<QTreeView*>();
	if (t) {
		t->setSelectionMode(QAbstractItemView::MultiSelection);
	}

	QStringList filters;
	filters << QString("Image source(*.jpg)") << QString("Image source(*.jpeg)") << QString("Image source(*.png)") << QString("Image source(*.bmp)");
	image_dlg->setNameFilters(filters);

	int nMode = image_dlg->exec();
	filePathList = image_dlg->selectedFiles();

	if (nMode)
	{
		imageNum = filePathList.size();
		if (imageNum > 0)
		{
			if (imageNum > 1)
				ui.pushButton_NextPicture->setEnabled(true);

			ui.pushButton_BeginCalibration->setEnabled(true);
			ui.pushButton_BeginCalibration_2->setEnabled(true);

			picIndex = 0;
			loadAndShowImage(picIndex, filePathList.at(picIndex));   //���ز���ʾͼƬ
		}
	}
	
}

//��ʼ���2�궨
void SystemCalibration::BeginCalibration2()
{
	isBeginCalibration2 = true;
	readChessboardPara(cellHeight, cellWidth, chessRow, chessColumn);

	completeAndEvaluateAndSave(chessRow, chessColumn);
}

//��ʼ���1�궨
void SystemCalibration::BeginCalibration()
{
	isBeginCalibration = true;
	//ui.stackedWidget->setCurrentIndex(1);

	readChessboardPara(cellHeight, cellWidth, chessRow, chessColumn);
	
	completeAndEvaluateAndSave(chessRow, chessColumn);
}

//��ɱ궨�ͽ�����ۡ��Լ�������ʾ���
void SystemCalibration::completeAndEvaluateAndSave(int & chessRow, int& chessColumn)
{
	if (imageNum > 0)
	{
		CvSize board_size = cvSize(chessRow - 1, chessColumn - 1);
		cvut::Seq<CvPoint2D32f> image_points_seq;  //�����⵽���нǵ�

		if (!connerDetection(image_size, board_size, image_points_seq))
		{
			QMessageBox::warning(NULL, QStringLiteral("����"), QStringLiteral("���̸�ǵ���ȡʧ�ܣ�"), Q_NULLPTR, Q_NULLPTR);
			return;
		}
		
		//���1
		intrinsic_matrix = new Matrix<double>(3, 3, 1);			 //������ڲ�������
		Matrix<double> distortion_coeffs(1, 4, 1);			 //�����4������ϵ�� k p
		Matrix<double> rotation_vectors(1, imageNum, 3);	 //ÿ��ͼ�����ת����
		Matrix<double> translation_vectors(1, imageNum, 3);  //ÿ��ͼ���ƽ������
		Matrix<double> image_points(1, image_points_seq.cvseq->total, 2); //������ȡ�����нǵ�
		Matrix<int> point_counts(1, imageNum, 1);                         //ÿ��ͼ���нǵ������
		Matrix<double> objects_points(1, board_size.width * board_size.height * imageNum, 3);  //��������ͼ��궨���ϵ���ά����

		//���2
		Matrix<double> rotation_vectors2(1, imageNum, 3);	  //ÿ��ͼ�����ת����
		Matrix<double> translation_vectors2(1, imageNum, 3);  //ÿ��ͼ���ƽ������
		intrinsic_matrix2 = new Matrix<double>(3, 3, 1);			  //������ڲ�������
		Matrix<double> distortion_coeffs2(1, 4, 1);			  //�����4������ϵ��
		Matrix<double> image_points2(1, image_points_seq.cvseq->total, 2); //������ȡ�����нǵ�
		Matrix<int> point_counts2(1, imageNum, 1);                         //ÿ��ͼ���нǵ������
		Matrix<double> objects_points2(1, board_size.width * board_size.height * imageNum, 3);  //��������ͼ��궨���ϵ���ά����

		/*
		ss.clear();
		ss << rotation_vectors.get_col(0);
		data = ss.str();
		ui.label_scrollArea->setText(data.c_str());*/

		if (!isBeginSystemCalib)
		{
			ui.stackedWidget->setCurrentIndex(1);
		}

		//����궨
		cameraCalibration(image_size, board_size, image_points_seq, *intrinsic_matrix, distortion_coeffs, rotation_vectors,
			translation_vectors, point_counts, image_points, objects_points);

		//���۱궨��� ������궨���
		evaluateCalibResult(point_counts, objects_points, rotation_vectors, 
			translation_vectors, *intrinsic_matrix, distortion_coeffs, image_points);

		if (ui.pushButton_BeginCalibration_2->isEnabled() && isBeginCalibration2)
		{
			cameraCalibration(image_size, board_size, image_points_seq, *intrinsic_matrix2, distortion_coeffs2, rotation_vectors2,
				translation_vectors2, point_counts2, image_points2, objects_points);

			evaluateCalibResult(point_counts2, objects_points2, rotation_vectors2,
				translation_vectors2, *intrinsic_matrix2, distortion_coeffs2, image_points2);
		}
		
 		if ( isClibrationCamer1 && isClibrationCamer2)
		{
			ui.pushButton_CalibrationResult->setEnabled(true);
			if (isBeginSystemCalib)
			{
				isBeginSystemCalib = false; 

				cvutMatrix::Matrix<double> E(3, 3); //��������������ͷ���λ�ù�ϵ��Essential Matrix
				cvutMatrix::Matrix<double> F(3, 3); //�Ȱ�����������ͷ���λ�ù�ϵ��Ҳ��������ͷ�����ڴ��ڲ���Ϣ��Fundamental Matrix

				cvStereoCalibrate(objects_points.cvmat, image_points.cvmat,
					image_points2.cvmat, point_counts.cvmat,
					(*intrinsic_matrix).cvmat, distortion_coeffs.cvmat,
					(*intrinsic_matrix2).cvmat, distortion_coeffs2.cvmat,
					image_size, steroR.cvmat, steroT.cvmat, E.cvmat, F.cvmat, 
					cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 100, 1e-5));

				QLabel *rotationLabelShow = ui.label_RotationMatrix;
				QLabel *translationLabelShow = ui.label_TranslationMatrix;;

				showRotationAndTranslation(steroR, steroT, translationLabelShow, rotationLabelShow);
			}
		}
	}
}

//��ȡÿһ��ͼ�񣬴�����ȡ���ǵ㣬Ȼ��Խǵ���������ؾ�ȷ��,Ȼ��ʵ�ֱ궨
bool SystemCalibration::connerDetection(CvSize& image_size, const CvSize&board_size, cvut::Seq<CvPoint2D32f>& image_points_seq)
{
	int count = 0;
	CvPoint2D32f* image_points_buf = new CvPoint2D32f[board_size.width * board_size.height];	//����ÿ��ͼ���ϼ�⵽�Ľǵ�
	cornerFilePathList.clear();     
	imageNameList.clear();

	for (int i = 0; i < imageNum; i++)
	{
		if (!filePathList.isEmpty())
		{
			QString fileFullName = filePathList.at(i);
			QByteArray fileFullNameTemp = fileFullName.toLatin1();
			Image<uchar> view(fileFullNameTemp.data());

			if (i == 0)
			{
				image_size.width = view.size().width;
				image_size.height = view.size().height;

				QFileInfo fi = QFileInfo(fileFullName);
				cornerDir = fi.absolutePath().toStdString() + "/Corner";

				/*string fileDir = fileFullName.toStdString();
				cornerDir = (fileDir.substr(0, fileDir.find_last_of('\\')) + "/Corner").c_str();*/
				if (cornerDir.empty())
				{
					QMessageBox::warning(NULL, QStringLiteral("����"), QStringLiteral("�����ǵ�洢Ŀ¼ʧ�ܣ�"), Q_NULLPTR, Q_NULLPTR);
					return false;
				}
			}

			//1.��ȡ�ǵ�
			if (cvFindChessboardCorners(view.cvimage, board_size,image_points_buf,&count, CV_CALIB_CB_ADAPTIVE_THRESH) == 0)
			{
				return false;
			}
			else
			{
				Image<uchar> view_gray(view.size(), 8, 1);
				rgb2gray(view,view_gray);

				//2.�����ؾ�ȷ��
				cvFindCornerSubPix(view_gray.cvimage,image_points_buf,count,cvSize(11,11),cvSize(-1,-1),cvTermCriteria(CV_TERMCRIT_ITER
					+CV_TERMCRIT_EPS,30,0.1));
				image_points_seq.push_back(image_points_buf,count);
				//3.��ͼ������ʾ�ǵ�λ��
				cvDrawChessboardCorners(view.cvimage,board_size,image_points_buf,count,1);

				//4.������ȡ�궨����ͼƬ
				QFileInfo fi = QFileInfo(fileFullName);
				string cornerFileFullName = cornerDir + '/' ;
				cornerFileFullName += (fi.fileName().toStdString() + ".corner.jpg");
				cvSaveImage(cornerFileFullName.c_str(), view.cvimage);

				cornerFilePathList.append(QString::fromStdString(cornerFileFullName));
				imageNameList.append(fi.fileName());

				//5.��ʾ�ǵ���ȡ���(�ڵ�ǰ��������ʾ��һ����ȡ�Ľǵ�ͼ��)
				if (i == 0)
				{
					isCorner = true;  
					picIndex = 0;

					ui.pushButton_NextPicture->setEnabled(true);
					if(imageNum == 1)
						ui.pushButton_NextPicture->setEnabled(false);

					IplImage *matImage = cvLoadImage(cornerFileFullName.c_str(), 1);  //���ر궨ͼƬ
					if (!matImage)
					{
						cerr << "func connerDetection �У�cvLoadImage�����ر궨ͼƬʧ�ܣ� " << endl;
						exit(1);
					}

					ui.label_showImage->clear();                                   //���label�ϵ�ͼƬ

					QPixmap tempPix = QPixmap(QPixmap::fromImage(cIplImgToQImg(matImage)));
					tempPix = tempPix.scaled(QSize(ui.label_showImage->width(), ui.label_showImage->height()), Qt::KeepAspectRatio);
					ui.label_showImage->setAlignment(Qt::AlignVCenter);
					ui.label_showImage->setAlignment(Qt::AlignHCenter);
					ui.label_showImage->setPixmap(tempPix);
					ui.label_showImage->show();								      //��ʾ

					cvReleaseImage(&matImage);
				}
			}
		}
		else
		{
			QMessageBox::warning(NULL, QStringLiteral("����"), QStringLiteral("��ѡ�����궨ͼƬ��"), Q_NULLPTR, Q_NULLPTR);
			break;
		}
	}

	if (image_points_buf != NULL)
	{
		delete[] image_points_buf;
	}
	
	return true;
}

//�������궨
void SystemCalibration::cameraCalibration(CvSize& image_size, const CvSize& board_size,cvut::Seq<CvPoint2D32f>& image_points_seq, 
	Matrix<double>& intrinsic_matrix,Matrix<double>& distortion_coeffs,
	Matrix<double>& rotation_vectors,Matrix<double>& translation_vectors, 
	Matrix<int>& point_counts,Matrix<double>& image_points, 
	Matrix<double>& object_points)
{

	CvSize square_size = cvSize(cellWidth,cellHeight);

	//1.��ʼ���궨���Ͻǵ����ά����
	for (int t = 0; t < imageNum; t++)
	{
		for (int i = 0; i < board_size.height; i++) //��
		{
			for (int j = 0; j < board_size.width; j++)
			{
				object_points(0, t*board_size.height * board_size.width + i * board_size.width + j, 0) = i * square_size.width;
				object_points(0, t*board_size.height * board_size.width + i * board_size.width + j, 1) = j * square_size.height;
				object_points(0, t*board_size.height * board_size.width + i * board_size.width + j, 2) = 0;
			}
		}
	}

	//2.���ǵ�Ĵ洢�ṹת���ɾ�����ʽ
	for (int i = 0; i < image_points_seq.cvseq->total; i++)
	{
		image_points(0, i, 0) = image_points_seq[i].x;
		image_points(0, i, 1) = image_points_seq[i].y;
	}

	//3.��ʼ��ÿ��ͼ���еĽǵ�����
	for (int i = 0; i < imageNum; i++)
		point_counts(0,i) = board_size.width * board_size.height;

	//4.��ʼ����
	cvCalibrateCamera2(object_points.cvmat,
		image_points.cvmat,
		point_counts.cvmat,
		image_size,
		intrinsic_matrix.cvmat,
		distortion_coeffs.cvmat,
		rotation_vectors.cvmat,
		translation_vectors.cvmat,
		0);

	if (!isBeginSystemCalib)
	{
		QMessageBox::about(NULL, QStringLiteral("����궨"), QStringLiteral("��ȡ�ǵ���ɣ��궨��ɣ�"));

		//

	}
	else
	{
		QMessageBox::about(NULL, QStringLiteral("ϵͳ�궨"), QStringLiteral("�����ƽ����ת�������ɹ���"));
	}

}

//�Ա궨�����������
void SystemCalibration::evaluateCalibResult(Matrix<int>& point_counts,Matrix<double>& objects_points, 
	Matrix<double>& rotation_vectors,Matrix<double>& translation_vectors, 
	Matrix<double>& intrinsic_matrix, Matrix<double>& distortion_coeffs,
	Matrix<double>& image_points)
{
	double total_err = 0.0;    //����ͼ���ƽ������ܺ�
	double err = 0.0;		   //ÿ��ͼ���ƽ�����
	Matrix<double> image_point2(1, point_counts(0, 0, 0), 2);  //�������¼���õ���ͶӰ��

	for (int i = 0; i < imageNum; i++)
	{
		//ͨ���õ������������������Կռ����ά������ͶӰ���㣬�õ��µ�ͶӰ��
		cvProjectPoints2(objects_points.get_cols(i*point_counts(0, 0, 0), (i + 1)*point_counts(0, 0, 0) - 1).cvmat,
			rotation_vectors.get_col(i).cvmat,
			translation_vectors.get_col(i).cvmat,
			intrinsic_matrix.cvmat,
			distortion_coeffs.cvmat,
			image_point2.cvmat,
			0, 0, 0,0);

		//�����µ�ͶӰ��;ɵ�ͶӰ��֮������
		err = cvNorm(image_points.get_cols(i*point_counts(0, 0, 0), (i + 1)*point_counts(0, 0, 0) - 1).cvmat,
			image_point2.cvmat, CV_L1);

		total_err += err /= point_counts(0, 0, 0);

		ss.clear();
		ss << err;
		ss >> data;
		data = data + "����\r\n";
		vecErr.push_back(data);
	}

	ss.clear();
	ss << total_err / imageNum ;
	ss >> data;
	data = data + "����\r\n";
	vecErr.push_back(data);

#if 0
	if (!isHandleCamera2)
	{
		SaveCalibrationResult(rotation_vectors, translation_vectors, intrinsic_matrix, distortion_coeffs);  //����궨���
		ui.pushButton_BeginCalibration->setText(QStringLiteral("��ʼ���2�궨"));
	}
	else  //�������2
	{
		SaveCalibrationResult2(rotation_vectors, translation_vectors, intrinsic_matrix, distortion_coeffs);  //����궨���
		isHandleCamera2 = false;
		ui.pushButton_BeginCalibration->setText(QStringLiteral("��ʼ���1�궨"));
	}

#endif // 0

	if (ui.pushButton_BeginCalibration->isEnabled() && isBeginCalibration)
	{
		saveCalibrationResult(rotation_vectors, translation_vectors, intrinsic_matrix, distortion_coeffs);  //����궨���
		
		isBeginCalibration = false;
	}
	else if (ui.pushButton_BeginCalibration_2->isEnabled() && isBeginCalibration2)  //�������2
	{
		saveCalibrationResult2(rotation_vectors, translation_vectors, intrinsic_matrix, distortion_coeffs);  //����궨���
		isBeginCalibration2 = false;
	}

}

//��ʾ���1�궨���
void SystemCalibration::saveCalibrationResult(Matrix<double>& rotation_vectors, Matrix<double>& translation_vectors,
	Matrix<double>& intrinsic_matrix,Matrix<double>& distortion_coeffs)
{
	//ui.stackedWidget->setCurrentIndex(ui.stackedWidget->currentIndex());

	Matrix<double> rotation_vector(3, 1);    //����ÿ��ͼ�����ת���� 
	Matrix<double> rotation_matrix(3, 3);    //����ÿ��ͼ�����ת����

	QLabel *distortionLabelShow = ui.label_distortionMatrix;
	QLabel *internalLabelShow = ui.label_internalMatrix;

	showInternalAndDistortion(intrinsic_matrix, distortion_coeffs,distortionLabelShow, internalLabelShow);
	
	ss.clear();
	ss.str("");
	for (int i = 0; i < imageNum; i++) {
		QString fileFullName = imageNameList.at(i);
		QByteArray fileFullNameTemp = fileFullName.toLatin1();

		ss << "\r\n��ͼ��" << fileFullNameTemp.data() << "��\r\n\tƽ����" << vecErr[i] << "\r\n";
		ss << "\t��ת������\r\n";
		//ss << rotation_vectors.get_col(i);
		ss << "\t" << rotation_vectors.data[0] << "\t" << rotation_vectors.data[1] << "\t" << rotation_vectors.data[2];


		//����ת�������д洢��ʽת�� 
		for (int j = 0; j < 3; j++)
			//data[i_row*cols + i_col*channels + i_channel];
			rotation_vector(j, 0, 0) = rotation_vectors(0, i, j);
		//����ת����ת��Ϊ���Ӧ����ת���� 
		cvRodrigues2(rotation_vector.cvmat, rotation_matrix.cvmat);

		ss << "\r\n\t��ת����\r\n";
		ss << rotation_matrix;
		ss << "\r\n\tƽ��������\r\n";
		//ss << translation_vectors.get_col(i) << "\r\n";
		ss << "\t" << translation_vectors.data[0] << "\t" << translation_vectors.data[1] << "\t" << translation_vectors.data[2];

	}
	ss << "\r\n����ƽ����" << vecErr[imageNum] << "\r\n";
	data = ss.str();

	//QTextCodec::setCodecForLocale(QTextCodec::codecForName("system"));
	QString str;
	ui.label_scrollArea->setText(str.fromLocal8Bit(data.c_str()));
	isClibrationCamer1 = true;
}

//��ʾ���2�궨���
void SystemCalibration::saveCalibrationResult2(Matrix<double>& rotation_vectors, Matrix<double>& translation_vectors,
	Matrix<double>& intrinsic_matrix, Matrix<double>& distortion_coeffs)
{
	//ui.stackedWidget->setCurrentIndex(ui.stackedWidget->currentIndex());

	Matrix<double> rotation_vector(3, 1);    //����ÿ��ͼ�����ת���� 
	Matrix<double> rotation_matrix(3, 3);    //����ÿ��ͼ�����ת����

	QLabel *distortionLabelShow = ui.label_distortionMatrix_2;
	QLabel *internalLabelShow = ui.label_internalMatrix_2;

	showInternalAndDistortion(intrinsic_matrix, distortion_coeffs, distortionLabelShow, internalLabelShow);

	ss.clear();
	ss.str("");
	for (int i = 0; i < imageNum; i++) {
		QString fileFullName = imageNameList.at(i);
		QByteArray fileFullNameTemp = fileFullName.toLatin1();

		ss << "\r\n��ͼ��" << fileFullNameTemp.data() << "��\r\n\tƽ����" << vecErr[i] << "\r\n";
		ss << "\t��ת������\r\n";
		//ss << rotation_vectors.get_col(i);
		ss << "\t" << rotation_vectors.data[0] << "\t" << rotation_vectors.data[1] << "\t" << rotation_vectors.data[2];

		//����ת�������д洢��ʽת�� 
		for (int j = 0; j < 3; j++)
			//data[i_row*cols + i_col*channels + i_channel];
			rotation_vector(j, 0, 0) = rotation_vectors(0, i, j);

		//����ת����ת��Ϊ���Ӧ����ת���� 
		cvRodrigues2(rotation_vector.cvmat, rotation_matrix.cvmat);

		ss << "\r\n\t��ת����\r\n";
		ss << rotation_matrix;
		ss << "\r\n\tƽ��������\r\n";
		//ss << translation_vectors.get_col(i) << "\r\n";
		ss << "\t" << translation_vectors.data[0] << "\t" << translation_vectors.data[1] << "\t" << translation_vectors.data[2];
	}
	ss << "\r\n����ƽ����" << vecErr[imageNum] << "\r\n";
	data = ss.str();

	QString str;
	ui.label_scrollArea->setText(str.fromLocal8Bit(data.c_str()));
	isClibrationCamer2 = true;
}

//��ʾ����ڲ����ͻ���ϵ��
void SystemCalibration::showInternalAndDistortion(Matrix<double>& intrinsic_matrix, Matrix<double>& distortion_coeffs,
	QLabel* distortionLabelShow, QLabel*internalLabelShow)
{
	vector<string> internalMatrix(9);
	convertTo1DMatrix(intrinsic_matrix, internalMatrix);

	vector<string> distortion(4);
	convertTo1DMatrix(distortion_coeffs, distortion);

	string internalStr = "\t";
	string distortionStr = "\t";
	for (int i = 1; i < 10; i++)
	{
		if (i % 3 == 0)
		{
			internalStr += internalMatrix[i - 1] + "\r\n\t";
		}
		else
		{
			internalStr += internalMatrix[i - 1] + "\t";
		}
	}

	for (int i = 1; i < 5; i++)
	{
		if (i % 2 == 0)
		{
			distortionStr += distortion[i - 1] + "\r\n\t"; //
		}
		else
		{
			distortionStr += distortion[i - 1] + "\t";
		}
	}

	internalLabelShow->setText(internalStr.c_str());
	distortionLabelShow->setText(distortionStr.c_str());
}

//��ʾ�����ƽ����ת����
void SystemCalibration::showRotationAndTranslation(Matrix<double>& steroR, Matrix<double>& steroT,
	QLabel* translationLabelShow, QLabel*rotationLabelShow)
{
	vector<string> steroRMatrix(9);
	convertTo1DMatrix(steroR, steroRMatrix);

	vector<string> steroTMatrix(3);
	convertTo1DMatrix(steroT, steroTMatrix);

	string steroRStr = "\t";
	string steroTStr = "\t";
	for (int i = 1; i < 10; i++)
	{
		if (i % 3 == 0)
		{
			steroRStr += steroRMatrix[i - 1] + "\r\n\t";
		}
		else
		{
			steroRStr += steroRMatrix[i - 1] + "\t";
		}
	}

	for (int i = 1; i < 4; i++)
	{
		steroTStr += steroTMatrix[i - 1] + "\t\t";
	}

	rotationLabelShow->setText(steroRStr.c_str());
	translationLabelShow->setText(steroTStr.c_str());
}

//ϵͳ�궨
void SystemCalibration::BeginSystemCalib()
{
	int nIndex = ui.stackedWidget->currentIndex();
	ui.stackedWidget->setCurrentIndex(nIndex - 1);

	isBeginSystemCalib = true;
	BeginCalibration();
}

//��һ��ͼƬ
void SystemCalibration::OnBnClickedButtonNext()
{
	picIndex++;

	if (picIndex >= 0 && picIndex < imageNum)
	{
		QString fileFullName = filePathList.at(picIndex);
		loadAndShowImage(picIndex, fileFullName);

		if (isCorner)
		{
			QString cornerFileFullName = cornerFilePathList.at(picIndex);
			loadAndShowImage(picIndex, cornerFileFullName);
		}
	}
	else
	{
		picIndex--;
	}

	if (picIndex == imageNum - 1)
	{
		ui.pushButton_NextPicture->setEnabled(false);
	}

	if (picIndex > 0)
	{
		ui.pushButton_PrePicture->setEnabled(true);
	}
}

//��һ��ͼƬ
void SystemCalibration::OnBnClickedButtonPre()
{
	picIndex--;

	if (picIndex >= 0 && picIndex < imageNum)
	{
		QString fileFullName = filePathList.at(picIndex);
		loadAndShowImage(picIndex, fileFullName);

		if (isCorner)
		{
			QString cornerFileFullName = cornerFilePathList.at(picIndex);
			loadAndShowImage(picIndex, cornerFileFullName);
		}
	}
	else
	{
		picIndex++;
	}

	if (picIndex == 0)  //һ��ͼƬʱ
	{
		ui.pushButton_PrePicture->setEnabled(false);
	}

	if (picIndex < imageNum - 1) //-1��imageNum-1ʱ����picIndex���ٵ�����£�����һ�š���ť����
	{
		ui.pushButton_NextPicture->setEnabled(true);
	}
}

void SystemCalibration::loadAndShowImage(int index, const QString &cfileFullName)
{
	QString fileFullName = cfileFullName;

	QByteArray fileFullNameTemp = fileFullName.toLatin1();
	IplImage *matImage = cvLoadImage(fileFullNameTemp.data(), 1);  //���ر궨ͼƬ
	if (!matImage) return;

	ui.label_showImage->clear();                                   //���label�ϵ�ͼƬ

	QPixmap tempPix = QPixmap(QPixmap::fromImage(cIplImgToQImg(matImage)));
	tempPix = tempPix.scaled(QSize(ui.label_showImage->width(), ui.label_showImage->height()), Qt::KeepAspectRatio);
	ui.label_showImage->setPixmap(tempPix);
	ui.label_showImage->setAlignment(Qt::AlignVCenter);
	ui.label_showImage->setAlignment(Qt::AlignHCenter);
	ui.label_showImage->show();								       //��ʾ

	cvReleaseImage(&matImage);
}
//��ȡ�û��������̸����
void SystemCalibration::readChessboardPara(float& cellHeight, float &cellWidth, int &chessRow, int &chessColumn)
{
	QString gridRowSize = ui.lineEdit_GridSizeRow->text();
	QString gridColSize = ui.lineEdit_GridSizeCol->text();
	QString gridRowNum = ui.lineEdit_GridRowNum->text();
	QString gridColNum = ui.lineEdit_GridColNum->text();

	if (gridRowSize.isEmpty() || gridColSize.isEmpty() || gridRowNum.isEmpty() || gridColNum.isEmpty())
	{
		QMessageBox::warning(NULL, QStringLiteral("����"), QStringLiteral("���������̸����"), Q_NULLPTR, Q_NULLPTR);
		return;
	}
	else
	{
		float gridRowSizee = gridRowSize.toFloat();
		float gridColSizee = gridColSize.toFloat();
		int gridRowNumm = gridRowNum.toInt();
		int gridColNumm = gridColNum.toInt();

		if (gridRowSizee <= 0 || gridColSizee <= 0 || gridRowNumm <= 0 || gridColNumm <= 0)
		{
			QMessageBox::warning(NULL, QStringLiteral("����"), QStringLiteral("���̸�������벻�Ϸ�"), Q_NULLPTR, Q_NULLPTR);
		}
		else
		{
			cellHeight = gridColSizee;
			cellWidth = gridRowSizee;
			chessRow = gridRowNumm;
			chessColumn = gridColNumm;
		}
	}

}

void SystemCalibration::BackToHome()
{
	ui.stackedWidget->setCurrentIndex(1);
}

//1.����ͼƬ����ȡ����������ĺ�õ�������
void SystemCalibration::GetFeaturePoint()
{
	cornerFilePathList.clear();  //�洢��ȡ��������֮���ͼƬ
	imageNameList.clear();       //�洢��ȡ��������֮ǰ��ͼƬ
	ss.clear();
	ss.str("");

	if (!filePathList.isEmpty())
	{
		//1.����ͼƬ
		QString fileFullName = filePathList.at(0);
		QByteArray fileFullNameTemp = fileFullName.toLatin1();
		Image<uchar> view(fileFullNameTemp.data());

		QFileInfo fi = QFileInfo(fileFullName);
		cornerDir = fi.absolutePath().toStdString() + "/Corner";
		if (cornerDir.empty())
		{
			QMessageBox::warning(NULL, QStringLiteral("����"), QStringLiteral("����corner�洢Ŀ¼ʧ�ܣ�"), Q_NULLPTR, Q_NULLPTR);
			return ;
		}

		//2.��ɹ���������ȡ
		allFeaturePoints = advancedThreWeight(view.cvimage);
		if (!allFeaturePoints.empty())
		{
			featurePointNum = 1;
			for (vector<CvPoint2D32f>::iterator it = allFeaturePoints.begin(); it != allFeaturePoints.end(); featurePointNum++, it++)
			{
				ss << "(" << it->x << "," <<  it->y << ")";
				if (featurePointNum % 2 == 0) ss << endl;
			}
			ss << "\t number :" << featurePointNum - 1 ;
			data = ss.str();
			ui.label_scrollArea_featurePoint->setText(data.c_str());
		}

		//3.�������������ȡ�궨����ͼƬ ����ʾ
		QFileInfo qfi = QFileInfo(fileFullName);
		string cornerFileFullName = cornerDir + '/';
		cornerFileFullName += (qfi.fileName().toStdString() + ".laser.jpg");
		cvSaveImage(cornerFileFullName.c_str(), view.cvimage);
		cornerFilePathList.append(QString::fromStdString(cornerFileFullName));
		imageNameList.append(qfi.fileName());

		IplImage* matImage = cvLoadImage(cornerFileFullName.c_str(), 1); 
		if (!matImage)
		{
			cerr << "func GetFeaturePoint::cvLoadImage " << endl;
			exit(1);
		}
		ui.label_showImage->clear();                                   
		QPixmap tempPix = QPixmap(QPixmap::fromImage(cIplImgToQImg(matImage)));
		tempPix = tempPix.scaled(QSize(ui.label_showImage->width(), ui.label_showImage->height()), Qt::KeepAspectRatio);
		ui.label_showImage->setAlignment(Qt::AlignVCenter);
		ui.label_showImage->setAlignment(Qt::AlignHCenter);
		ui.label_showImage->setPixmap(tempPix);
		ui.label_showImage->show();								    

		cvReleaseImage(&matImage);
	}
	
}

//2.������ת�������������ϵ��ȷ������ƽ�棬��ȡ��ת���߷���
void SystemCalibration::GetEquationCoeff1()
{
	Plane plane1,plane2;
	Line line;

	//��ü���ƽ��
	if (flag1)
	{
		getLaserPlane(plane1);
		isPlaneOk++;
		flag1 = false;
		ui.pushButton_coefficient1->setText(u8"3����ȡ������ֱ�߷���");
	}
	else
	{
		getLaserPlane(plane2);
		isPlaneOk++;
		flag1 = true;
		ui.pushButton_coefficient1->setText(u8"2����ȡ����ƽ�淽��");
	}
	
	//����ƽ���ཻ�õ���ת����
	if (isPlaneOk > 0 && isPlaneOk % 2 == 0)
	{
		line = eqations.CrossPlaneLine(plane1, plane2);

		showLine(line);
	}
}

void SystemCalibration::GetEquationCoeff2()
{
	Plane plane1, plane2;
	Line line;
	
	//��ü���ƽ��
	if (flag2)
	{
		getLaserFitPlane(plane1);
		isPlaneOk1++;
		flag2 = false;
		ui.pushButton_coefficient2->setText(u8"3����ȡ������ֱ�߷���");
	}
	else
	{
		getLaserFitPlane(plane2);
		isPlaneOk1++;
		flag2 = true;
		ui.pushButton_coefficient2->setText(u8"2����ȡ����ƽ�淽��");
	}

	//����ƽ���ཻ�õ���ת����
	if (isPlaneOk1 > 0 && isPlaneOk1 % 2 == 0)
	{
		line = eqations.CrossPlaneLine(plane1, plane2);
		showLine2(line);
	}
}

//��ü���ƽ��
void SystemCalibration::getLaserPlane(Plane& plane)
{
	//1)��������������ϵת��  intrinsic_matrix   allFeaturePoints[0]
	Plane planeTmp;
	Matrix<double> featurePoints_matrixCamera(3, 1);
	Matrix<double> featurePoints_matrixCamera1(3, 1);
	Matrix<double> featurePoints_matrixCamera2(3, 1);
	Matrix<double> intrinsic_matrixInvert(3, 3);

	//intrinsic_matrixInvert = invert(*intrinsic_matrix);   
	cvInvert(intrinsic_matrix->cvmat, intrinsic_matrixInvert.cvmat, DECOMP_LU);

	int halfPoint = cvRound((featurePointNum - 1) / 2);
	double arr[] = { allFeaturePoints[0].x,allFeaturePoints[0].y, 1 };
	double arr1[] = { allFeaturePoints[halfPoint].x,allFeaturePoints[halfPoint].y, 1 };
	double arr2[] = { allFeaturePoints[featurePointNum - 2].x,allFeaturePoints[featurePointNum - 2].y, 1 };
	CvMat matTmp = cvMat(3, 1, CV_64FC1, arr);
	Matrix<double>	featurePoints_matrixPixel(&matTmp);
	CvMat matTmp1 = cvMat(3, 1, CV_64FC1, arr1);
	Matrix<double>	featurePoints_matrixPixel1(&matTmp1);
	CvMat matTmp2 = cvMat(3, 1, CV_64FC1, arr2);
	Matrix<double>	featurePoints_matrixPixel2(&matTmp2);

	//ss.clear();
	//ss.str("");
	//for (int i = 0; i < matTmp.rows; i++)
	//{
	//	for (int j = 0; j < matTmp.cols; j++)
	//	{
	//		ss << cvmGet(featurePoints_matrixPixel2.cvmat, i, j )<< "\t";
	//	}
	//}
	//data = ss.str();
	//ui.lineEdit_A->setText(data.c_str());   //���ص� ����

	//featurePoints_matrixCamera = intrinsic_matrixInvert*featurePoints_matrixPixel;  
	cvMatMul(intrinsic_matrixInvert.cvmat, featurePoints_matrixPixel.cvmat, featurePoints_matrixCamera.cvmat);
	cvMatMul(intrinsic_matrixInvert.cvmat, featurePoints_matrixPixel1.cvmat, featurePoints_matrixCamera1.cvmat);
	cvMatMul(intrinsic_matrixInvert.cvmat, featurePoints_matrixPixel2.cvmat, featurePoints_matrixCamera2.cvmat);


	//���������ϵ�� ����
//	testShow31(featurePoints_matrixCamera2, featurePoints_matrixCamera2, ui.label_scrollArea_featurePoint, ui.label_scrollArea_featurePoint);

	//2)����ȷ��һ������ƽ��
	CvPoint3D64f point1 = { featurePoints_matrixCamera.data[0],featurePoints_matrixCamera.data[1],featurePoints_matrixCamera.data[2] };
	CvPoint3D64f point2 = { featurePoints_matrixCamera1.data[0],featurePoints_matrixCamera1.data[1],featurePoints_matrixCamera1.data[2] };
	CvPoint3D64f point3 = { featurePoints_matrixCamera2.data[0],featurePoints_matrixCamera2.data[1],featurePoints_matrixCamera2.data[2] };
	planeTmp = eqations.CreatePlaneEquation(point1, point2, point3);

	ss.clear();
	ss.str("");
	ss << plane.getA() << "\t" << plane.getB() << "\t" << plane.getC();
	ss << plane.getPoint().x << "\t" << plane.getPoint().y << "\t" << plane.getPoint().z << "\t";
	data = ss.str();
	ui.label_scrollArea_featurePoint->setText(data.c_str());  //����ƽ�����

	plane = planeTmp;
}

void SystemCalibration::getLaserFitPlane(Plane& plane)
{
	Plane planeTmp;
	Matrix<double> featurePoints_matrixCamera(3, 1);
	Matrix<double> intrinsic_matrixInvert(3, 3);

	cvInvert(intrinsic_matrix->cvmat, intrinsic_matrixInvert.cvmat, DECOMP_LU);
	CvMat *points_mat = cvCreateMat(allFeaturePoints.size(), 3, CV_64FC1);

	for (int i = 0; i < allFeaturePoints.size(); i++)
	{
		double arr[] = { allFeaturePoints[i].x,allFeaturePoints[i].y, 1 };

		CvMat matTmp = cvMat(3, 1, CV_64FC1, arr);
		Matrix<double>	featurePoints_matrixPixel(&matTmp);

		cvMatMul(intrinsic_matrixInvert.cvmat, featurePoints_matrixPixel.cvmat, featurePoints_matrixCamera.cvmat);
		points_mat->data.db[i * 3 + 0] = (featurePoints_matrixCamera.cvmat)->data.db[0];
		points_mat->data.db[i * 3 + 1] = (featurePoints_matrixCamera.cvmat)->data.db[1];
		points_mat->data.db[i * 3 + 2] = (featurePoints_matrixCamera.cvmat)->data.db[2];
	}	

	ss.clear();
	ss.str("");
	for (int i = 0; i < points_mat->rows; i++)
	{
		for (int j = 0; j < points_mat->cols; j++)
		{
			ss << cvmGet(points_mat, i, j )<< "\t";
			ss << "\r\n";
		}
	}
	data = ss.str();
	ui.label_scrollArea_featurePoint->setText(data.c_str());   //���ص� ����

	//���������ϵ�㣺����
	//testShow31(intrinsic_matrixInvert, featurePoints_matrixCamera, ui.label_scrollArea_featurePoint, ui.label_scrollArea_featurePoint);

	double planeArry[4] = {0};
	cvFitPlane(points_mat, planeArry);
	plane.setAA(planeArry[0]);
	plane.setBB(planeArry[1]);
	plane.setCC(planeArry[2]);
	plane.setDD(planeArry[3]);	


	ss.clear();
	ss.str("");
	ss << plane.getA() << "\t" << plane.getB() << "\t" << plane.getC();
	ss << plane.getPoint().x << "\t" << plane.getPoint().y << "\t" << plane.getPoint().z << "\t";
	data = ss.str();
	ui.label_scrollArea_featurePoint->setText(data.c_str());  //����ƽ�����

	cvReleaseMat(&points_mat);
}

void SystemCalibration::cvFitPlane(const CvMat* points, double *plane)
{
	// ��������  
	int nrows = points->rows;
	int ncols = points->cols;
	int type = points->type;
	CvMat* centroid = cvCreateMat(1, ncols, type);
	cvSet(centroid, cvScalar(0));
	for (int c = 0; c<ncols; c++) {
		for (int r = 0; r < nrows; r++)
		{
			centroid->data.fl[c] += points->data.fl[ncols*r + c];
		}
		centroid->data.fl[c] /= nrows;
	}
	// ÿ�����ȥ��������  
	CvMat* points2 = cvCreateMat(nrows, ncols, type);
	for (int r = 0; r<nrows; r++)
		for (int c = 0; c<ncols; c++)
			points2->data.fl[ncols*r + c] = points->data.fl[ncols*r + c] - centroid->data.fl[c];
	// Э������������ֵ 
	CvMat* A = cvCreateMat(ncols, ncols, type);
	CvMat* W = cvCreateMat(ncols, ncols, type);
	CvMat* V = cvCreateMat(ncols, ncols, type);
	cvGEMM(points2, points, 1, NULL, 0, A, CV_GEMM_A_T);
	cvSVD(A, W, NULL, V, CV_SVD_V_T);
	// ָ��ƽ��ϵ��������������Ӧ����С������ 
	plane[ncols] = 0;
	for (int c = 0; c<ncols; c++) {
		plane[c] = V->data.fl[ncols*(ncols - 1) + c];
		plane[ncols] += plane[c] * centroid->data.fl[c];
	}
	
	cvReleaseMat(&centroid);
	cvReleaseMat(&points2);
	cvReleaseMat(&A);
	cvReleaseMat(&W);
	cvReleaseMat(&V);
}

void SystemCalibration::showLine(Line &line)
{
	ss.clear();
	ss.str("");
	ss << line.getA();
	data = ss.str();
	ui.lineEdit_A->setText(data.c_str());  
	ss.clear();
	ss.str("");
	ss << line.getB();
	data = ss.str();
	ui.lineEdit_B->setText(data.c_str());
	ss.clear();
	ss.str("");
	ss << line.getC();
	data = ss.str();
	ui.lineEdit_C->setText(data.c_str());
	ss.clear();
	ss.str("");
	data = ss.str();
	ss << line.getPoint().x << "\t" << line.getPoint().y << "\t" << line.getPoint().z << "\t";
	ui.lineEdit_D->setText(data.c_str());
}

void SystemCalibration::showLine2(Line &line)
{
	ss.clear();
	ss.str("");
	ss << line.getA();
	data = ss.str();
	ui.lineEdit_AA->setText(data.c_str());
	ss.clear();
	ss.str("");
	ss << line.getB();
	data = ss.str();
	ui.lineEdit_14->setText(data.c_str());
	ss.clear();
	ss.str("");
	ss << line.getC();
	data = ss.str();
	ui.lineEdit_CC->setText(data.c_str());
	ss.clear();
	ss.str("");
	data = ss.str();
	ss << line.getPoint().x << "\t" << line.getPoint().y << "\t" << line.getPoint().z << "\t";
	ui.lineEdit_DD->setText(data.c_str());
}

vector<CvPoint2D32f> SystemCalibration::advancedThreWeight(IplImage *imageView)
{
	/*QByteArray fileFullNameTemp = fileFullName.toLatin1();
	IplImage* matImage = cvLoadImage(fileFullNameTemp.data(), 1);*/
	IplImage *gray = cvCreateImage(cvGetSize(imageView), IPL_DEPTH_8U, 1);
	//IplImage *imageViewTmp = cvCreateImage(cvGetSize(imageView), IPL_DEPTH_8U, 3);
	cvSmooth(imageView, imageView, CV_MEDIAN, 3);

	/*CvMat *mat = cvCreateMat(imageView->height, imageView->width, CV_64FC3);
	cvConvert(imageView, mat);*/
	//threshold((InputArray)imageView, (OutputArray)imageViewTmp, 100, 255, THRESH_BINARY);


	vector<CvPoint2D32f> allPoints;
	cvZero(gray);

	//ת��Ϊ�Ҷ�ͼ
	cvCvtColor(imageView, gray, CV_BGR2GRAY);
	//greyCenter(gray, allPoints);
	int height = imageView->height;
	int width = imageView->width;
	int data;
	int UPSum = 0;
	int DSum = 0;
	float Avg;
	for (int j = 0; j<height; j++)				
	{
		UPSum = 0;
		DSum = 0;
		int tempthre = 0;						 //��ʱ��ֵ
		bool flag = true; int up = 0; int down = 0;
	
		tempthre = adaptThreshold(gray, width, j);  	

		for (int i = 0; i<width; i++)			 //����һ��ͼƬ
		{
			data = (uchar)gray->imageData[j*gray->widthStep + i];
			if (data>tempthre)
			{
				if (flag) {
					up = i;
					flag = false;
				}
				UPSum = UPSum + data*(i + 1);
				DSum = DSum + data;
				down = i;
			}
		}
		flag = true;
		if ((DSum>0) && ((down - up)<5) )//������Ȳ�����10������
		{
			Avg = (float)UPSum / DSum;
			if (j > 500 && j < 800)
			{
				allPoints.push_back(cvPoint2D32f(Avg, j));
			}
		}
	}
	drawPoint(imageView, allPoints);

	cvReleaseImage(&gray);

	return allPoints;
}

void SystemCalibration::drawPoint(IplImage* image, vector<CvPoint2D32f> allpoints)
{
	for (int i = 0; i < allpoints.size(); i++)
	{
		cvCircle(image, cvPoint((int)allpoints[i].x, (int)allpoints[i].y), 1, CV_RGB(0, 255, 0), 1);
	}
}

int SystemCalibration::adaptThreshold(IplImage *pic, int width, int j)
{
	int data = 0;
	int maxdata = 0;

	for (int i = 0; i<width; i++)	  
	{
		data = (uchar)pic->imageData[j*pic->widthStep + i];

		if (data>maxdata) {  		//����Ӧ��ֵ�Ĳ���
			maxdata = data;
		}
	}

	return maxdata -50;
}

//IplImageתQImage
QImage SystemCalibration::cIplImgToQImg(IplImage* cvimage)
{
	QImage destImage(cvimage->width, cvimage->height, QImage::Format_RGB32);

	for (int i = 0; i < cvimage->height; i++)
	{
		for (int j = 0; j < cvimage->width; j++)
		{
			int r, g, b;
			if (3 == cvimage->nChannels)
			{
				b = (int)CV_IMAGE_ELEM(cvimage, uchar, i, j * 3 + 0);
				g = (int)CV_IMAGE_ELEM(cvimage, uchar, i, j * 3 + 1);
				r = (int)CV_IMAGE_ELEM(cvimage, uchar, i, j * 3 + 2);
			}
			else if (1 == cvimage->nChannels)
			{
				b = (int)CV_IMAGE_ELEM(cvimage, uchar, i, j);
				g = b;
				r = b;
			}

			destImage.setPixel(j, i, qRgb(r, g, b));
		}
	}

	return destImage;
}
//����: 3 * 3�����ӡ
void SystemCalibration::testShow33(Matrix<double>& steroR, Matrix<double>& steroT, QLabel* translationLabelShow, QLabel* rotationLabelShow)
{
	vector<string> steroRMatrix(9);
	convertTo1DMatrix(steroR, steroRMatrix);

	vector<string> steroTMatrix(9);
	convertTo1DMatrix(steroT, steroTMatrix);

	string steroRStr = "\t";
	string steroTStr = "\t";
	for (int i = 1; i < 10; i++)
	{
		if (i % 3 == 0)
		{
			steroRStr += steroRMatrix[i - 1] + "\r\n\t";
			steroTStr += steroTMatrix[i - 1] + "\r\n\t";

		}
		else
		{
			steroRStr += steroRMatrix[i - 1] + "\t";
			steroTStr += steroTMatrix[i - 1] + "\t";
		}
	}

	rotationLabelShow->setText(steroRStr.c_str());
	translationLabelShow->setText(steroTStr.c_str());
}

void SystemCalibration::testShow31(Matrix<double>& steroR, Matrix<double>& steroT, QLabel* translationLabelShow, QLabel* rotationLabelShow)
{
	vector<string> steroRMatrix(9);
	convertTo1DMatrix(steroR, steroRMatrix);

	vector<string> steroTMatrix(3);
	convertTo1DMatrix(steroT, steroTMatrix);

	string steroRStr = "\t";
	string steroTStr = "\t";
	for (int i = 1; i < 10; i++)
	{
		if (i % 3 == 0)
		{
			steroRStr += steroRMatrix[i - 1] + "\r\n\t";

		}
		else
		{
			steroRStr += steroRMatrix[i - 1] + "\t";
		}
	}

	for (int i = 1; i < 4; i++)
	{
		steroTStr += steroTMatrix[i - 1] + "\t\t";
	}
	
	//ss.clear();
	//ss.str("");
	//ss << "\t" << steroT.data[0] << "\t" << steroT.data[1] << "\t" << steroT.data[2];
	//data = ss.str();

	rotationLabelShow->setText(steroRStr.c_str());
	translationLabelShow->setText(steroTStr.c_str());
}