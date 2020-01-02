//Copyright(c) 2013, KEYENCE Corporation. All rights reserved.
#include "StdAfx.h"
#include "DataExport.h"


CDataExport::CDataExport(void)
{
}

CDataExport::~CDataExport(void)
{
}

/*
 Export profile data
 @param Profile data
 @param File name
 @param The number of the profiles to export
 @return Did the export succeed?(True:Success)
 */
BOOL CDataExport::ExportProfile(const PROFILE_DATA *profileData, CString strFileName, int nProfileCnt)
{
	CStringA strPathAnsi(strFileName);
	ofstream ofOutfile(strPathAnsi);

	if(ofOutfile.is_open() == FALSE)
	{
		AfxMessageBox(_T("Output file cannot open."), MB_ICONERROR | MB_OK);
		return FALSE;
	}

	int nDataCount = (int) profileData[nProfileCnt].m_profileInfo.byProfileCnt * (profileData[nProfileCnt].m_profileInfo.byEnvelope + 1);
	for(int i = 0; i < profileData[nProfileCnt].m_profileInfo.wProfDataCnt; i++)
	{
		int pos = profileData[nProfileCnt].m_profileInfo.lXStart + profileData[nProfileCnt].m_profileInfo.lXPitch * i;
		ofOutfile << pos << "\t";
		for(int j = 0; j < nDataCount; j++)
		{
			ofOutfile << profileData[nProfileCnt].m_pnProfileData[i + profileData[nProfileCnt].m_profileInfo.wProfDataCnt * j] << "\t";
		}
		ofOutfile << endl;
	}

	return TRUE;
}


BOOL CDataExport::InitProfileByDatas(int datas[], string filesName)
{
	return TRUE;
}

BOOL CDataExport::dirExists(const string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}

CString CDataExport::MakeCVSProfileName(int orders)
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




CString CDataExport::MakeProfileName(int orders)
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
	filename.Format(_T("E:\\logs\\%4d\\%02d\\%02d\\7A-8B\\"), sys.wYear, sys.wMonth, sys.wDay);
	if (!dirExists(filename.GetBuffer(0)))
	{
		bool flag = CreateDirectory(filename, NULL);
	}
	filename.Format(_T("E:\\logs\\%4d\\%02d\\%02d\\7A-8B\\%4d_%02d_%02d_%02d_%02d_Seconds_%02d_Milliseconds_%03d_week%1d_Laser_%d.log"), sys.wYear, sys.wMonth, sys.wDay, sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek, orders);
	return filename;
}

string CDataExport::toStringDatas(int datas)
{
	return to_string(long long(datas)) + "\t";
}

/*
 Export profile data
 @param Profile data
 @param File name
 @param The number of the profiles to export
 @return Did the export succeed?(True:Success)
 */
BOOL CDataExport::ExportProfileEx(const PROFILE_DATA *profileData, CString strFileName, int nProfileCnt)
{
	FILE *fp;
	string result_str = "";
	if ((fp = fopen((LPSTR)(LPCSTR)strFileName, "w")) == NULL) 
	{
		AfxMessageBox(_T("Output file cannot open."), MB_ICONERROR | MB_OK);
		return FALSE;
	}
	int nDataCount = (int) profileData[0].m_profileInfo.byProfileCnt * (profileData[0].m_profileInfo.byEnvelope + 1);
	int ProfileCnt = 0;
	for(int i = 0; i < nProfileCnt; i++)
	{
		for(int j = 0; j < nDataCount; j++)
		{
			ProfileCnt = profileData[i].m_profileInfo.wProfDataCnt;
			for(int k = 0; k < ProfileCnt; k++)
			{
				result_str += toStringDatas(profileData[i].m_pnProfileData[j * profileData[i].m_profileInfo.wProfDataCnt + k]);
			}
		}
		result_str += "\n";
	}
	fwrite(result_str.c_str(), 1, result_str.size(), fp);
	fclose(fp);
	return TRUE;
}

void CDataExport::saveToTxt(const PROFILE_DATA *m_vecProfileData, CString strFileName, int nProfileCnt)
{
    CStringA strPathAnsi(strFileName);
	ofstream ofOutfile(strPathAnsi);
	ofOutfile.open("result.csv");
    for (int i = 0; i < 500; i++)
	{
		for (int k = 0; k < 1200; k++)
		{
			ofOutfile << m_vecProfileData[i].m_pnProfileData[k] << ",";
		}
	}
	ofOutfile << endl;
	ofOutfile.close();
}

/*
 Export profile data
 @param Profile data
 @param File name
 @param The number of the profiles to export
 @return Did the export succeed?(True:Success)
 */
BOOL CDataExport::ExportDoProfileEx(const string profileData, CString strFileName, int nProfileCnt)
{
	FILE *fp;
	if ((fp = fopen((LPSTR)(LPCSTR)strFileName, "w")) == NULL)
	{
		AfxMessageBox(_T("Output file cannot open."), MB_ICONERROR | MB_OK);
		return FALSE;
	}
	fwrite(profileData.c_str(), 1, profileData.size(), fp);
	fclose(fp);
	return TRUE;
}

BOOL CDataExport::ExportDoProfileEx(const char *datas, CString strFileName, int nProfileCnt)
{
	FILE *fp;
	if ((fp = fopen((LPSTR)(LPCSTR)strFileName, "a+")) == NULL)
	{
		AfxMessageBox(_T("Output file cannot open."), MB_ICONERROR | MB_OK);
		return FALSE;
	}
	fputs(datas, fp);
	fclose(fp);
	return TRUE;
}

/*
 Export profile data
 @param Profile data
 @param File name
 @param The number of the profiles to export
 @return Did the export succeed?(True:Success)
 */
BOOL CDataExport::ExportProfileThreadEx(const PROFILE_DATA *profileData, CString strFileName, int nProfileCnt)
{
	FILE *fp;
	if ((fp = fopen((LPSTR)(LPCSTR)strFileName, "w")) == NULL)
	{
		AfxMessageBox(_T("Output file cannot open."), MB_ICONERROR | MB_OK);
		return FALSE;
	}
	int nDataCount = (int)profileData[0].m_profileInfo.byProfileCnt * (profileData[0].m_profileInfo.byEnvelope + 1);
	int ProfileCnt = 0;
	int m_couterFlow = 0;
	int* m_pnCntDatas;
	string datas;
	for (int i = 0; i < nProfileCnt; i++)
	{
		m_pnCntDatas = profileData[i].m_pnProfileData;
		for (int j = 0; j < nDataCount; j++)
		{
			ProfileCnt = profileData[i].m_profileInfo.wProfDataCnt;
			m_couterFlow = j * ProfileCnt;
			for (int k = 0; k < ProfileCnt; k++)
			{
				fprintf(fp, "%ld\t", m_pnCntDatas[m_couterFlow + k]);
				// cout << m_pnCntDatas[m_couterFlow + k] << endl;
				// fprintf(fp, "%ld\t", profileData[i].m_pnProfileData[j * profileData[i].m_profileInfo.wProfDataCnt + k]);
			}
		}
		fwrite("\n", 1, 1, fp);
	}
	fclose(fp);
	return TRUE;
}

BOOL CDataExport::ExportDoProfileEx(const PROFILE_DATA *profileData, CString strFileName, int nProfileCnt)
{
	CStringA strPathAnsi(strFileName);
	ofstream ofOutfile(strPathAnsi);
	if (ofOutfile.is_open() == FALSE)
	{
		AfxMessageBox(_T("Output file cannot open."), MB_ICONERROR | MB_OK);
		return FALSE;
	}
	int nDataCount = (int)profileData[0].m_profileInfo.byProfileCnt * (profileData[0].m_profileInfo.byEnvelope + 1);
	for (int i = 0; i < nProfileCnt; i++)
	{
		for (int j = 0; j < nDataCount; j++)
		{
			for (int k = 0; k < profileData[i].m_profileInfo.wProfDataCnt; k++)		// Profile Data
			{
				ofOutfile << profileData[i].m_pnProfileData[j * profileData[i].m_profileInfo.wProfDataCnt + k] << "\t";
			}
		}
		ofOutfile << endl;
	}
	return TRUE;
}

/*
 Export measure data
 @param Measure data
 @param File name
 @return Did the export succeed?(Trur:Success)
 */
BOOL CDataExport::ExportMeasureDatas(vector<MEASURE_DATA> &vecMeasureData, CString strfileName)
{
	CStringA strPathAnsi(strfileName);
	ofstream outfile(strPathAnsi);	
	if(outfile.is_open() == FALSE)
	{
		AfxMessageBox(_T("Output file cannot open."), MB_ICONERROR | MB_OK);
		return FALSE;
	}
	outfile.setf(ios_base::fixed, ios_base::floatfield);
	outfile.precision(4);			// display the result to 4 places of decimals	

	vector<MEASURE_DATA>::iterator ite = vecMeasureData.begin(); 
	while(ite != vecMeasureData.end())
	{
		for(int i=0; i< LJV7IF_OUT_COUNT; i++)
		{
			outfile << (ite->m_MeasureData[i].fValue) << "\t";
		}
		++ite;
		outfile << endl;
	}

	return TRUE;
}

