#pragma once

///��������������--�����ǻ�����
typedef struct problem {
	char* messsage[254];
} PROBLEMDATA;

///�����Ǽ���еĴ�������--������ִ���쳣�ı���
typedef struct errors {
	char* messsage[254];
} ERRORDATA;

///������ÿһ�μ��Ľ������---��Ƶ
typedef struct crossvideo {
	char* filedatas[254];
} CROSSVIDEODATA;

///������ÿһ�μ��Ľ������---����
typedef struct crosslaser {
	char* filedatas[254];
} CROSSLASERDATA;

///������ÿһ�μ��ǰ���ļ������·�
typedef struct fileid {
	int filetype;//����0:json, 1:ini, 2:txt, 3:jpg, 4:bmp, 5:png
	char* filename[254];
	int isUsed[5]; //�����ļ����Ƿ�ɹ�5̨������ 0����ʶ����δ���1����ʶ�ɹ�
} MAKEFILEID;

///������config�ļ��·�
typedef struct systemsetup {
	int filetype; //����0:json, 1:ini, 2:txt
	char* filename[254];
	char* filepaths[254];
	int CallBackUsed[5]; //�����ļ����Ƿ�ɹ�5̨,������ϡ�5̨��������
} CONFIGFILES;

//Problem data
class DataTypeStruct
{
public:
	DataTypeStruct();
	~DataTypeStruct();
	/// ��ȡ���ݲ�Ӧ��
	int GetDataApply(int ctype);
	/// �������ݲ�Ӧ��
	void SetDataApply(int ctype, char* files);

private:
	int CreateSetupFile(char* files);
	int DoSetupFile(char* files);

};

