#include <opencv2/opencv.hpp>
#include "./alg/DllHgbjAlg.h"

#pragma once

class AIOperationDatas
{
public:
	AIOperationDatas(void);
	~AIOperationDatas(void);
	static void DataProcessing(int* pipe_data);
private:
	static void AnalyticalDatas(HGOutputAB &output_datas);
};

