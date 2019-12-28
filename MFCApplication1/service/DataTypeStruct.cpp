#include "DataTypeStruct.h"



DataTypeStruct::DataTypeStruct()
{
}


DataTypeStruct::~DataTypeStruct()
{
}

//通过标识ctype，获取数据并应用
int DataTypeStruct::GetDataApply(int ctype)
{
	return 1;
}
//通过标识ctype，设置对应的文件类型
void DataTypeStruct::SetDataApply(int ctype, char* files)
{

}

//创建配置文件
int DataTypeStruct::CreateSetupFile(char* files)
{
	return 1;
}

//创建配置文件
int DataTypeStruct::DoSetupFile(char* files)
{
	return 1;
}
