#include <string>
#include <iostream>
#include <time.h>
#include <sys/timeb.h>
#include <fstream>
#include <exdisp.h>
#include <comdef.h>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <io.h>
#include <vector>
#include <afxwin.h>
#include <afx.h>
#include <algorithm>
#include <iterator>
#include <Windows.h>
#include <pthread.h>
#include <limits.h>
#include <sched.h>
#include <tchar.h>
#include "DeviceData.h"
#include "stdafx.h"
#include "LJV7_IF.h"
#include "LJV7_ErrorCode.h"
#include "DeviceData.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "ThreadSafeBuffer.h"
#include "DataExport.h"
#include "AIOperationDatas.h"
#include "Define.h"
#pragma once
#define _CRT_SECURE_NO_DEPRECATE
#define NUM_THREADS 56 //线程数
#define m_xvCallbackFrequency 500
using namespace std;
static const int m_xvMainDataCnt = 500;
const static int ROWNUMS = 500;
const static int COLUMNS = 1200;
const static int TotalSize = (500 * (1200 * sizeof(int))) + 500;

class CaptureLaser
{
public:
	CaptureLaser();
	~CaptureLaser();
	/* 自定义系统过程---开始 */
	/// 快速链接
	void KeepGettingData();
	int DoLaserInitUsbHighSpeedDatas();
	int StartCommonSystems();
	int ReConnectionCommonSystems();
	int PauseCommonSystems();
	int StopCommonSystems();
	/* 自定义系统过程---结束 */
private:
	int fileCnt = 0;
	int m_xvDataCnt = 500;
	static const int TIMER_ID = 100;        ////关闭or启动暂停设备，时间ID
	static const int TIMER_EX_ID = 200;     ////关闭or启动收集数据，使用ID
	static const int TIMER_ELASP = 150;
	static const int FLIE_COUNT_DATA = 500;
	static const int MAX_PROFILE_COUNT = 2400; /// 这里设定缓存的帧数，防止丢帧。600 * （2 帧）
	static const int DEVICE_ID = 0;
	static const int WRITE_DATA_SIZE = 500;
	static const int STORAGE_INFO_STATUS_EMPTY = 0;
	static const int STORAGE_INFO_STATUS_STORING = 1;
	static const int STORAGE_INFO_STATUS_FINISHED = 2;
	
	static LJV7IF_PROFILE_INFO m_profileInfo;
	vector<MEASURE_DATA> m_vecMeasureData;
	
	static LJV7IF_PROFILE_INFO m_aProfileInfo[LJV7IF_DEVICE_COUNT];
	int m_xvReceiveProfileNum;
	int m_saveProfileCnt;
	CString m_xvTargetBank;
	CString m_xvPosMode;
	DWORD m_xvBatchNo;
	DWORD m_xvProfileNo;
	BYTE m_xvProfileCnt;
	BYTE m_xvEraseFlg;
	CString m_fxvSaveFilePath;
	CString m_sxvSaveFilePath;
	int f_data_index;
	int m_xvCurrentDeviceID;
	int m_xvHeadNum;
	int m_xvXRange;
	int m_xvBinning;
	int m_xvXCompression;
	BOOL m_xvIsEnvelope;
	BOOL m_xvIsCountOnly;
	BOOL m_xvIsStartTimer;
	CString m_strCommandLog;
	CString m_strSaveFilePath;
	int m_xvSendPos;
	UINT m_xvYear;
	UINT m_xvMonth;
	UINT m_xvDay;
	UINT m_xvHour;
	UINT m_xvMinute;
	UINT m_xvSecond;
	CDeviceData m_aDeviceData[LJV7IF_DEVICE_COUNT];
	int m_anProfReceiveCnt[LJV7IF_DEVICE_COUNT];
	int m_writeProfileCnt[LJV7IF_DEVICE_COUNT];
	int s_FileNo[LJV7IF_DEVICE_COUNT];
	// 收到内容Profile.
	static void CountProfileReceive(BYTE* pBuffer, DWORD dwSize, DWORD dwCount, DWORD dwNotify, DWORD dwUser);
	static void ReceiveHighSpeedOnceData(BYTE* pBuffer, DWORD dwSize, DWORD dwCount, DWORD dwNotify, DWORD dwUser);
	static char* my_strncpy(char *dest, char *source);
	void Settime();
	int initCommonSystems();
	LJV7IF_PROFILE_INFO getProFileInfoConfig();
	int initCommonDataUsbCommunicationInitalizeSystems();
	int initHighSpeedDataUsbCommunicationInitalizeSystems();
	void GetCollectionHighSpeedUsbDatas();
	void BeginHighSpeedDataCommunication();
	int initPrestarthighspeeddatacommunication();
	int StartHighSpeedDataCommunication();

	int GetStartHighSpeedDataCommunication();
	int GetProFileDatas();
	CString CreateDataLogFiles(int orders);
	
	void setStopHighSpeedDataCommunication();
	
	void setUsbHighSpeedDataCommunicationFinalize();
	int CheckMemoryAccess();
	int GetActiveProgram();
	int GetSelection();
	void ResetAllControllers();
	void ResetDatasControllers();
	void GetMeasurementValue();
	void ReceiveHighSpeedData(BYTE* pBuffer, DWORD dwSize, DWORD dwCount, DWORD dwNotify, DWORD dwUser);
	LJV7IF_GET_PROFILE_REQ GetProFileReq();
	LJV7IF_GET_BATCH_PROFILE_ADVANCE_REQ getBatchProfileAdvanceReq();
	LJV7IF_HIGH_SPEED_PRE_START_REQ getHighSpeedPreStartReq();
	LJV7IF_GET_BATCH_PROFILE_REQ GetBatchProfileReq();
	LJV7IF_TIME GetTimeParameter();
	CString GetMeasureDataInfo(LJV7IF_MEASURE_DATA measureData);
	int closeSystems();
	int StopMeasure();
	int ClearMemory();
	int GetOneProfileDataSize();
	int resetControllers();
	int setToFactorySettig();
	void getbatchprofileadvance();
	void ChechkFasterTimer();
	//////////////这里是checkCode做联合调用
	BOOL CheckReturnCode(int nRc);
	bool dirExists(const string& dirName_in);
	
};