#include <cstdlib>
#include <cerrno>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstdio>
#include "NetworkClient.h"
#include "Core/Utils/CoreUtils.h"

using namespace std;
const static int NUMT=4;
#pragma once

class httpClient
{
public:
	httpClient();
	~httpClient();
	int PostDatas();
	void GetDatas();
	void PostByRawData();
	void DownloadFile();
	void UploaderFile();
private:
	NetworkClient nc;
};

