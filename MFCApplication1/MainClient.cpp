#include "stdafx.h"
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

struct sockaddr_in server_addr;
vector<string>writeQueue;
string ServiceIp;
int ServicePorts;
typedef struct DatasInfo
{
	char* szFileBuffers;
	long nFileSize;

} DATASINFO;

void get_init_setup_file()
{
	IniFile inifiles;
	inifiles.load(INIFILEPATH);
	int ret = 0;
	ServiceIp = inifiles.getStringValue("client_section", "client_host", ret);
	ServicePorts = inifiles.getIntValue("client_section", "client_Ports", ret);
}

void init_server_addr()
{
#ifdef WIN32
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
	const char *Iptr = ServiceIp.c_str();
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(ServicePorts);
	int status = inet_pton(AF_INET, Iptr, &server_addr.sin_addr);
}

void read_cb(struct bufferevent *bev, void *arg)
{
	evutil_socket_t fd = bufferevent_getfd(bev);
	char msg[FileBuffers];
	size_t len = bufferevent_read(bev, msg, sizeof(msg) - 1);
	msg[len] = '\0';
	printf("fd=%u, read line: %s\n", fd, msg);
}

void write_service_cb(struct bufferevent *bev, void *arg)
{
	cout << "write_service_cb!!!!" << endl;
}

//事件回调  
void error_cb(struct bufferevent *bev, short event, void *arg) {
	//
	// BEV_EVENT_READING 在 bufferevent 上进行读取操作时出现了一个事件
	// BEV_EVENT_WRITING 在 bufferevent 上进行写入操作时出现了一个事件
	// BEV_EVENT_ERROR 进行 bufferevent 操作时出错
	// BEV_EVENT_TIMEOUT 在 bufferevent 上出现了超时
	// BEV_EVENT_EOF 在 bufferevent 上遇到了文件结束符，连接断开
	// BEV_EVENT_CONNECTED 在 bufferevent 上请求连接完成了
	evutil_socket_t fd = bufferevent_getfd(bev);
	if (event & BEV_EVENT_TIMEOUT)
	{
		printf("Timed out\n");
		bufferevent_free(bev);
	}
	else if (event & BEV_EVENT_EOF)
	{
		printf("connection closed\n");
		bufferevent_free(bev);
	}
	else if (event & BEV_EVENT_ERROR)
	{
		printf("some other error\n");

#ifdef AUTO_RECONNECT
		closesocket(fd);
		//如果断开了则主动重连	
		bool b_reconnect = false;
		int timeOut = 10;
		SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
		while (timeOut--)
		{
			//此while内除了Sleep，其他还会消耗肉眼可见的一点时间延迟
			int n = connect(sock, (sockaddr*)&server_addr, sizeof(server_addr));
			if (n == 0)
			{
				cout << "reconnect success!!" << endl;
				bufferevent_setfd(bev, sock);
				//此时要重新添加事件
				bufferevent_setcb(bev, read_cb, NULL, error_cb, NULL);
				bufferevent_enable(bev, EV_READ | EV_WRITE | EV_PERSIST);
				b_reconnect = true;
				break;
			}
			else
			{
				cout << "Err " << GetLastError() << endl;
			}
			Sleep(500);
		}
		if (!b_reconnect)
		{
			cout << "reconnect failed" << endl;
			bufferevent_free(bev);
		}
#else
		bufferevent_free(bev);
#endif
	}
}

DWORD WINAPI SendMsg(LPVOID p)
{
	//struct bufferevent * bev = (struct bufferevent *)p;
	string str;
	while (1)
	{
		cin >> str;
		//int n = bufferevent_write(bev, str, strlen(str) + 1);
		writeQueue.push_back(str);
		//cout << "write " << n << endl;
	}
	return 0;
}

string getPictureFileName(string path)
{
	int pos = path.rfind("/");
	if (pos != string::npos)
	{
		return path.substr(pos + 1) + string("#");
	}
	return string("noname#");
}

string getSetupFileName(string path)
{
	int pos = path.rfind("/");
	if (pos != string::npos)
	{
		return path.substr(pos + 1) + string("$");
	}
	return string("noname$");
}

string getDatasFileName(string path)
{
	int pos = path.rfind("/");
	if (pos != string::npos)
	{
		return path.substr(pos + 1) + string("*");
	}
	return string("noname*");
}

DATASINFO ReadPictureFile(const char* files)
{
	long size;
	char* buffers;
	DATASINFO datainfo;
	ifstream in(files, ios::in | ios::binary);
	in.seekg(0, ios::end);
	size = in.tellg();
	in.seekg(0, ios::beg);
	buffers = new char[size];
	in.read(buffers, size);
	string encoded = base64_encode(reinterpret_cast<const unsigned char*>(buffers), size);
	datainfo.szFileBuffers = _strdup(encoded.c_str());
	datainfo.nFileSize = size;
	in.close();
	delete[] buffers;
	buffers = NULL;
	return datainfo;
}

DATASINFO ReadTextFile(const char* files)
{
	char* buffer;
	DATASINFO datainfo;
	long size;
	ifstream in(files, ios::in);
	in.seekg(0, ios::end);
	size = in.tellg();
	in.seekg(0, ios::beg);
	buffer = new char[size];
	datainfo.szFileBuffers = new char[size];
	in.read(buffer, size);
	in.close();
	datainfo.szFileBuffers = _strdup(buffer);
	datainfo.nFileSize = size;
	delete[] buffer;
	buffer = NULL;
	return datainfo;
}

string CombinStringToChars(string charDatas, const char* combinDatas)
{
	string result = charDatas + string(combinDatas);
	return result;
}

void write_cb(evutil_socket_t fd, short event, void *arg)
{
	// 如果一个event监听了可写事件，那么这个event就会一直被触发（死循环）。
	// 因为一般情况下，如果不是发大量的数据这个写缓冲区是不会满的。
	struct bufferevent* bev = (struct bufferevent*)arg;
	BufferManager buffer;
	if (!writeQueue.empty())
	{
		const char *msg = writeQueue.front().c_str();
		char path[MAX_PATH] = { 0 };
		DATASINFO fileData;
		string name = "";
		string FileOnDatas = "";
		size_t FileBufferSize = 0;
		if (msg[0] == '#')//输入文件路径时以#开始 ---- 图片传输
		{
			sscanf(msg, "#%s", path);
			fileData = ReadPictureFile(path);
			//文件头格式为  file+文件名+编码后的数据
			name = getPictureFileName(path);
			FileOnDatas = string("file") + name;
			FileBufferSize = FileOnDatas.length();
			string ImageBuffer = CombinStringToChars(FileOnDatas, fileData.szFileBuffers);
			cout << "the file data counter:" << ImageBuffer.length() << endl;
			cout << "FileBufferSize counter:" << FileBufferSize << endl;
			bufferevent_write(bev, ImageBuffer.c_str(), FileBufferSize + ImageBuffer.length());
		}
		else if (msg[0] == '$')//输入文件路径时以#开始 ---- 配置.ini文件传输
		{
			sscanf(msg, "$%s", path);
			fileData = ReadTextFile(path);
			//文件头格式为  file+文件名+编码后的数据
			name = getSetupFileName(path);
			FileBufferSize = fileData.nFileSize + 4 + name.length();
			FileOnDatas = string("iile") + getSetupFileName(path);
			string IniBuffers = CombinStringToChars(FileOnDatas, fileData.szFileBuffers);
			bufferevent_write(bev, IniBuffers.c_str(), FileBufferSize);
			printf("传送配置文件");
		}
		else if (msg[0] == '*')//输入文件路径时以#开始 ---- 数据.json文件传输
		{
			sscanf(msg, "*%s", path);
			fileData = ReadTextFile(path);
			//文件头格式为  file+文件名+编码后的数据
			name = getDatasFileName(path);
			FileOnDatas = string("dile") + getDatasFileName(path);
			FileBufferSize = fileData.nFileSize + FileOnDatas.length();
			string JsonBuffers = CombinStringToChars(FileOnDatas, fileData.szFileBuffers);
			bufferevent_write(bev, JsonBuffers.c_str(), FileBufferSize);
			printf("传送数据文件");
		}
		else
		{//把终端的消息发送给服务器端
			bufferevent_write(bev, msg, strlen(msg));
			printf("传送消息文本");
		}
		//删除头
		writeQueue.erase(writeQueue.begin());
	}
}

int getFileSize()
{

	return -1;
}

void sendFile()
{
	printf("send file data.");
}

//int main(int argc, char* argv[])
//{
//	get_init_setup_file();
//	init_server_addr();
//	struct event_base *base_ev;
//	base_ev = event_base_new();
//	/*创建bufferevent对象*/
//	struct bufferevent* bev = bufferevent_socket_new(base_ev, -1, BEV_OPT_CLOSE_ON_FREE);
//	//这是对connect()的封装。如果bev的fd是-1，那么会自动调用socket()，并且设置nonblock。，随后再异步调用connect()；如果fd已经指定了，那么只是告诉bev去做connect()操作。
//	int flag = bufferevent_socket_connect(bev, (struct sockaddr *)&server_addr, sizeof(server_addr));
//	/*设置回调函数*/
//	bufferevent_setcb(bev, read_cb, write_service_cb, error_cb, NULL);
//	if (-1 == flag)
//	{
//		printf("Connect failed\n");
//		return 1;
//	}
//	/**
//	 * 启用事件管理：
//	 *（a）EV_TIMEOUT: 超时
//	 *（b）EV_READ: 只要网络缓冲中还有数据，回调函数就会被触发
//	 *（c）EV_WRITE: 只要塞给网络缓冲的数据被写完，回调函数就会被触发
//	 *（d）EV_SIGNAL: POSIX信号量，参考manual吧
//	 *（e）EV_PERSIST: 不指定这个属性的话，回调函数被触发后事件会被删除
//	 *（f）EV_ET: Edge-Trigger边缘触发，参考EPOLL_ET
//	**/
//	bufferevent_enable(bev, EV_READ | EV_WRITE | EV_PERSIST);
//	/*监听可写事件*/
//	struct event *ev = event_new(base_ev, bufferevent_getfd(bev), EV_WRITE | EV_PERSIST, write_cb, bev);
//	event_add(ev, NULL);
//	HANDLE hHandle = CreateThread(NULL, 0, SendMsg, bev, 0, 0);
//	event_base_dispatch(base_ev); //事件循环。
//	event_base_free(base_ev);  //销毁libevent  
//	CloseHandle(hHandle);
//	system("pause");
//	return 1;
//}