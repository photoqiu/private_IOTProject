//Copyright(c) 2013, KEYENCE Corporation. All rights reserved.
#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <afxpriv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include "DeviceData.h"


using namespace std;


class CDataExport
{
public:
	CDataExport(void);
	~CDataExport(void);
	static BOOL InitProfileByDatas(int datas[], string filesName);
	static BOOL ExportMeasureDatas(vector<MEASURE_DATA> &vecMeasureData, CString strfileName);
	static BOOL ExportProfileEx(const PROFILE_DATA *profileData, CString  strFileName, int nProfileCnt);
	static BOOL ExportProfileThreadEx(const PROFILE_DATA *profileData, CString strFileName, int nProfileCnt);
	static BOOL ExportDoProfileEx(const string profileData, CString strFileName, int nProfileCnt);
	static BOOL ExportProfile(const PROFILE_DATA *profileData, CString strFileName, int nProfileCnt);
	static BOOL ExportDoProfileEx(const PROFILE_DATA *profileData, CString strFileName, int nProfileCnt);
	static BOOL ExportDoSaveProfileEx(char *datas, CString strFileName, int nProfileCnt);
	static CString MakeProfileName(int orders);
	static CString MakeCVSProfileName(int orders);
	static void saveToTxt(const PROFILE_DATA *m_vecProfileData, CString strFileName, int nProfileCnt);
private:
	static BOOL dirExists(const string& dirName_in);
	static string toStringDatas(int datas);
};
