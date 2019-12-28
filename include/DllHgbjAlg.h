#pragma once

#include <opencv2/opencv.hpp>

using namespace std;

/////////////////////////////

#ifndef	 _HGBJDLLSTRUCT_
#define  _HGBJDLLSTRUCT_

typedef struct SteelPipeBooks
{
	float    diameter;       //直径
	float    thickness;      //壁厚
	float    length;         //长度
	float    speed;          //速度
	int      serial_number;  //编号
	int      book_db_id;     //数据库Id
	int*     datas;          //给算法的钢管数据。

}Book;

typedef struct HGInputAB
{
	Book book;

	// 预留参数：

};

typedef struct HGOutputSingle{
	//GGDetResType resDet;
	int nResType;
	cv::Mat resImage;
	int nResGrad[10];
};

typedef struct HGOutputAB{
	Book book;
	HGOutputSingle outA;
	HGOutputSingle outB;
};


#endif

/////////////////////////

#ifdef DLLHGBJALG_EXPORTS
#define DLLHGBJALGDLL_API __declspec(dllexport)
#else
#define DLLHGBJALGDLL_API __declspec(dllimport)
#endif // DLLDEMODLL_EXPORTS
#ifdef __cplusplus    //__cplusplus是cpp中自定义的一个宏
extern "C" {
#endif

	//DLLHGBJALGDLL_API int test_add(int a, int b); //自定义函数的声明

	DLLHGBJALGDLL_API void DetProcess(HGOutputAB &oup, HGInputAB inp);


	///////////////////////////////////


#ifdef __cplusplus    //__cplusplus是cpp中自定义的一个宏
}
#endif