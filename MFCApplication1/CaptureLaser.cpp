#include "stdafx.h"
#include "CaptureLaser.h"

LJV7IF_PROFILE_INFO CaptureLaser::m_profileInfo;
LJV7IF_PROFILE_INFO CaptureLaser::m_aProfileInfo[LJV7IF_DEVICE_COUNT];
vector<PROFILE_DATA> g_vecProfileData;


CaptureLaser::CaptureLaser()
{
}


CaptureLaser::~CaptureLaser()
{
}

void CaptureLaser::ReceiveHighSpeedData(BYTE* pBuffer, DWORD dwSize, DWORD dwCount, DWORD dwNotify, DWORD dwUser)
{
	vector<PROFILE_DATA> vecProfileData;
	int nProfDataCnt = (dwSize - sizeof(LJV7IF_PROFILE_HEADER) - sizeof(LJV7IF_PROFILE_FOOTER)) / sizeof(DWORD);
	for (DWORD i = 0; i < dwCount; i++)
	{
		BYTE *pbyBlock = pBuffer + dwSize * i;
		LJV7IF_PROFILE_HEADER* pHeader = (LJV7IF_PROFILE_HEADER*)pbyBlock;
		int* pnProfileData = (int*)(pbyBlock + sizeof(LJV7IF_PROFILE_HEADER));
		LJV7IF_PROFILE_FOOTER* pFooter = (LJV7IF_PROFILE_FOOTER*)(pbyBlock + dwSize - sizeof(LJV7IF_PROFILE_FOOTER));
		vecProfileData.push_back(PROFILE_DATA(m_profileInfo, pHeader, pnProfileData, pFooter));
	}
	CThreadSafeBuffer* threadSafeBuf = CThreadSafeBuffer::getInstance();
	threadSafeBuf->Add(dwUser, vecProfileData, dwNotify);
}

/*
 High-speed data communication "Start" button clicked
*/
void CaptureLaser::BeginHighSpeedDataCommunication()
{
	LJV7IF_StopHighSpeedDataCommunication(m_xvCurrentDeviceID);
	LJV7IF_HighSpeedDataCommunicationFinalize(m_xvCurrentDeviceID);
	int nRc = LJV7IF_RC_OK;
	DWORD dwProfileCnt = m_xvProfileCnt;
	DWORD dwThreadId = m_xvCurrentDeviceID;
	nRc = LJV7IF_HighSpeedDataUsbCommunicationInitalize(m_xvCurrentDeviceID, ReceiveHighSpeedOnceData, dwProfileCnt, dwThreadId);
	if (!CheckReturnCode(nRc)) return;
	LJV7IF_HIGH_SPEED_PRE_START_REQ req;
	LJV7IF_PROFILE_INFO md_profileInfo;
	req.bySendPos = m_xvSendPos;
	nRc = LJV7IF_PreStartHighSpeedDataCommunication(m_xvCurrentDeviceID, &req, &md_profileInfo);
	if (!CheckReturnCode(nRc)) return;
	nRc = LJV7IF_StartHighSpeedDataCommunication(m_xvCurrentDeviceID);
	if (!CheckReturnCode(nRc)) return;
	m_xvReceiveProfileNum = 0;
}

LJV7IF_PROFILE_INFO CaptureLaser::getProFileInfoConfig()
{
	LJV7IF_PROFILE_INFO profileInfo;
	profileInfo.byProfileCnt = m_xvProfileCnt;
	profileInfo.byEnvelope = 0;
	profileInfo.wProfDataCnt = 1200;
	return profileInfo;
}

int CaptureLaser::initCommonSystems()
{
	int nRc = LJV7IF_Initialize();
	int nVersion = LJV7IF_GetVersion();
	
	m_xvIsCountOnly = true;
	for (int i = 0; i < LJV7IF_DEVICE_COUNT; i++)
	{
		m_aDeviceData[i].m_deviceStatus = CDeviceData::DEVICESTATUS_NoConnection;
		cout << m_aDeviceData[i].GetStatusString() << endl;
	}
	/////  初始化设备信息结束。一切归零，并检查版本。
	return 0;
}

LJV7IF_TIME CaptureLaser::GetTimeParameter()
{
	LJV7IF_TIME timeSystems;
	time_t now = time(0);
	tm *ltm = localtime(&now);
	m_xvYear = 2000 - (1900 + ltm->tm_year);
	m_xvMonth = 1 + ltm->tm_mon;
	m_xvDay = ltm->tm_mday;
	m_xvHour = ltm->tm_hour;
	m_xvMinute = ltm->tm_min;
	m_xvSecond = ltm->tm_sec;
	timeSystems.byYear = m_xvYear;
	timeSystems.byMonth = m_xvMonth;
	timeSystems.byDay = m_xvDay;
	timeSystems.byHour = m_xvHour;
	timeSystems.byMinute = m_xvMinute;
	timeSystems.bySecond = m_xvSecond;
	return timeSystems;
}

void CaptureLaser::Settime()
{
	LJV7IF_TIME time = GetTimeParameter();
	int nRc = LJV7IF_SetTime(m_xvCurrentDeviceID, &time);
}

/////  打开USB设备，并设置标记位置。
int CaptureLaser::initCommonDataUsbCommunicationInitalizeSystems()
{
	m_xvIsCountOnly = false;
	int usb_open_signal = LJV7IF_UsbOpen(m_xvCurrentDeviceID);
	CDeviceData::DEVICESTATUS status = (usb_open_signal == LJV7IF_RC_OK) ? CDeviceData::DEVICESTATUS_UsbFast : CDeviceData::DEVICESTATUS_NoConnection;
	m_aDeviceData[m_xvCurrentDeviceID].m_deviceStatus = status;
	return 0;
}

void CaptureLaser::CountProfileReceive(BYTE* pBuffer, DWORD dwSize, DWORD dwCount, DWORD dwNotify, DWORD dwUser)
{
	CThreadSafeBuffer* threadSafeBuf = CThreadSafeBuffer::getInstance();
	threadSafeBuf->AddCount(dwUser, dwCount, dwNotify);
}

void OutputDebugStringEx(const char *strOutputString, ...)
{
	va_list vlArgs = NULL;
	va_start(vlArgs, strOutputString);
	size_t nLen = _vscprintf(strOutputString, vlArgs) + 1;
	char *strBuffer = new char[nLen];
	_vsnprintf_s(strBuffer, nLen, nLen, strOutputString, vlArgs);
	va_end(vlArgs);
	OutputDebugStringA(strBuffer);
	delete[] strBuffer;
}

char* CaptureLaser::my_strncpy(char *dest, char *source)
{
	if (dest != 0 && source != 0)
	{
		int dlen = strlen(dest);
		int slen = strlen(source);
		if (dlen <= slen)
		{
			/* 如果目标字符串长度小于源字符串长度 */
			/* 就把源字符串按从前往后的顺序，完全覆盖在目标字符串上 */
			return strncpy(dest, source, dlen);
		}
		else
		{
			/* 如果目标字符串长度大于源字符串长度 */
			/* 就把源字符串按从前往后的顺序，覆盖在目标字符串上 */
			/* 保留目标字符串后面几位字符 */
			return strncpy(dest, source, slen);
		}
	}
	return 0;
}

void CaptureLaser::ReceiveHighSpeedOnceData(BYTE* pBuffer, DWORD dwSize, DWORD dwCount, DWORD dwNotify, DWORD dwUser)
{
	vector<PROFILE_DATA> vecProfileData;
	int nProfDataCnt = (dwSize - sizeof(LJV7IF_PROFILE_HEADER) - sizeof(LJV7IF_PROFILE_FOOTER)) / sizeof(DWORD) + 1;
	int nReceiveDataOnceSize = (sizeof(int) * nProfDataCnt);
	char *dataBuffers = new char[TotalSize];
	int *BufferInt = new int[TotalSize];
	for (DWORD i = 0; i < dwCount; i++)
	{
		char *TempBuffers = new char[nProfDataCnt];
		nReceiveDataOnceSize = nReceiveDataOnceSize * (i + 1);
		BYTE *pbyBlock = pBuffer + dwSize * i;
		LJV7IF_PROFILE_HEADER* pHeader = (LJV7IF_PROFILE_HEADER*)pbyBlock;
		int* pnProfileData = (int*)(pbyBlock + sizeof(LJV7IF_PROFILE_HEADER));
		LJV7IF_PROFILE_FOOTER* pFooter = (LJV7IF_PROFILE_FOOTER*)(pbyBlock + dwSize - sizeof(LJV7IF_PROFILE_FOOTER));
		vecProfileData.push_back(PROFILE_DATA(m_aProfileInfo[dwUser], pHeader, pnProfileData, pFooter));
		memcpy_s(TempBuffers, nProfDataCnt, pnProfileData, nProfDataCnt);
		snprintf(dataBuffers, nReceiveDataOnceSize, "%s%s", dataBuffers, TempBuffers);
		if (TempBuffers != NULL)
		{
			delete[] TempBuffers;
			TempBuffers = NULL;
		}
	}
	int val = atoi(dataBuffers);
	BufferInt = &val;
	AIOperationDatas::DataProcessing(BufferInt);
	if (dataBuffers != NULL) 
	{
		delete[] dataBuffers;
		dataBuffers = NULL;
	}
	if (BufferInt != NULL)
	{
		delete[] BufferInt;
		BufferInt = NULL;
	}
}

//void CaptureLaser::ReceiveHighSpeedOnceData(BYTE* pBuffer, DWORD dwSize, DWORD dwCount, DWORD dwNotify, DWORD dwUser)
//{
//	vector<PROFILE_DATA> vecProfileData;
//	int nProfDataCnt = (dwSize - sizeof(LJV7IF_PROFILE_HEADER) - sizeof(LJV7IF_PROFILE_FOOTER)) / sizeof(DWORD);
//	int nReceiveDataOnceSize = (sizeof(int) * nProfDataCnt);
//	// int *dataBuffers = new int[TotalSize];
//	int *dataNumbers[ROWNUMS];
//	for (DWORD i = 0; i < dwCount; i++)
//	{
//		BYTE *pbyBlock = pBuffer + dwSize * i;
//		LJV7IF_PROFILE_HEADER* pHeader = (LJV7IF_PROFILE_HEADER*)pbyBlock;
//		int* pnProfileData = (int*)(pbyBlock + sizeof(LJV7IF_PROFILE_HEADER));
//		LJV7IF_PROFILE_FOOTER* pFooter = (LJV7IF_PROFILE_FOOTER*)(pbyBlock + dwSize - sizeof(LJV7IF_PROFILE_FOOTER));
//		vecProfileData.push_back(PROFILE_DATA(m_aProfileInfo[dwUser], pHeader, pnProfileData, pFooter));
//		dataNumbers[i] = pnProfileData;
//	}
//	//dataBuffers = dataNumbers;
//	//memcpy_s(dataBuffers, TotalSize + 1200, dataNumbers, TotalSize);
//	cout << dataNumbers << endl;
//	//int *dataBuffers = dataNumbers;
//	//AIOperationDatas::DataProcessing(dataBuffers);
//	// delete[] dataBuffers;
//	// dataBuffers = NULL;
//	//CThreadSafeBuffer* threadSafeBuf = CThreadSafeBuffer::getInstance();
//	//threadSafeBuf->Add(dwUser, vecProfileData, dwNotify);
//}

/////  打开高速USB设备，并设置标记位置。
int CaptureLaser::initHighSpeedDataUsbCommunicationInitalizeSystems()
{
	m_aDeviceData[m_xvCurrentDeviceID].m_vecMeasureData.clear();
	m_aDeviceData[m_xvCurrentDeviceID].m_vecProfileData.clear();
	DWORD dwProfileCnt = m_xvCallbackFrequency;
	DWORD dwThreadId = m_xvCurrentDeviceID;
	int nRc = LJV7IF_HighSpeedDataUsbCommunicationInitalize(m_xvCurrentDeviceID, ReceiveHighSpeedOnceData, dwProfileCnt, dwThreadId);
	if (nRc == LJV7IF_RC_OK)
	{
		m_aDeviceData[m_xvCurrentDeviceID].m_deviceStatus = CDeviceData::DEVICESTATUS_UsbFast;
		return 1;
	}
	else 
	{
		nRc = LJV7IF_StopHighSpeedDataCommunication(m_xvCurrentDeviceID);
		nRc = LJV7IF_HighSpeedDataCommunicationFinalize(m_xvCurrentDeviceID);
	}
	return nRc;
}

LJV7IF_HIGH_SPEED_PRE_START_REQ CaptureLaser::getHighSpeedPreStartReq()
{
	LJV7IF_HIGH_SPEED_PRE_START_REQ req;
	req.bySendPos = m_xvSendPos;
	return req;
}

int CaptureLaser::initPrestarthighspeeddatacommunication() 
{
	LJV7IF_HIGH_SPEED_PRE_START_REQ req = getHighSpeedPreStartReq();
	LJV7IF_PROFILE_INFO info;
	int nRc = LJV7IF_PreStartHighSpeedDataCommunication(m_xvCurrentDeviceID, &req, &info);
	if (nRc == LJV7IF_RC_OK)
	{
		m_aProfileInfo[m_xvCurrentDeviceID] = info;
		return 1;
	}
	else 
	{
		nRc = LJV7IF_StopHighSpeedDataCommunication(m_xvCurrentDeviceID);
		nRc = LJV7IF_HighSpeedDataCommunicationFinalize(m_xvCurrentDeviceID);
	}
	return nRc;
}

void CaptureLaser::getbatchprofileadvance()
{
	m_vecMeasureData.clear();
	m_aDeviceData[m_xvCurrentDeviceID].m_vecMeasureData.clear();
	m_aDeviceData[m_xvCurrentDeviceID].m_vecProfileData.clear();
	// Setting of fuction
	LJV7IF_GET_BATCH_PROFILE_ADVANCE_REQ req = getBatchProfileAdvanceReq();
	LJV7IF_GET_BATCH_PROFILE_ADVANCE_RSP rsp;
	LJV7IF_PROFILE_INFO profileInfo;
	DWORD dwOneDataSize = GetOneProfileDataSize() + sizeof(LJV7IF_MEASURE_DATA) * LJV7IF_OUT_COUNT;
	DWORD dwAllDataSize = dwOneDataSize * req.byGetProfCnt;
	vector<int> vecProfileData(dwAllDataSize / sizeof(int));
	LJV7IF_MEASURE_DATA aMeasureData[LJV7IF_OUT_COUNT];
	LJV7IF_MEASURE_DATA aBatchMeasureData[LJV7IF_OUT_COUNT];
	// Send Command
	int nRc = LJV7IF_GetBatchProfileAdvance(m_xvCurrentDeviceID, &req, &rsp, &profileInfo, (DWORD*)&vecProfileData.at(0),
		dwAllDataSize, aBatchMeasureData, aMeasureData);
}

int CaptureLaser::GetOneProfileDataSize()
{
	// Get the Profile Count
	int nProfileCnt = m_xvProfileCnt;
	int nCompression = 1;
	UINT retBuffSize = 0;											// Buffer size (in units of bytes)
	int basicSize = nProfileCnt / (m_xvBinning + 1);				// Basic size
	basicSize /= nCompression;
	retBuffSize += (UINT)basicSize * (m_xvHeadNum == 0 ? 1U : 2U);	// Number of headers
	retBuffSize *= (m_xvIsEnvelope ? 2U : 1U);						// Envelope setting
	retBuffSize *= sizeof(UINT);									//in units of bytes
	retBuffSize += sizeof(LJV7IF_PROFILE_HEADER);					// Sizes of the header and footer structures
	retBuffSize += sizeof(LJV7IF_PROFILE_FOOTER);
	retBuffSize += MAX_PROFILE_COUNT;
	return retBuffSize;
}

/// 使用的时候需要初始化。还未做初始化设置
LJV7IF_GET_BATCH_PROFILE_ADVANCE_REQ CaptureLaser::getBatchProfileAdvanceReq()
{
	TCHAR* pszEnd = NULL;
	LJV7IF_GET_BATCH_PROFILE_ADVANCE_REQ req;
	req.byPosMode = (BYTE)(::_tcstol("00", &pszEnd, 16));
	req.dwGetBatchNo = 4294967;
	req.dwGetProfNo = 4294967;
	req.byGetProfCnt = m_xvProfileCnt;

	return req;
}
/// 使用的时候需要初始化。还未做初始化设置
LJV7IF_GET_BATCH_PROFILE_REQ CaptureLaser::GetBatchProfileReq()
{
	TCHAR* pszEnd = NULL;
	LJV7IF_GET_BATCH_PROFILE_REQ req;
	req.byTargetBank = (BYTE)(::_tcstol(m_xvTargetBank, &pszEnd, 16));
	req.byPosMode = (BYTE)(::_tcstol(m_xvPosMode, &pszEnd, 16));
	req.dwGetBatchNo = 0;
	req.dwGetProfNo = 0;
	req.byGetProfCnt = m_xvProfileCnt;
	req.byErase = m_xvEraseFlg;
	return req;
}

int CaptureLaser::resetControllers()
{
	int nRc = LJV7IF_RebootController(m_xvCurrentDeviceID);
	return nRc;
}

int CaptureLaser::setToFactorySettig()
{
	int nRc = LJV7IF_RetrunToFactorySetting(m_xvCurrentDeviceID);
	return nRc;
}

int CaptureLaser::GetStartHighSpeedDataCommunication() 
{
	int nRc = LJV7IF_RC_OK;
	LJV7IF_PROFILE_INFO info;
	LJV7IF_HIGH_SPEED_PRE_START_REQ req = getHighSpeedPreStartReq();
	CThreadSafeBuffer* threadSafeBuf;
	m_aDeviceData[m_xvCurrentDeviceID].m_vecMeasureData.clear();
	m_aDeviceData[m_xvCurrentDeviceID].m_vecProfileData.clear();
	nRc = LJV7IF_HighSpeedDataUsbCommunicationInitalize(m_xvCurrentDeviceID, ReceiveHighSpeedOnceData, m_xvProfileCnt, (UINT)m_xvCurrentDeviceID);
	if (!CheckReturnCode(nRc)) return -1;
	m_aDeviceData[m_xvCurrentDeviceID].m_deviceStatus = CDeviceData::DEVICESTATUS_UsbFast;
	nRc = LJV7IF_PreStartHighSpeedDataCommunication(m_xvCurrentDeviceID, &req, &info);
	if (!CheckReturnCode(nRc)) return -1;
	m_aProfileInfo[m_xvCurrentDeviceID] = info;
	threadSafeBuf = CThreadSafeBuffer::getInstance();
	threadSafeBuf->ClearBuffer(m_xvCurrentDeviceID);
	nRc = LJV7IF_StartHighSpeedDataCommunication(m_xvCurrentDeviceID);
	if (!CheckReturnCode(nRc)) return -1;
	return 0;
}

LJV7IF_GET_PROFILE_REQ CaptureLaser::GetProFileReq()
{
	TCHAR* pszEnd = NULL;
	LJV7IF_GET_PROFILE_REQ req;
	req.byTargetBank = (BYTE)PROFILEBANK_ACTIVE;
	req.byPosMode = (BYTE)PROFILEPOS_CURRENT;
	req.dwGetProfNo = m_xvProfileNo;
	req.byGetProfCnt = m_xvProfileCnt;
	req.byErase = m_xvEraseFlg;
	req.reserve[0] = 0;
	req.reserve[1] = 0;
	req.reserve2[0] = 0;
	req.reserve2[1] = 0;
	return req;
}

int CaptureLaser::GetProFileDatas()
{
	int nRc = LJV7IF_RC_OK;
	LJV7IF_GET_PROFILE_REQ req = GetProFileReq();
	LJV7IF_GET_PROFILE_RSP rsp;
	LJV7IF_PROFILE_INFO profileInfo;
	DWORD dwOneDataSize = GetOneProfileDataSize();
	DWORD dwAllDataSize = dwOneDataSize * req.byGetProfCnt;
	vector<int> vecProfileData(dwAllDataSize / sizeof(DWORD));
	nRc = LJV7IF_GetProfile(m_xvCurrentDeviceID, &req, &rsp, &profileInfo, (DWORD*)&vecProfileData.at(0), dwAllDataSize);
	if (!CheckReturnCode(nRc)) return -1;
	m_aDeviceData[m_xvCurrentDeviceID].m_vecMeasureData.clear();
	m_aDeviceData[m_xvCurrentDeviceID].m_vecProfileData.clear();
	int nDataUnitSize = (sizeof(LJV7IF_PROFILE_HEADER) + sizeof(int) * profileInfo.wProfDataCnt * profileInfo.byProfileCnt * (profileInfo.byEnvelope + 1) + sizeof(LJV7IF_PROFILE_FOOTER)) / sizeof(int);
	for (int i = 0; i < rsp.byGetProfCnt; i++)
	{
		int *pnBlock = &vecProfileData.at(nDataUnitSize * i);
		LJV7IF_PROFILE_HEADER* pHeader = (LJV7IF_PROFILE_HEADER*)pnBlock;
		int* pnProfileData = pnBlock + (sizeof(LJV7IF_PROFILE_HEADER) / sizeof(DWORD));
		LJV7IF_PROFILE_FOOTER* pFooter = (LJV7IF_PROFILE_FOOTER*)(pnProfileData + profileInfo.wProfDataCnt * profileInfo.byProfileCnt * (profileInfo.byEnvelope + 1));
		m_aDeviceData[m_xvCurrentDeviceID].m_vecProfileData.push_back(PROFILE_DATA(profileInfo, pHeader, pnProfileData, pFooter));
	}
	// CDataExport::ExportProfileEx(&(m_aDeviceData[m_xvCurrentDeviceID].m_vecProfileData.at(0)), m_fxvSaveFilePath, 0);
	return 0;
}

void CaptureLaser::setStopHighSpeedDataCommunication() 
{
	int nRc = LJV7IF_StopHighSpeedDataCommunication(m_xvCurrentDeviceID);
}

void CaptureLaser::setUsbHighSpeedDataCommunicationFinalize()
{
	int nRc = LJV7IF_HighSpeedDataCommunicationFinalize(m_xvCurrentDeviceID);
	m_aDeviceData[m_xvCurrentDeviceID].m_deviceStatus = CDeviceData::DEVICESTATUS_UsbFast;
}

int CaptureLaser::StopMeasure()
{
	int nRc = LJV7IF_StopMeasure(m_xvCurrentDeviceID);
	return nRc;
}

/* 清除缓存 */
int CaptureLaser::ClearMemory()
{
	CThreadSafeBuffer* threadSafeBuf = CThreadSafeBuffer::getInstance();
	threadSafeBuf->ClearBuffer(m_xvCurrentDeviceID);
	int nRc = LJV7IF_ClearMemory(m_xvCurrentDeviceID);
	m_aDeviceData[m_xvCurrentDeviceID].m_vecMeasureData.clear();
	m_aDeviceData[m_xvCurrentDeviceID].m_vecProfileData.clear();
	g_vecProfileData.clear();
	m_xvReceiveProfileNum = 0;
	m_saveProfileCnt = 0;
	fileCnt = 0;
	return nRc;
}

/* 设置软关闭。*/
int CaptureLaser::closeSystems()
{
	CThreadSafeBuffer* threadSafeBuf;
	int nRc = LJV7IF_RC_OK;
	threadSafeBuf = CThreadSafeBuffer::getInstance();
	threadSafeBuf->ClearBuffer(m_xvCurrentDeviceID);
	m_aDeviceData[m_xvCurrentDeviceID].m_vecMeasureData.clear();
	m_aDeviceData[m_xvCurrentDeviceID].m_vecProfileData.clear();
	nRc = LJV7IF_StopHighSpeedDataCommunication(m_xvCurrentDeviceID);
	nRc = LJV7IF_HighSpeedDataCommunicationFinalize(m_xvCurrentDeviceID);
	nRc = LJV7IF_CommClose(m_xvCurrentDeviceID);
	nRc = LJV7IF_Finalize();
	m_aDeviceData[m_xvCurrentDeviceID].m_deviceStatus = CDeviceData::DEVICESTATUS_NoConnection;
	return nRc;
}

BOOL CaptureLaser::CheckReturnCode(int nRc)
{
	if (nRc == LJV7IF_RC_OK) return true;
	return false;
}

int CaptureLaser::CheckMemoryAccess() 
{
	BYTE byBusy = 0;
	int nRc = LJV7IF_CheckMemoryAccess(m_xvCurrentDeviceID, &byBusy);
	if (nRc == LJV7IF_RC_OK)
	{
		if (byBusy != 0)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

int CaptureLaser::GetActiveProgram()
{
	BYTE progNo = 0;
	int nRc = LJV7IF_GetActiveProgram(m_xvCurrentDeviceID, &progNo);
	if (nRc == LJV7IF_RC_OK)
	{
		return progNo;
	}
	return -1;
}

void CaptureLaser::GetMeasurementValue()
{
	LJV7IF_MEASURE_DATA aMeasureData[LJV7IF_OUT_COUNT];
	int nRc = LJV7IF_GetMeasurementValue(m_xvCurrentDeviceID, aMeasureData);
	if (nRc == LJV7IF_RC_OK)
	{
		m_vecMeasureData.clear();
		m_vecMeasureData.push_back(aMeasureData);
	}
}

void CaptureLaser::ChechkFasterTimer()
{
	//UINT nTimerELASP = 200;
	//// Start Timer or Stop Timer
	//if (m_xvIsStartTimer)
	//{
	//	SetTimer(NULL, TIMER_ID, nTimerELASP);
	//}
	//else
	//{
	//	KillTimer(TIMER_ID);
	//}
}

CString CaptureLaser::GetMeasureDataInfo(LJV7IF_MEASURE_DATA measureData)
{
	CString strMeasureDataInfo;

	switch (measureData.byDataInfo)
	{
	case LJV7IF_MEASURE_DATA_INFO_VALID:
		strMeasureDataInfo = _T("Valid	");
		break;
	case LJV7IF_MEASURE_DATA_INFO_ALARM:
		strMeasureDataInfo = _T("Alarm value  ");
		break;
	case LJV7IF_MEASURE_DATA_INFO_WAIT:
		strMeasureDataInfo = _T("Judgment wait value  ");
		break;
	default:
		strMeasureDataInfo = _T("Unexpected value	");
		break;
	}

	switch (measureData.byJudge)
	{
	case 0:
		strMeasureDataInfo += _T("______  ");
		break;
	case LJV7IF_JUDGE_RESULT_HI:
		strMeasureDataInfo += _T("HI____  ");
		break;
	case LJV7IF_JUDGE_RESULT_GO:
		strMeasureDataInfo += _T("__GO__  ");
		break;
	case LJV7IF_JUDGE_RESULT_LO:
		strMeasureDataInfo += _T("____LO  ");
		break;
	case (LJV7IF_JUDGE_RESULT_HI | LJV7IF_JUDGE_RESULT_LO):
		strMeasureDataInfo += _T("HI__LO  ");
		break;
		case (int)(LJV7IF_JUDGE_RESULT_HI
			| LJV7IF_JUDGE_RESULT_GO
			| LJV7IF_JUDGE_RESULT_LO) :
			strMeasureDataInfo += _T("ALL BIT  ");
			break;
		default:
			strMeasureDataInfo += _T("UNEXPECTED ");
			break;
	}

	return strMeasureDataInfo;
}

int CaptureLaser::StartHighSpeedDataCommunication()
{
	int nRc = LJV7IF_StartHighSpeedDataCommunication(m_xvCurrentDeviceID);
	if (!CheckReturnCode(nRc)) 
	{
		nRc = LJV7IF_StopHighSpeedDataCommunication(m_xvCurrentDeviceID);
		nRc = LJV7IF_HighSpeedDataCommunicationFinalize(m_xvCurrentDeviceID);
		return nRc;
	} 
	return 1;
}

void CaptureLaser::ResetAllControllers()
{
	
}

void CaptureLaser::ResetDatasControllers()
{
	int nRc = LJV7IF_Reset(m_xvCurrentDeviceID, GetSelection());
}

void CaptureLaser::GetCollectionHighSpeedUsbDatas()
{
	LJV7IF_PROFILE_INFO profileInfo;
	LJV7IF_GET_BATCH_PROFILE_REQ req = GetBatchProfileReq();
	LJV7IF_GET_BATCH_PROFILE_RSP rsp;
	DWORD dwProfileDataSize = MAX_PROFILE_COUNT + (sizeof(LJV7IF_PROFILE_HEADER) + sizeof(LJV7IF_PROFILE_FOOTER)) / sizeof(DWORD);
	vector<int> vecProfileData(dwProfileDataSize);
	int nRc = LJV7IF_GetBatchProfile(DEVICE_ID, &req, &rsp, &profileInfo, (DWORD*)&vecProfileData.at(0), dwProfileDataSize * sizeof(int));
	if (!CheckReturnCode(nRc)) return;
	std::vector<PROFILE_DATA> vecReceiveProfileData;
	if (nRc == LJV7IF_RC_OK)
	{
		int *pnBlock = &vecProfileData.at(0);
		LJV7IF_PROFILE_HEADER* pHeader = (LJV7IF_PROFILE_HEADER*)pnBlock;
		int* pnProfileData = pnBlock + (sizeof(LJV7IF_PROFILE_HEADER) / sizeof(DWORD));
		LJV7IF_PROFILE_FOOTER* pFooter = (LJV7IF_PROFILE_FOOTER*)(pnProfileData + profileInfo.wProfDataCnt * profileInfo.byProfileCnt * (profileInfo.byEnvelope + 1));
		vecReceiveProfileData.push_back(PROFILE_DATA(profileInfo, pHeader, pnProfileData, pFooter));          // Store the profile data
		// CDataExport::ExportProfileEx(&(vecReceiveProfileData.at(0)), m_xvDataFilePath, rsp.byGetProfCnt);  // Profile output
	}
}

//void CaptureLaser::GetCollectionHighSpeedUsbDatas()
//{
//	// Setting of function
//	LJV7IF_PROFILE_INFO profileInfo;
//	DWORD dwProfileDataSize = MAX_PROFILE_COUNT + (sizeof(LJV7IF_PROFILE_HEADER) + sizeof(LJV7IF_PROFILE_FOOTER)) / sizeof(DWORD);
//	vector<int> vecProfileData(dwProfileDataSize);
//	LJV7IF_MEASURE_DATA aMeasureData[LJV7IF_OUT_COUNT];
//	int nRc = LJV7IF_GetProfileAdvance(DEVICE_ID, &profileInfo, (DWORD*)&vecProfileData.at(0), dwProfileDataSize * sizeof(int), aMeasureData);
//	//LJV7IF_GetBatchProfile(DEVICE_ID, &profileInfo, &rsp, &profileInfo, (DWORD*)&vecProfileData.at(0), dwAllDataSize);
//	if (!CheckReturnCode(nRc)) return;
//	std::vector<PROFILE_DATA> vecReceiveProfileData;
//	if (nRc == LJV7IF_RC_OK)
//	{
//		int *pnBlock = &vecProfileData.at(0);
//		LJV7IF_PROFILE_HEADER* pHeader = (LJV7IF_PROFILE_HEADER*)pnBlock;
//		int* pnProfileData = pnBlock + (sizeof(LJV7IF_PROFILE_HEADER) / sizeof(DWORD));
//		LJV7IF_PROFILE_FOOTER* pFooter = (LJV7IF_PROFILE_FOOTER*)(pnProfileData + profileInfo.wProfDataCnt * profileInfo.byProfileCnt * (profileInfo.byEnvelope + 1));
//		vecReceiveProfileData.push_back(PROFILE_DATA(profileInfo, pHeader, pnProfileData, pFooter));	// Store the profile data
//		CDataExport::ExportProfileEx(&(vecReceiveProfileData.at(0)), m_xvDataFilePath, 1);				// Profile output
//	}
//}

int CaptureLaser::GetSelection()
{
	int m_nSelection = 0;		// Initialize
	BOOL isAllReset = true;
	m_nSelection += (isAllReset) ? (unsigned int)1 : (unsigned int)0;
	m_nSelection += (isAllReset) ? (unsigned int)(1 << 1) : (unsigned int)0;
	m_nSelection += (isAllReset) ? (unsigned int)(1 << 2) : (unsigned int)0;
	m_nSelection += (isAllReset) ? (unsigned int)(1 << 3) : (unsigned int)0;
	m_nSelection += (isAllReset) ? (unsigned int)(1 << 4) : (unsigned int)0;
	m_nSelection += (isAllReset) ? (unsigned int)(1 << 5) : (unsigned int)0;
	m_nSelection += (isAllReset) ? (unsigned int)(1 << 6) : (unsigned int)0;
	m_nSelection += (isAllReset) ? (unsigned int)(1 << 7) : (unsigned int)0;
	m_nSelection += (isAllReset) ? (unsigned int)(1 << 8) : (unsigned int)0;
	m_nSelection += (isAllReset) ? (unsigned int)(1 << 9) : (unsigned int)0;
	m_nSelection += (isAllReset) ? (unsigned int)(1 << 10) : (unsigned int)0;
	m_nSelection += (isAllReset) ? (unsigned int)(1 << 11) : (unsigned int)0;
	m_nSelection += (isAllReset) ? (unsigned int)(1 << 12) : (unsigned int)0;
	m_nSelection += (isAllReset) ? (unsigned int)(1 << 13) : (unsigned int)0;
	m_nSelection += (isAllReset) ? (unsigned int)(1 << 14) : (unsigned int)0;
	m_nSelection += (isAllReset) ? (unsigned int)(1 << 15) : (unsigned int)0;
	return m_nSelection;
}


bool CaptureLaser::dirExists(const string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}

CString CaptureLaser::CreateDataLogFiles(int orders)
{
	CString filename;
	SYSTEMTIME sys;
	HANDLE hFile;
	char *tmpBuf;
	DWORD dwBytesWrite, dwBytesToWrite;
	dwBytesToWrite = 1;
	dwBytesWrite = 0;
	tmpBuf = "  ";
	GetLocalTime(&sys);
	filename.Format(_T("E:\\logs\\%4d\\"), sys.wYear);
	if (!dirExists(filename.GetBuffer(0)))
	{
		bool flag = CreateDirectory(filename, NULL);
	}
	filename.Format(_T("E:\\logs\\%4d\\%02d\\"), sys.wYear, sys.wMonth);
	if (!dirExists(filename.GetBuffer(0)))
	{
		bool flag = CreateDirectory(filename, NULL);
	}
	filename.Format(_T("E:\\logs\\%4d\\%02d\\%02d\\"), sys.wYear, sys.wMonth, sys.wDay);
	if (!dirExists(filename.GetBuffer(0)))
	{
		bool flag = CreateDirectory(filename, NULL);
	}
	filename.Format(_T("E:\\logs\\%4d\\%02d\\%02d\\5A-6B\\"), sys.wYear, sys.wMonth, sys.wDay);
	if (!dirExists(filename.GetBuffer(0)))
	{
		bool flag = CreateDirectory(filename, NULL);
	}
	filename.Format(_T("E:\\logs\\%4d\\%02d\\%02d\\5A-6B\\%4d_%02d_%02d_%02d_%02d_Seconds_%02d_Milliseconds_%03d_week%1d_Laser_%d.log"), sys.wYear, sys.wMonth, sys.wDay, sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek, orders);
	hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		if (!WriteFile(hFile, tmpBuf, dwBytesToWrite, &dwBytesWrite, NULL)) {
			CloseHandle(hFile);
			return filename;
		}
	}
	do { //循环写文件，确保完整的文件被写入  
		WriteFile(hFile, tmpBuf, dwBytesToWrite, &dwBytesWrite, NULL);
		dwBytesToWrite -= dwBytesWrite;
		tmpBuf += dwBytesWrite;
	} while (dwBytesToWrite > 0);
	CloseHandle(hFile);
	return filename;
}

int CaptureLaser::DoLaserInitUsbHighSpeedDatas()
{
	m_xvSendPos = 2;          //这里是配置文件选项
	m_xvIsCountOnly = false;  //这里是配置文件选项
	m_xvProfileNo = 0;      //这里是配置文件选项
	m_xvProfileCnt = 600;       //这里是配置文件选项
	m_xvDataCnt = 500;
	m_xvEraseFlg = 0;         //这里是配置文件选项
	m_xvTargetBank = "00";    //这里是配置文件选项
	m_xvPosMode = "00";       //这里是配置文件选项
	m_xvBinning = 0;
	m_xvReceiveProfileNum = 0;
	m_anProfReceiveCnt[m_xvCurrentDeviceID] = 0;
	s_FileNo[m_xvCurrentDeviceID] = 0;
	g_vecProfileData.clear();
	m_xvHeadNum = 0;
	m_xvIsEnvelope = true;
	f_data_index = 1;
///////////////////////////////////////////////////////////
	LJV7IF_StopHighSpeedDataCommunication(m_xvCurrentDeviceID);
	LJV7IF_HighSpeedDataCommunicationFinalize(m_xvCurrentDeviceID);
	int nRc = LJV7IF_RC_OK;
	nRc = LJV7IF_CommClose(m_xvCurrentDeviceID);
	if (!CheckReturnCode(nRc)) return -1;
	nRc = LJV7IF_Finalize();
	if (!CheckReturnCode(nRc)) return -1;
	nRc = LJV7IF_Initialize();
	if (!CheckReturnCode(nRc)) return -1;
	/////  初始化设备信息结束。一切归零，并检查版本。
	nRc = LJV7IF_UsbOpen(m_xvCurrentDeviceID);
	if (!CheckReturnCode(nRc)) return -1;
	CDeviceData::DEVICESTATUS status = (nRc == LJV7IF_RC_OK) ? CDeviceData::DEVICESTATUS_UsbFast : CDeviceData::DEVICESTATUS_NoConnection;
	m_aDeviceData[m_xvCurrentDeviceID].m_deviceStatus = status;
	cout << m_aDeviceData[m_xvCurrentDeviceID].GetStatusString() << endl;
	Settime();
	return 0;
}

int CaptureLaser::StartCommonSystems()
{
	ClearMemory();
	DoLaserInitUsbHighSpeedDatas();	
	initHighSpeedDataUsbCommunicationInitalizeSystems();
	initPrestarthighspeeddatacommunication();
	StartHighSpeedDataCommunication();
	return 0;
}

void GettingDataThread(void *arg)
{
	/*
	pthread_t thread1;
	pthread_t thread2;
	//初始化互斥锁变量
	if (pthread_mutex_init(&mutex, NULL) != 0)
	{
		cout<<"Init metux error."<<endl;
		exit(1);
	}
	//创建子线程1执行函数fun_thread1
	if (pthread_create(&thread1, NULL, &fun_thread1, NULL) != 0)
	{
		cout<<"Init thread1 error."<<endl;
		exit(1);
	}
	//创建子线程2执行函数fun_thread2
	if (pthread_create(&thread2, NULL,&fun_thread2, NULL) != 0)
	{
		cout<<"Init thread2 error."<<endl;
		exit(1);
	}
	/*pthread_join(thread1,NULL);
	pthread_join(thread2, NULL);
	pthread_exit(NULL);
	
	pthread_mutex_t mutex;
	int nBatchNo = 0;
	DWORD dwNotify = 0;
	pthread_mutex_lock(&mutex);
	vector<PROFILE_DATA> vecProfileData;
	CThreadSafeBuffer* threadSafeBuf = CThreadSafeBuffer::getInstance();
	threadSafeBuf->Get(0, &dwNotify, &nBatchNo, vecProfileData);
	if (vecProfileData.size() >= 1)
	{
		CString m_fSaveFilePath = CDataExport::MakeProfileName(0);
		CDataExport::ExportProfileThreadEx(&(vecProfileData.at(0)), m_fSaveFilePath, m_xvMainDataCnt);
	}
	Sleep(2);
	pthread_mutex_unlock(&mutex);
	return arg;
	*/
}

void CaptureLaser::KeepGettingData()
{
	return;
	/*
	pthread_t producter_t;
	pthread_mutex_init(&mutex, NULL);
	pthread_create(&producter_t, NULL, GettingDataThread, NULL);
	pthread_join(producter_t, NULL);
	pthread_mutex_destroy(&mutex);
	*/
}

int CaptureLaser::ReConnectionCommonSystems()
{
	HWND hWnd = ::GetActiveWindow();
	KillTimer(hWnd, TIMER_EX_ID);
	KillTimer(hWnd, TIMER_ID);
	ResetDatasControllers();
	BeginHighSpeedDataCommunication();
	GetCollectionHighSpeedUsbDatas();
	fileCnt = 0;
	return 0;
}

int CaptureLaser::PauseCommonSystems()
{
	int flag = CheckMemoryAccess();
	int nRc = LJV7IF_RC_OK;
	HWND hWnd = ::GetActiveWindow();
	KillTimer(hWnd, TIMER_EX_ID);
	if (flag == 1)
	{
		SetTimer( hWnd, TIMER_ID, TIMER_ELASP, NULL);
	}
	else
	{
		KillTimer(hWnd, TIMER_ID);
		nRc = LJV7IF_StopHighSpeedDataCommunication(m_xvCurrentDeviceID);
		nRc = LJV7IF_HighSpeedDataCommunicationFinalize(m_xvCurrentDeviceID);
		ClearMemory();
	}
	return 0;
}

int CaptureLaser::StopCommonSystems()
{
	int flag = CheckMemoryAccess();
	HWND hWnd = ::GetActiveWindow();
	KillTimer(hWnd, TIMER_ID);
	ClearMemory();
	closeSystems();
	return 0;
}