#include <event2/event.h>
#include <event2/bufferevent.h>  
#include <event2/buffer.h>  
#include <event2/util.h>
#include <iostream>
#include <windows.h>
#include <process.h>
#include <WinSock2.h>
#include <vector>
#include <event2/listener.h>
#include "base64.h"
#include "BufferManager.h"


typedef struct Service 
{
	int port;
	int buffers;
	char* hosts;
	char fileType[4]; //ini jpg png bmp json log
} CONFIGSERVICE;

typedef struct DatasInfo
{
	char szFileName[254];
	long nFileSize;

} DATASINFO;

#define Ports 81123
#define HostsName 127.0.0.1
#define UPLOADERIMAGEPATH "D:\\uploaded\\"
#define CharsBuffers 4096
#define FileBuffers 4095

#pragma once

using namespace std;

class  _clientInfo
{
public:
	_clientInfo() {}
	_clientInfo(int fd, struct bufferevent * _bev) :sock_fd(fd), m_pBev(_bev), bLogin(true) {}
	evutil_socket_t sock_fd;
	struct bufferevent * m_pBev;
	bool bLogin;//������Ҫ��½�����ʱ����Ĭ�ϴ���Ϊfalse
};

class DoService
{
public:
	DoService();
	
	SOCKET getFd() { return sockFd; }
	void setSendAll(bool b) { m_bSendAll = b; }
	size_t getCurrentClinetNum() { return client_vec.size(); }
	void event_loop();
	~DoService();

private:
	SOCKET sockFd;
	static vector<_clientInfo> client_vec;
	static bool m_bSendAll;
	struct event_base* base_ev; //����һ��event_base ,��Ҫ�����������ʵ���¼��ļ���ѭ����һ�������һ���߳�һ��event_base������̵߳��������Ҫ�����event_base��
	static bool bTransFile;//ͼƬ�ļ�����
	static bool iTransFile;//�����ļ�.ini����
	static bool dTransFile;//ͼƬ�ļ�.json����
	static string fileData;
	static string currentTransferFileName;
	static string FormatCharDatas(string datas, const char flag);
	bool initSock();
	static void paraseHead(const string& src, string& msgType, string& name, int *startPos);
	static string getImagePath(const string& fileName);
	static void sendAll(char *msg);
	static void read_cb(struct bufferevent* bev, void *arg);
	static void error_cb(struct bufferevent *bev, short event, void *arg);
	static void do_accept(evutil_socket_t fd, short event, void *arg);
	static void HandleTXTMsg(struct bufferevent *bev, int fd, char *msg, int len);
};