
// MFCApplication1Dlg.h: 头文件
//
#include "CaptureLaser.h"
#include "ClientDatas.h"
#include "DBOperation.h"
#include <afxwin.h>
#include <pthread.h>
#include <cstdio>
#include "httpClient.h"
#include "DBOperation.h"
#include "afxcmn.h"

#pragma once

// CMFCApplication1Dlg 对话框
class CMFCApplication1Dlg : public CDialogEx
{
// 构造
public:
	CMFCApplication1Dlg(CWnd* pParent = nullptr);	// 标准构造函数
	static const int TIMER_EX_ID = 10;
	static const int TIMER_ELASP = 26;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION1_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();

private:
	CaptureLaser m_CaptrueLaser;
	httpClient m_httpClient;
	DBOperation db;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButton6();
};
