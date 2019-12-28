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

//�¼��ص�  
void error_cb(struct bufferevent *bev, short event, void *arg) {
	//
	// BEV_EVENT_READING �� bufferevent �Ͻ��ж�ȡ����ʱ������һ���¼�
	// BEV_EVENT_WRITING �� bufferevent �Ͻ���д�����ʱ������һ���¼�
	// BEV_EVENT_ERROR ���� bufferevent ����ʱ����
	// BEV_EVENT_TIMEOUT �� bufferevent �ϳ����˳�ʱ
	// BEV_EVENT_EOF �� bufferevent ���������ļ������������ӶϿ�
	// BEV_EVENT_CONNECTED �� bufferevent ���������������
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
		//����Ͽ�������������	
		bool b_reconnect = false;
		int timeOut = 10;
		SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
		while (timeOut--)
		{
			//��while�ڳ���Sleep�����������������ۿɼ���һ��ʱ���ӳ�
			int n = connect(sock, (sockaddr*)&server_addr, sizeof(server_addr));
			if (n == 0)
			{
				cout << "reconnect success!!" << endl;
				bufferevent_setfd(bev, sock);
				//��ʱҪ��������¼�
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
	// ���һ��event�����˿�д�¼�����ô���event�ͻ�һֱ����������ѭ������
	// ��Ϊһ������£�������Ƿ��������������д�������ǲ������ġ�
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
		if (msg[0] == '#')//�����ļ�·��ʱ��#��ʼ ---- ͼƬ����
		{
			sscanf(msg, "#%s", path);
			fileData = ReadPictureFile(path);
			//�ļ�ͷ��ʽΪ  file+�ļ���+����������
			name = getPictureFileName(path);
			FileOnDatas = string("file") + name;
			FileBufferSize = FileOnDatas.length();
			string ImageBuffer = CombinStringToChars(FileOnDatas, fileData.szFileBuffers);
			cout << "the file data counter:" << ImageBuffer.length() << endl;
			cout << "FileBufferSize counter:" << FileBufferSize << endl;
			bufferevent_write(bev, ImageBuffer.c_str(), FileBufferSize + ImageBuffer.length());
		}
		else if (msg[0] == '$')//�����ļ�·��ʱ��#��ʼ ---- ����.ini�ļ�����
		{
			sscanf(msg, "$%s", path);
			fileData = ReadTextFile(path);
			//�ļ�ͷ��ʽΪ  file+�ļ���+����������
			name = getSetupFileName(path);
			FileBufferSize = fileData.nFileSize + 4 + name.length();
			FileOnDatas = string("iile") + getSetupFileName(path);
			string IniBuffers = CombinStringToChars(FileOnDatas, fileData.szFileBuffers);
			bufferevent_write(bev, IniBuffers.c_str(), FileBufferSize);
			printf("���������ļ�");
		}
		else if (msg[0] == '*')//�����ļ�·��ʱ��#��ʼ ---- ����.json�ļ�����
		{
			sscanf(msg, "*%s", path);
			fileData = ReadTextFile(path);
			//�ļ�ͷ��ʽΪ  file+�ļ���+����������
			name = getDatasFileName(path);
			FileOnDatas = string("dile") + getDatasFileName(path);
			FileBufferSize = fileData.nFileSize + FileOnDatas.length();
			string JsonBuffers = CombinStringToChars(FileOnDatas, fileData.szFileBuffers);
			bufferevent_write(bev, JsonBuffers.c_str(), FileBufferSize);
			printf("���������ļ�");
		}
		else
		{//���ն˵���Ϣ���͸���������
			bufferevent_write(bev, msg, strlen(msg));
			printf("������Ϣ�ı�");
		}
		//ɾ��ͷ
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
//	/*����bufferevent����*/
//	struct bufferevent* bev = bufferevent_socket_new(base_ev, -1, BEV_OPT_CLOSE_ON_FREE);
//	//���Ƕ�connect()�ķ�װ�����bev��fd��-1����ô���Զ�����socket()����������nonblock����������첽����connect()�����fd�Ѿ�ָ���ˣ���ôֻ�Ǹ���bevȥ��connect()������
//	int flag = bufferevent_socket_connect(bev, (struct sockaddr *)&server_addr, sizeof(server_addr));
//	/*���ûص�����*/
//	bufferevent_setcb(bev, read_cb, write_service_cb, error_cb, NULL);
//	if (-1 == flag)
//	{
//		printf("Connect failed\n");
//		return 1;
//	}
//	/**
//	 * �����¼�����
//	 *��a��EV_TIMEOUT: ��ʱ
//	 *��b��EV_READ: ֻҪ���绺���л������ݣ��ص������ͻᱻ����
//	 *��c��EV_WRITE: ֻҪ�������绺������ݱ�д�꣬�ص������ͻᱻ����
//	 *��d��EV_SIGNAL: POSIX�ź������ο�manual��
//	 *��e��EV_PERSIST: ��ָ��������ԵĻ����ص��������������¼��ᱻɾ��
//	 *��f��EV_ET: Edge-Trigger��Ե�������ο�EPOLL_ET
//	**/
//	bufferevent_enable(bev, EV_READ | EV_WRITE | EV_PERSIST);
//	/*������д�¼�*/
//	struct event *ev = event_new(base_ev, bufferevent_getfd(bev), EV_WRITE | EV_PERSIST, write_cb, bev);
//	event_add(ev, NULL);
//	HANDLE hHandle = CreateThread(NULL, 0, SendMsg, bev, 0, 0);
//	event_base_dispatch(base_ev); //�¼�ѭ����
//	event_base_free(base_ev);  //����libevent  
//	CloseHandle(hHandle);
//	system("pause");
//	return 1;
//}