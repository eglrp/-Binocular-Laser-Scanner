#pragma once
#include<Windows.h>
#include"MVCAPI.h"
#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>  
#include<opencv2\imgproc\imgproc.hpp>  
#include<iostream>  
using namespace cv;
#define  CAMNO 4
class CameraCapture
{
public:
	virtual BOOL OnInitialize();  //��ʼ��
	BOOL Open();
	BOOL Close();
	//������������
	BOOL Capture();
	BOOL CaptureImage(MVCFRAMEINFO lFrameInfo, BOOL l_bRaw);
	void getIndex(DWORD num);
	//�ص�����
	static UINT WINAPI MStreamProc(WORD iDevNo, MVCFRAMEINFO m_FrameInfo, PVOID pUserData); //�ɼ��ص�����
	static DWORD WINAPI Processfunc(LPVOID lpData); //������
	void  ReInitDisp();

public:
	BOOL			m_bColor;		//��ɫ�����־
	BOOL			m_bDevopen;		//�豸������־
	BOOL			m_bCapture;		//�ɼ�������־
	BOOL          m_bMatSaveOnce;           //�ɼ�Mat��־
	DWORD		m_iDevIndex;	//�豸���
	DWORD			m_iDevType;		//�豸�ͺ�
	MVCGE_DEVLIST lInfo;  //�豸��Ϣ
	DWORD			g_iTotalCaped;	//�ɼ���֡��
	DWORD			g_iCapNumber;	//�ɼ�֡��
	DWORD			g_iDspNumber;	//��ʾ֡��
	int				m_iImgBit;		//�ɼ�λ��
	int				m_iImgWidth;	//�ɼ����
	int				m_iImgHeight;	//�ɼ��߶�
	BOOL         	m_bJumbo;  //��֡������־
	BOOL			m_bHookData;	//���ݴ����ʶ
	PBYTE			m_pRAWData;		//ԭʼͼ��Buffer
	PBYTE			m_pRAWDataEx;	//ԭʼͼ��Buffer
	PBYTE			m_pRGBData;		//ת����ɫBuffer
	MVCFRAMEINFO	RAWFrameInfo;	//ԭʼͼ��ṹ��
	MVCFRAMEINFO	CVTFrameInfo;	//ת��ͼ��ṹ��
	HANDLE			hProcessThread;	//���������
	HANDLE			hEventBuf;		//ͼ��Ԥ���ź�
	HANDLE			hEventEnd;		//�����˳��ź�
	BOOL m_bHookDisp;  //ͼ����ʾ��־
	BOOL m_bBayer3x3; //T:ʹ��Bayer3x3ת����ʾ,F:��ͨBayerת��
	HWND			m_Hwnd;			//�����ھ��
	RECT			m_RectEx;		//�����ھ���
	Mat              imgMat;      
	int saveTimes;
	char buffer_name[50];
	char buffer_name2[50];
};