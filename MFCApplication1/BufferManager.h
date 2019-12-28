#include <Windows.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <tchar.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#pragma once

#define FILENAMESIZE 254
#define BUFFSIZE 1024000

class BufferManager
{
public:
	BufferManager(void);
	~BufferManager(void);

public:
	int WritePictureFile(const char* datas, const char* files, long size);
	int WriteTextFile(const char* datas, const char* files);
	std::string ReadPictureFile(const char* files);
	std::string ReadTextFile(const char* files);
private :
	char* ImageBuffer;
	char* TextBuffer;

};
