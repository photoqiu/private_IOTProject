#include "DoService.h"


vector<_clientInfo> DoService::client_vec;
bool DoService::m_bSendAll = true;
bool DoService::bTransFile = false;
bool DoService::iTransFile = false;
bool DoService::dTransFile = false;
string DoService::fileData;
string DoService::currentTransferFileName;


DoService::DoService()
{
	initSock();
}

//����ͷ����������
void DoService::paraseHead(const string& src, string& msgType, string& name, int *startPos)
{
	msgType = src.substr(0, 4);
	//�յ�����Ϣͷ����#��β������#�Ժ����ļ�����Ϣ-ͼƬ����
	int pos = src.find("#");
	//�յ�����Ϣͷ����#��β������$�Ժ����ļ�����Ϣ-����ini����
	int inipos = src.find("$");
	//�յ�����Ϣͷ����#��β������#�Ժ����ļ�����Ϣ-json����
	int configpos = src.find("*");
	//�յ�����Ϣͷ����#��β������#�Ժ����ļ�����Ϣ
	if (pos != string::npos)
	{
		name.assign(src.begin() + 4, src.begin() + pos);
		*startPos = pos + 1;
	}
	else if (inipos != string::npos)
	{
		name.assign(src.begin() + 4, src.begin() + inipos);
		*startPos = pos + 1;
	}
	else if (configpos != string::npos)
	{
		name.assign(src.begin() + 4, src.begin() + configpos);
		*startPos = pos + 1;
	}
	else
	{
		name = "untiltled";
		*startPos = 4;
	}
}

/*�����Ǳ����·������*/
string DoService::getImagePath(const string& fileName)
{
	return UPLOADERIMAGEPATH + fileName;
}

void DoService::sendAll(char *msg)
{
	for (auto &ele : client_vec)
	{
		/////�����ж��ǲ���Ҫ��¼
		if (ele.bLogin)
			bufferevent_write(ele.m_pBev, msg, strlen(msg));
	}
}

bool DoService::initSock()
{
#ifdef WIN32
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
	//SOCKET sockFd;
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;//�����ַ������
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(Ports);
	sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockFd == INVALID_SOCKET)
	{
		cout << "socket error" << endl;
		system("pause");
		return false;
	}
	evutil_make_listen_socket_reuseable(sockFd);//����socket����
	evutil_make_socket_nonblocking(sockFd);     //���÷�����
	//��IP    
	if (bind(sockFd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == SOCKET_ERROR) {
		puts("bind error");
		system("pause");
		return false;
	}
	listen(sockFd, 10);
	return true;
}

void DoService::read_cb(struct bufferevent *bev, void *arg)
{
	BufferManager m_files;
	struct evbuffer *buf = (struct evbuffer *)arg;
	evutil_socket_t fd = bufferevent_getfd(bev);
	char msg[CharsBuffers];
	string bufferDatas;
	memset(msg, 0, CharsBuffers);
	size_t len = bufferevent_read(bev, msg, sizeof(msg) - 1);
	cout << "the file length:" << len << endl;
	if (len < FileBuffers)
	{
		msg[len] = '\0';
	}
	string msgType = "";
	string name = "";
	string imageBuffers = "";
	string data;
	int headLen = 0;
	if (len > 4)
	{
		paraseHead(msg, msgType, name, &headLen);
	}
	else 
	{
		msgType = "";
	}	
	if (msgType == "file")
	{
		currentTransferFileName = name;
		bTransFile = true;
		fileData.clear();
		data.assign(msg + headLen, msg + len);
		if (len < FileBuffers)//���һ���Դ�����ɣ���ֱ�ӱ���
		{
			bTransFile = false;
			data = FormatCharDatas(data, '#');
			imageBuffers = base64_decode(data);
			m_files.WritePictureFile(imageBuffers.c_str(), getImagePath(currentTransferFileName).c_str(), imageBuffers.length());
			return;
		}
		fileData += data;
	}
	else if (bTransFile)
	{
		data.assign(msg, msg + len);
		if (len < FileBuffers)
		{
			bTransFile = false;
			fileData += data;
			fileData = FormatCharDatas(fileData, '#');
			imageBuffers = base64_decode(fileData);
			cout << "�յ��ļ���" << strlen(imageBuffers.c_str()) << endl;
			m_files.WriteTextFile(fileData.c_str(), getImagePath("123.log").c_str(), fileData.length());
			m_files.WritePictureFile(imageBuffers.c_str(), getImagePath(currentTransferFileName).c_str(), imageBuffers.length());
			return;
		}
		fileData += data;
	}
	else if (msgType == "dile")
	{
		currentTransferFileName = name;
		dTransFile = true;
		fileData.clear();
		data.assign(msg + headLen, msg + len);
		if (len < FileBuffers)//���һ���Դ�����ɣ���ֱ�ӱ���
		{
			dTransFile = false;
			data = FormatCharDatas(data, '*');
			m_files.WriteTextFile(data.c_str(), getImagePath(currentTransferFileName).c_str(), data.length());
			return;
		}
		fileData += data;
	}
	else if (dTransFile)
	{
		data.assign(msg, msg + len);
		if (len < FileBuffers)
		{
			dTransFile = false;
			fileData += data;
			fileData = FormatCharDatas(fileData, '*');
			m_files.WriteTextFile(fileData.c_str(), getImagePath(currentTransferFileName).c_str(), fileData.length());
			return;
		}
		fileData += data;
	}
	else if (msgType == "iile")
	{
		currentTransferFileName = name;
		iTransFile = true;
		fileData.clear();
		data.assign(msg + headLen, msg + len);
		if (len < FileBuffers)//���һ���Դ�����ɣ���ֱ�ӱ���
		{
			iTransFile = false;
			data = FormatCharDatas(data, '$');
			m_files.WriteTextFile(data.c_str(), getImagePath(currentTransferFileName).c_str(), data.length());
			return;
		}
		fileData += data;
	}
	else if (iTransFile)
	{
		data.assign(msg, msg + len);
		if (len < FileBuffers)
		{
			iTransFile = false;
			fileData += data;
			fileData = FormatCharDatas(fileData, '$');
			m_files.WriteTextFile(fileData.c_str(), getImagePath(currentTransferFileName).c_str(), fileData.length());
			return;
		}
		fileData += data;
	}
	else
	{
		HandleTXTMsg(bev, fd, msg, len);
	}
}

string DoService::FormatCharDatas(string datas, const char flag) 
{
	int pos = datas.find(flag);
	string backdatas = "";
	if (pos != string::npos)
	{
		backdatas.assign(datas.begin() + (pos + 1), datas.end());
		return backdatas;
	}
	return datas;
}

void DoService::HandleTXTMsg(struct bufferevent* bev, int fd, char *msg, int len)
{
	BufferManager m_files;
	printf("fd=%u, �յ�: %s\n", fd, msg);
	if (m_bSendAll)
	{
		sendAll(msg);
	}
	else
	{
		bufferevent_write(bev, msg, len);
	}
}

//�¼��ص�  
void DoService::error_cb(struct bufferevent *bev, short event, void *arg)
{
	/*
	BEV_EVENT_READING �� bufferevent �Ͻ��ж�ȡ����ʱ������һ���¼�
	BEV_EVENT_WRITING �� bufferevent �Ͻ���д�����ʱ������һ���¼�
	BEV_EVENT_ERROR ���� bufferevent ����ʱ����
	BEV_EVENT_TIMEOUT �� bufferevent �ϳ����˳�ʱ
	BEV_EVENT_EOF �� bufferevent ���������ļ������������ӶϿ�
	BEV_EVENT_CONNECTED �� bufferevent ���������������
	*/
	evutil_socket_t fd = bufferevent_getfd(bev);
	vector<_clientInfo>::iterator it = client_vec.begin(); //itָ�����쳣�Ŀͻ��˾��
	while (it != client_vec.end())
	{
		if (it->sock_fd == fd)
			break;
		it++;
	}
	printf("fd = %u, ", fd);
	if (event & BEV_EVENT_TIMEOUT) {
		printf("Timed out\n");
	}
	else if (event & BEV_EVENT_EOF) {
		printf("connection closed\n");
	}
	else if (event & BEV_EVENT_ERROR) {
		printf("some other error\n");
	}
	bufferevent_free(bev);//ɾ����ǰ��Ч�ͻ��˶�Ӧ��bev
	if (it != client_vec.end())//�޳���Ч�Ŀͻ��˾��
	{
		client_vec.erase(it);
		cout << "��ǰ��������" << client_vec.size() << endl;
	}
}

void DoService::do_accept(evutil_socket_t fd, short event, void *arg)
{
	 evutil_socket_t client_socketfd;//�ͻ����׽���      
	 struct sockaddr_in client_addr; //�ͻ��������ַ�ṹ��     
	 int in_size = sizeof(struct sockaddr_in);
	 /**
	  * �ͻ���socket
	  * �ȴ������������������ʽ��
	 **/
	 client_socketfd = accept(fd, (struct sockaddr *) &client_addr, &in_size);    
	 if (client_socketfd < 0) {
		puts("accpet error");
		exit(1);
	 }
	 //����ת��  
	 struct event_base* base_evs = (struct event_base *) arg;
	 //socket���ͻ�ӭ��Ϣ
	 const char* msg = "Send from Server";
	 int size = send(client_socketfd, msg, strlen(msg), 0);
	 //����һ���¼�������¼���Ҫ���ڼ����Ͷ�ȡ�ͻ��˴��ݹ���������  
	 //�־����ͣ����ҽ�base_ev���ݵ�do_read�ص�������ȥ  
	 //struct event *ev;
	 //ev = event_new(base_ev, client_socketfd, EV_TIMEOUT | EV_READ | EV_PERSIST, do_read, base_ev);
	 //event_add(ev, NULL);
	 struct bufferevent *bev = bufferevent_socket_new(base_evs, client_socketfd, BEV_OPT_CLOSE_ON_FREE);//BEV_OPT_CLOSE_ON_FREE�� bufferevent ���ͷ�ͬʱ�رյײ㣨socket ���رյȣ�
	 //bufferevent_set_timeouts(bev, struct timeval *READ, struct timeval *WRITE)�����ö�д��ʱ
	 //���ö�ȡ������errorʱ��ķ���  
	 bufferevent_setcb(bev, read_cb, NULL, error_cb, base_evs);
	 //��������  
	 bufferevent_enable(bev, EV_READ | EV_PERSIST);
	 //�������ӵĿͻ���
	 _clientInfo obj(client_socketfd, bev);
	 client_vec.push_back(obj);
	 cout << "��ǰ��������" << client_vec.size() << endl;
}

void DoService::event_loop()
{
	base_ev = event_base_new(); //��ʼ��һ��event_base  
	//����һ���¼�������Ϊ�־���EV_PERSIST���ص�����Ϊdo_accept����Ҫ���ڼ������ӽ����Ŀͻ��ˣ�  
	//��base_ev���ݵ�do_accept�е�arg����  
	struct event *ev = event_new(base_ev, sockFd, EV_TIMEOUT | EV_READ | EV_PERSIST, do_accept, base_ev);
	//ע���¼���ʹ�¼����� pending�ĵȴ�״̬  
	event_add(ev, NULL);
	event_base_dispatch(base_ev); //�¼�ѭ������Ϊ�������û��ע���¼������Ի�ֱ���˳�  .0 ��ʾ�ɹ�
	event_base_free(base_ev);  //����libevent
}

DoService::~DoService()
{
	
}
