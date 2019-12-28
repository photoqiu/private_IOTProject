#include <event2/event.h>
#include <event2/bufferevent.h>  
#include <event2/buffer.h>  
#include <fstream>
#include <iostream>
#include <windows.h>
#include <WinSock2.h>
#include <vector>
#include <string>
#include <stddef.h>
#include "base64.h"
#include "inifile.h"
#include "BufferManager.h"
using namespace std;
using namespace inifile;

#define AUTO_RECONNECT
#define INIFILEPATH "./setupfiles/client_config.ini"
#define FileBuffers 2048

#pragma once
void read_cb(struct bufferevent *bev, void *arg);
void write_service_cb(struct bufferevent *bev, void *arg);
void error_cb(struct bufferevent *bev, short event, void *arg);
DWORD WINAPI SendMsg(LPVOID p);
void write_cb(evutil_socket_t fd, short event, void *arg);

class ClientDatas
{
public:
	ClientDatas();
	~ClientDatas();
	void CreateClient();
private:
	void get_init_setup_file();
	void init_server_addr();	
};