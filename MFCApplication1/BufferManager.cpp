#include "stdafx.h"
#include "BufferManager.h"


BufferManager::BufferManager(void)
{
}


BufferManager::~BufferManager(void)
{
}

int BufferManager::WritePictureFile(const char* datas, const char* files, long size)
{
	std::ofstream in(files, std::ios::out | std::ios::binary);
	in.write(datas, size);
	in.close();
	return 1;
}

int BufferManager::WriteTextFile(const char* datas, const char* files)
{
	std::ofstream in(files, std::ios::out | std::ios::ate);
	long size = sizeof(datas);
	in.write(datas, size);
	in.close();
	return 1;
}

std::string BufferManager::ReadPictureFile(const char* files)
{
	long size;
	std::ifstream in(files, std::ios::in | std::ios::binary | std::ios::ate);
	size = in.tellg();
	in.seekg(0, std::ios::beg);
	ImageBuffer = new char[size];
	in.read(ImageBuffer, size);
	in.close();
	return ImageBuffer;
}

std::string BufferManager::ReadTextFile(const char* files)
{
	char* buffer;
	long size;
	std::ifstream in(files, std::ios::in | std::ios::ate);
	size = in.tellg();
	in.seekg(0, std::ios::beg);
	buffer = new char[size];
	in.read(buffer, size);
	std::string datastr(buffer);
	in.close();
	std::cout << "the complete file is in a buffer size:" << size << std::endl;
	delete[] buffer;
	return datastr;
}