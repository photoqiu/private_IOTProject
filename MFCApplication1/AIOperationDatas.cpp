#include "stdafx.h"
#include "AIOperationDatas.h"


using namespace std;

static float WallThickness = 4;		//�ֹܱں�
static float PipeDiameter = 4;		//�ֹ�ֱ��
static float TubeLength = 4;		//�ֹܳ���
static float RunningSpeed = 1.2;  //�ֹ������ٶ�
const static int TotalSize = 500 * (1200 * sizeof(int));
const static int ROWNUMS = 500;
const static int COLUMNS = 1200;


AIOperationDatas::AIOperationDatas(void)
{
}

AIOperationDatas::~AIOperationDatas(void)
{
}

void AIOperationDatas::DataProcessing(int *buffers)
{
	Book PipeDatas;
	PipeDatas.diameter = WallThickness;
	PipeDatas.thickness = PipeDiameter;
	PipeDatas.length = TubeLength;
	PipeDatas.speed = RunningSpeed;
	PipeDatas.serial_number = 5;
	PipeDatas.book_db_id = 5;
	PipeDatas.datas = buffers;
	HGInputAB input_datas;
	input_datas.book = PipeDatas;
	HGOutputAB output_datas;
	DetProcess(output_datas, input_datas);
	AIOperationDatas::AnalyticalDatas(output_datas);
}

void AIOperationDatas::ExchangeDatas(int *buffers)
{
	Book PipeDatas;
	PipeDatas.diameter = WallThickness;
	PipeDatas.thickness = PipeDiameter;
	PipeDatas.length = TubeLength;
	PipeDatas.speed = RunningSpeed;
	PipeDatas.serial_number = 5;
	PipeDatas.book_db_id = 5;
	PipeDatas.datas = buffers;
	HGInputAB input_datas;
	input_datas.book = PipeDatas;
	HGOutputAB output_datas;
	DetProcess(output_datas, input_datas);
	AIOperationDatas::AnalyticalDatas(output_datas);
}

string AIOperationDatas::MakeFileNames(string camera_types)
{
	string folderPath = "E:\\cameras_pic_datas";
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	if (0 != _access(folderPath.c_str(), 2))
	{
		_mkdir(folderPath.c_str());   // ���� 0 ��ʾ�����ɹ���-1 ��ʾʧ��
	}
	folderPath = folderPath + "\\" + to_string(sys.wYear);
	if (0 != _access(folderPath.c_str(), 2))
	{
		_mkdir(folderPath.c_str());   // ���� 0 ��ʾ�����ɹ���-1 ��ʾʧ��
	}
	folderPath = folderPath + "\\" + to_string(sys.wMonth);
	if (0 != _access(folderPath.c_str(), 2))
	{
		_mkdir(folderPath.c_str());   // ���� 0 ��ʾ�����ɹ���-1 ��ʾʧ��
	}
	folderPath = folderPath + "\\" + to_string(sys.wDay) + "\\";
	if (0 != _access(folderPath.c_str(), 2))
	{
		_mkdir(folderPath.c_str());   // ���� 0 ��ʾ�����ɹ���-1 ��ʾʧ��
	}
	folderPath += to_string(sys.wYear) + to_string(sys.wMonth) + to_string(sys.wDay) + to_string(sys.wHour) + to_string(sys.wMinute) + to_string(sys.wSecond) + to_string(sys.wMilliseconds) + "_" + camera_types + ".jpg";
	return folderPath;
}

void AIOperationDatas::AnalyticalDatas(HGOutputAB output_datas)
{
	struct HGOutputSingle output_data_a;
	struct HGOutputSingle output_data_b;
	Book PipeDatas;
	PipeDatas = output_datas.book;      ///�ֹ���Ϣ�ش�
	output_data_a = output_datas.outA;  ///��ͷA�ĸֹ�����
	output_data_b = output_datas.outB;  ///��ͷB�ĸֹ�����
	PipeDatas = output_datas.book;
	Mat aimg = output_data_a.resImage;
	Mat bimg = output_data_b.resImage;
	string strFileNameA = AIOperationDatas::MakeFileNames("A");
	string strFileNameB = AIOperationDatas::MakeFileNames("B");
	if (!aimg.empty())
	{
		imshow("��ͷA����ȱ��", aimg);
		imwrite(strFileNameA, aimg);
		waitKey(0);
	}
	if (!bimg.empty())
	{
		imshow("��ͷB����ȱ��", bimg);
		imwrite(strFileNameB, bimg);
		waitKey(0);
	}
	if (output_data_a.nResType == 2) 
	{
		AfxMessageBox(_T("��ͷA����ȱ��"), MB_ICONERROR | MB_OK);
		//����Ҫд�����ݿ�
		//����Ҫ�ύͼƬ����������
	}
	else if (output_data_a.nResType == -3) 
	{
		AfxMessageBox(_T("��ͷA��������"), MB_ICONERROR | MB_OK);
	}
	if (output_data_b.nResType == 2) 
	{
		AfxMessageBox(_T("��ͷB����ȱ��"), MB_ICONERROR | MB_OK);
		//����Ҫд�����ݿ�
		//����Ҫ�ύͼƬ����������
	}
	else if (output_data_b.nResType == -3)
	{
		AfxMessageBox(_T("��ͷB��������"), MB_ICONERROR | MB_OK);
	}
}