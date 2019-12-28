#pragma once

///这里是问题数据--这里是坏数据
typedef struct problem {
	char* messsage[254];
} PROBLEMDATA;

///这里是监控中的错误数据--这里是执行异常的报错
typedef struct errors {
	char* messsage[254];
} ERRORDATA;

///这里是每一次检测的结果数据---视频
typedef struct crossvideo {
	char* filedatas[254];
} CROSSVIDEODATA;

///这里是每一次检测的结果数据---激光
typedef struct crosslaser {
	char* filedatas[254];
} CROSSLASERDATA;

///这里是每一次检测前的文件名的下发
typedef struct fileid {
	int filetype;//这里0:json, 1:ini, 2:txt, 3:jpg, 4:bmp, 5:png
	char* filename[254];
	int isUsed[5]; //推送文件名是否成功5台服务器 0：标识推送未到达。1：标识成功
} MAKEFILEID;

///这里是config文件下发
typedef struct systemsetup {
	int filetype; //这里0:json, 1:ini, 2:txt
	char* filename[254];
	char* filepaths[254];
	int CallBackUsed[5]; //推送文件名是否成功5台,配置完毕。5台服务器。
} CONFIGFILES;

//Problem data
class DataTypeStruct
{
public:
	DataTypeStruct();
	~DataTypeStruct();
	/// 获取数据并应用
	int GetDataApply(int ctype);
	/// 设置数据并应用
	void SetDataApply(int ctype, char* files);

private:
	int CreateSetupFile(char* files);
	int DoSetupFile(char* files);

};

