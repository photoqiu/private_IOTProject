#pragma once

#include <opencv2/opencv.hpp>

using namespace std;

/////////////////////////////

#ifndef	 _HGBJDLLSTRUCT_
#define  _HGBJDLLSTRUCT_

typedef struct SteelPipeBooks
{
	float    diameter;       //ֱ��
	float    thickness;      //�ں�
	float    length;         //����
	float    speed;          //�ٶ�
	int      serial_number;  //���
	int      book_db_id;     //���ݿ�Id
	int*     datas;          //���㷨�ĸֹ����ݡ�

}Book;

typedef struct HGInputAB
{
	Book book;

	// Ԥ��������

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
#ifdef __cplusplus    //__cplusplus��cpp���Զ����һ����
extern "C" {
#endif

	//DLLHGBJALGDLL_API int test_add(int a, int b); //�Զ��庯��������

	DLLHGBJALGDLL_API void DetProcess(HGOutputAB &oup, HGInputAB inp);


	///////////////////////////////////


#ifdef __cplusplus    //__cplusplus��cpp���Զ����һ����
}
#endif