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

//解析头部发送类型
void DoService::paraseHead(const string& src, string& msgType, string& name, int *startPos)
{
	msgType = src.substr(0, 4);
	//收到的信息头部以#结尾，所以#以后是文件的信息-图片数据
	int pos = src.find("#");
	//收到的信息头部以#结尾，所以$以后是文件的信息-配置ini数据
	int inipos = src.find("$");
	//收到的信息头部以#结尾，所以#以后是文件的信息-json数据
	int configpos = src.find("*");
	//收到的信息头部以#结尾，所以#以后是文件的信息
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

/*这里是保存的路径定义*/
string DoService::getImagePath(const string& fileName)
{
	return UPLOADERIMAGEPATH + fileName;
}

void DoService::sendAll(char *msg)
{
	for (auto &ele : client_vec)
	{
		/////这里判断是不是要登录
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
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;//任意地址可连接
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(Ports);
	sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockFd == INVALID_SOCKET)
	{
		cout << "socket error" << endl;
		system("pause");
		return false;
	}
	evutil_make_listen_socket_reuseable(sockFd);//设置socket重用
	evutil_make_socket_nonblocking(sockFd);     //设置非阻塞
	//绑定IP    
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
		if (len < FileBuffers)//如果一次性传输完成，则直接保存
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
			cout << "收到文件：" << strlen(imageBuffers.c_str()) << endl;
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
		if (len < FileBuffers)//如果一次性传输完成，则直接保存
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
		if (len < FileBuffers)//如果一次性传输完成，则直接保存
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
	printf("fd=%u, 收到: %s\n", fd, msg);
	if (m_bSendAll)
	{
		sendAll(msg);
	}
	else
	{
		bufferevent_write(bev, msg, len);
	}
}

//事件回调  
void DoService::error_cb(struct bufferevent *bev, short event, void *arg)
{
	/*
	BEV_EVENT_READING 在 bufferevent 上进行读取操作时出现了一个事件
	BEV_EVENT_WRITING 在 bufferevent 上进行写入操作时出现了一个事件
	BEV_EVENT_ERROR 进行 bufferevent 操作时出错
	BEV_EVENT_TIMEOUT 在 bufferevent 上出现了超时
	BEV_EVENT_EOF 在 bufferevent 上遇到了文件结束符，连接断开
	BEV_EVENT_CONNECTED 在 bufferevent 上请求连接完成了
	*/
	evutil_socket_t fd = bufferevent_getfd(bev);
	vector<_clientInfo>::iterator it = client_vec.begin(); //it指向发生异常的客户端句柄
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
	bufferevent_free(bev);//删除当前无效客户端对应的bev
	if (it != client_vec.end())//剔除无效的客户端句柄
	{
		client_vec.erase(it);
		cout << "当前连接数：" << client_vec.size() << endl;
	}
}

void DoService::do_accept(evutil_socket_t fd, short event, void *arg)
{
	 evutil_socket_t client_socketfd;//客户端套接字      
	 struct sockaddr_in client_addr; //客户端网络地址结构体     
	 int in_size = sizeof(struct sockaddr_in);
	 /**
	  * 客户端socket
	  * 等待接受请求，这边是阻塞式的
	 **/
	 client_socketfd = accept(fd, (struct sockaddr *) &client_addr, &in_size);    
	 if (client_socketfd < 0) {
		puts("accpet error");
		exit(1);
	 }
	 //类型转换  
	 struct event_base* base_evs = (struct event_base *) arg;
	 //socket发送欢迎信息
	 const char* msg = "Send from Server";
	 int size = send(client_socketfd, msg, strlen(msg), 0);
	 //创建一个事件，这个事件主要用于监听和读取客户端传递过来的数据  
	 //持久类型，并且将base_ev传递到do_read回调函数中去  
	 //struct event *ev;
	 //ev = event_new(base_ev, client_socketfd, EV_TIMEOUT | EV_READ | EV_PERSIST, do_read, base_ev);
	 //event_add(ev, NULL);
	 struct bufferevent *bev = bufferevent_socket_new(base_evs, client_socketfd, BEV_OPT_CLOSE_ON_FREE);//BEV_OPT_CLOSE_ON_FREE当 bufferevent 被释放同时关闭底层（socket 被关闭等）
	 //bufferevent_set_timeouts(bev, struct timeval *READ, struct timeval *WRITE)来设置读写超时
	 //设置读取方法和error时候的方法  
	 bufferevent_setcb(bev, read_cb, NULL, error_cb, base_evs);
	 //设置类型  
	 bufferevent_enable(bev, EV_READ | EV_PERSIST);
	 //保存连接的客户端
	 _clientInfo obj(client_socketfd, bev);
	 client_vec.push_back(obj);
	 cout << "当前连接数：" << client_vec.size() << endl;
}

void DoService::event_loop()
{
	base_ev = event_base_new(); //初始化一个event_base  
	//创建一个事件，类型为持久性EV_PERSIST，回调函数为do_accept（主要用于监听连接进来的客户端）  
	//将base_ev传递到do_accept中的arg参数  
	struct event *ev = event_new(base_ev, sockFd, EV_TIMEOUT | EV_READ | EV_PERSIST, do_accept, base_ev);
	//注册事件，使事件处于 pending的等待状态  
	event_add(ev, NULL);
	event_base_dispatch(base_ev); //事件循环。因为我们这边没有注册事件，所以会直接退出  .0 表示成功
	event_base_free(base_ev);  //销毁libevent
}

DoService::~DoService()
{
	
}
