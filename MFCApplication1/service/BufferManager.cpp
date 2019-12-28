#include "BufferManager.h"


BufferManager::BufferManager(void)
{
}


BufferManager::~BufferManager(void)
{
}

std::string BufferManager::GetUnixTime() {
	time_t resultTimer = time(nullptr);
	long datetimer = (long)resultTimer;
	std::ostringstream os;
	os << datetimer;
	std::string result;
	std::istringstream is(os.str());
	is >> result;
	return result;
}

int BufferManager::WritePictureFile(const char* datas, const char* files, size_t flens)
{
	std::ofstream in(files, std::ios::ate | std::ios::out| std::ios::binary);
	in.write(datas, flens);
	in.close();
	return 1;
}

int BufferManager::WriteTextFile(const char* datas, const char* files, size_t flens)
{
	///////////////////////////////////////////////////// 这里要配置文件的字符集.
	std::ofstream in(files, std::ios::out | std::ios::ate);
	in.write(datas, flens);
	in.close();
	return 1;
}