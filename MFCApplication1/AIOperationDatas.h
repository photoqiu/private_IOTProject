#include <direct.h>
#include <io.h>
#include <vector>
#include <iostream>
#include <pthread.h>
#include <opencv2/opencv.hpp>
#include "DeviceData.h"
#include "./alg/DllHgbjAlg.h"

#pragma once
using namespace std;
using namespace cv;
class AIOperationDatas
{
public:
	AIOperationDatas(void);
	~AIOperationDatas(void);
	static void DataProcessing(int *buffers);
	static void ReadFileContents(string filename);
	static void DeleteFiles(string filename);
private:
	static void AnalyticalDatas(HGOutputAB output_datas);
	static void ExchangeDatas(int *buffers);
	static string MakeFileNames(string camera_types);
};

