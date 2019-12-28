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
class SocketClient
{
public:
	SocketClient();
	~SocketClient();

};

