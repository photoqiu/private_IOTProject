#include <Windows.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <codecvt>
#include <exdisp.h>
#include <comdef.h>
#include <ctime>
#include <sstream>
#include <fstream>
#include <io.h>
#include <math.h>
#include "base64.h"
#pragma once

#define FILENAMESIZE 254
#define BUFFSIZE 1024000
class BufferManager
{
public:
	BufferManager(void);
	~BufferManager(void);

public:
	static int WritePictureFile(const char* datas, const char* files, size_t flens);
	static int WriteTextFile(const char* datas, const char* files, size_t flens);
	std::string GetUnixTime();
};
