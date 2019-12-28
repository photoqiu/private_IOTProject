#include "stdafx.h"
#include "AIOperationDatas.h"


using namespace std;

static float WallThickness = 4;		//�ֹܱں�
static float PipeDiameter = 4;		//�ֹ�ֱ��
static float TubeLength = 4;		//�ֹܳ���
static float RunningSpeed = 1.2;  //�ֹ������ٶ�

AIOperationDatas::AIOperationDatas(void)
{
}

AIOperationDatas::~AIOperationDatas(void)
{
}

void AIOperationDatas::DataProcessing(int* pipe_data)
{
	Book PipeDatas;
	PipeDatas.diameter = WallThickness;
	PipeDatas.thickness = PipeDiameter;
	PipeDatas.length = TubeLength;
	PipeDatas.speed = RunningSpeed;
	PipeDatas.serial_number = 5;
	PipeDatas.book_db_id = 5;
	PipeDatas.datas = pipe_data;
	struct HGInputAB input_datas;
	input_datas.book = PipeDatas;
	struct HGOutputAB output_datas;
	DetProcess(output_datas, input_datas);
	AnalyticalDatas(output_datas);
}

void AIOperationDatas::AnalyticalDatas(HGOutputAB &output_datas)
{
	struct HGOutputSingle output_data_a;
	struct HGOutputSingle output_data_b;
	Book PipeDatas;
	output_data_a = output_datas.outA;
	output_data_b = output_datas.outB;
	PipeDatas = output_datas.book;
	if (output_data_a.nResType == 2) 
	{
		AfxMessageBox(_T("��ͷA����ȱ��"), MB_ICONERROR | MB_OK);
	}
	else if (output_data_a.nResType == -3) 
	{
		AfxMessageBox(_T("��ͷA��������"), MB_ICONERROR | MB_OK);
	}
	if (output_data_b.nResType == 2) 
	{
		AfxMessageBox(_T("��ͷB����ȱ��"), MB_ICONERROR | MB_OK);
	}
	else if (output_data_b.nResType == -3)
	{
		AfxMessageBox(_T("��ͷB��������"), MB_ICONERROR | MB_OK);
	}
}