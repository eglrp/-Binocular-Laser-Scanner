#include "CameraCapture.h"

//CameraCapture *pDemoDlg;
BOOL CameraCapture::OnInitialize()
{
	// TODO: Add extra initialization here
//	pDemoDlg = this;
	m_bDevopen = FALSE;
	m_bCapture = FALSE;
	m_bHookDisp = TRUE;
	m_bBayer3x3 = FALSE;
	m_bJumbo = FALSE;
	m_bMatSaveOnce = FALSE;
	m_pRAWData = NULL;
	m_pRAWDataEx = NULL;
	m_pRGBData = NULL;
	g_iTotalCaped = 0;
	g_iCapNumber = 0;
	g_iDspNumber = 0;
	saveTimes = 0;
	m_bColor =1;
	m_iImgBit = 8;
	//���ұ��������Ŀ
	if (MVC_GetDeviceNumber() == 0)
	{
		return FALSE;
	}
	else {
		//��ȡ�豸��Ϣ
		MVC_GetDeviceInfo(m_iDevIndex, &lInfo);
		return TRUE;
	}
}

void CameraCapture::getIndex(DWORD num)
{
	m_iDevIndex = num;
}
BOOL CameraCapture::Open()
{
	//��������֡
	if (!m_bJumbo) MVC_SetNetPacketSize(m_iDevIndex, 1440);
	//�Զ�����IP��ַ
	MVC_AutoIPConfig(m_iDevIndex);
	if (MVC_OpenDevice(m_iDevIndex) == CY_RESULT_OK)
	{
		m_iDevType = MVC_GetCameraType(m_iDevIndex);
		if (m_iDevType == RS_A1300_GM60)
		{
			//�ڰ������ͨ��
			m_bColor = 0;  
		}
		//����ͼ���ڴ� ��MAX����
		m_iImgWidth = MVC_GetParameter(m_iDevIndex, MVCADJ_WIDTH);
		m_iImgHeight = MVC_GetParameter(m_iDevIndex, MVCADJ_HEIGHT);
		int MAX_WIDTH = 1600;
		int MAX_HEIGHT = 1200;
	    m_pRAWData = (BYTE*)malloc(MAX_WIDTH*MAX_HEIGHT * 2);
		m_pRAWDataEx = (BYTE*)malloc(MAX_WIDTH*MAX_HEIGHT*CAMNO * 2);
		m_pRGBData = (BYTE*)malloc(MAX_WIDTH*MAX_HEIGHT*CAMNO * 6);
		memset(&RAWFrameInfo, 0, sizeof(MVCFRAMEINFO));
		memset(&CVTFrameInfo, 0, sizeof(MVCFRAMEINFO));
		RAWFrameInfo.lBufPtr = m_pRAWData;
		CVTFrameInfo.lBufPtr = m_pRGBData;
		//�ر��ڲ���ʾ
		MVC_SetParameter(m_iDevIndex, MVCDISP_ISDISP, 0);
		//���ڲ�ͬ����İ��ӳ������������˲ʱ����
		if (m_iDevIndex == 1)
		{
			MVC_SetParameter(m_iDevIndex, MVCADJ_INTERPACKETDELAY, 1000);
			MVC_SetParameter(m_iDevIndex, MVCCAP_BUFQUEUESIZE, 4); //��������������ɼ�
		}

		//6.�߳̿���
		DWORD ThreadId = m_iDevIndex;
		hProcessThread = CreateThread(NULL, 0, Processfunc, this, 0, &ThreadId);
		SetThreadPriority(hProcessThread, THREAD_PRIORITY_HIGHEST);
		hEventEnd = CreateEvent(NULL, FALSE, FALSE, NULL);
		hEventBuf = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_bHookData = 0;
		//ÿ1s����һ��On Timer����
		//SetTimer(0, 1000, NULL);

		m_bDevopen = 1;
		return TRUE;
	}
	else {
		return FALSE;
	}

}
BOOL CameraCapture::Close()
{
	if (MVC_DisableCapture(m_iDevIndex) == CY_RESULT_OK)
	{
		if (MVC_CloseDevice(m_iDevIndex)== CY_RESULT_OK)
		{
			return true;
		}
		return false;
	}
	return false;
}
BOOL CameraCapture::Capture()
{
	//�󶨻ص�����
	memset(&m_RectEx, 0, sizeof(RECT));
	//MVC_SubDisplayInit(m_iDevIndex, 0, m_RectEx, m_Hwnd, m_bBayer3x3);
	MVC_SetStreamHOOK(m_iDevIndex, MStreamProc, this);
	
	if (MVC_EnableCapture(m_iDevIndex) == CY_RESULT_OK)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}


UINT WINAPI CameraCapture::MStreamProc(WORD lDevNo, MVCFRAMEINFO m_FrameInfo, PVOID pUserData)
{
	CameraCapture *ptDlg = (CameraCapture *)(pUserData);
	ptDlg->g_iTotalCaped++;
    ptDlg->g_iCapNumber++;
	ptDlg->m_iImgBit = m_FrameInfo.PixelID;

	//��ȡָ��
	BYTE* DataPtr = (BYTE*)m_FrameInfo.lBufPtr;

	if (!ptDlg->m_bHookData)
	{
		memcpy(&ptDlg->RAWFrameInfo, &m_FrameInfo, sizeof(MVCFRAMEINFO));
		memcpy(ptDlg->m_pRAWData, m_FrameInfo.lBufPtr, m_FrameInfo.lBufSize);
		ptDlg->RAWFrameInfo.lBufPtr = ptDlg->m_pRAWData;
		ptDlg->m_bHookData = 1;
		SetEvent(ptDlg->hEventBuf);
	}
	else
	{
		//DumpInfo("Busy!!!Drop Roi Img %d",m_FrameInfo.lBufPtr[0]);
	}
	return 0;
}

DWORD WINAPI CameraCapture::Processfunc(LPVOID lpData)
{
	CameraCapture *ptDlg = (CameraCapture *)(lpData);
	MVCFRAMEINFO lRGBFrameInfo;
	memset(&lRGBFrameInfo, 0, sizeof(MVCFRAMEINFO));
	DWORD lbROISaveFlag = 0;
	while (TRUE)
	{
		if (WaitForSingleObject(ptDlg->hEventEnd, 0) == WAIT_OBJECT_0)//�ȴ��ر���ʾ�ź�
		{
			break;
		}
		if (WaitForSingleObject(ptDlg->hEventBuf, 1) == WAIT_OBJECT_0)//�ȴ�ͼ��Ԥ���ź�
		{
			if (ptDlg->m_bHookData )
			{
				//��ʾ����
				if (ptDlg->m_bHookDisp)
				{
				//	MVC_PixelConverter3(ptDlg->m_iDevIndex, &ptDlg->RAWFrameInfo, &ptDlg->CVTFrameInfo, ptDlg->m_bBayer3x3);
				//	MVC_SubDisplayDisp(ptDlg->m_iDevIndex, 0, ptDlg->RAWFrameInfo.lBufPtr, ptDlg->RAWFrameInfo.Width, ptDlg->RAWFrameInfo.Height);
				//	ptDlg->g_iDspNumber++;
				}
			//	MVC_PixelConverter(ptDlg->m_iDevIndex, &ptDlg->RAWFrameInfo, &ptDlg->CVTFrameInfo, ptDlg->m_bBayer3x3);
				//ptDlg->CaptureImage(ptDlg->RAWFrameInfo, ptDlg->m_iImgBit == 8 ? 0 : 1);
			//	char FileName[255];
			//	sprintf(FileName, "%2d.bmp", ptDlg->g_iCapNumber);
			//	MVC_SaveFile(ptDlg->m_iDevIndex, FILE_BMP, &ptDlg->RAWFrameInfo, FileName, FALSE, FALSE, 0, FALSE);
				if(ptDlg->m_bMatSaveOnce)
				{ 
					//ptDlg->CaptureImage(ptDlg->RAWFrameInfo, ptDlg->m_iImgBit == 8 ? 0 : 1);
					ptDlg->imgMat = Mat(ptDlg->m_iImgHeight, ptDlg->m_iImgWidth, CV_8UC1, ptDlg->RAWFrameInfo.lBufPtr);
					ptDlg->m_bMatSaveOnce--;
				}
		
				//�ͷ�Buffer��
				ptDlg->m_bHookData = 0;
			}
		}
		else if (ptDlg->m_bHookData)
		{
			//δ�ȵ�Event�ź�Ҳ�ͷ�Buffer������ֹ����
			ptDlg->m_bHookData = 0;
		}
	}
	return 0;
}
BOOL CameraCapture::CaptureImage(MVCFRAMEINFO lFrameInfo, BOOL l_bRaw)
{
	saveTimes++;
	sprintf(buffer_name, "RAWDATA\\%d-%d-BW.bmp", m_iDevIndex,saveTimes);
	sprintf(buffer_name2, "RAWDATA\\%d-%d-RAW.raw", m_iDevIndex, saveTimes);
	//����ͼ������
	if (l_bRaw == 0)
	{
	     MVC_SaveFile(m_iDevIndex, FILE_BMP, &lFrameInfo, buffer_name, 0, 1, 100, FALSE);
	}
	else if (l_bRaw == 1)
	{
		MVC_SaveFile(m_iDevIndex, FILE_RAW, &lFrameInfo, buffer_name2, 0, 1, 100, FALSE);
	}
	
	return TRUE;
}
void CameraCapture::ReInitDisp()
{
	MVC_SubDisplayClose(m_iDevIndex, 0);
	memset(&m_RectEx, 0, sizeof(RECT));
	MVC_SubDisplayInit(m_iDevIndex, 0, m_RectEx, m_Hwnd, m_bBayer3x3);
}

