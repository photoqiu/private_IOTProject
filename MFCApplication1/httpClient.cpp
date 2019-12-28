#include "stdafx.h"
#include "httpClient.h"

httpClient::httpClient()
{
}

httpClient::~httpClient()
{
}

int httpClient::PostDatas()
{
	nc.setUrl("http://127.0.0.1:5000/");
	nc.addQueryHeader("User-Agent", "Mozilla/5.0");
	nc.addQueryParam("refresh_token", "5ce4902a-e466-452d-8bbb-0fb1bf378f8a");
	nc.addQueryParam("client_id", "gigy");
	nc.addQueryParam("client_secret", "secret");
	nc.addQueryParam("grant_type", "refresh_token");
	nc.doPost();
	cout << nc.responseBody() << endl;
	return 0;
}

void httpClient::GetDatas()
{
	// + nc.urlEncode("Smelly cat")
	nc.doGet("http://www.jd.com");
	std::cout << nc.responseBody();
}

void httpClient::PostByRawData()
{
	nc.setUrl("http://www.jd.com");
	nc.doPost("param1=value&param=value");
}

void httpClient::DownloadFile()
{
	nc.setOutputFile("d:\\test\\image.jpg"); //only UTF-8 file names are supported on Windows
	nc.doGet("http://img.lanrentuku.com/img/allimg/1906/15608252966046.jpg");
}

void httpClient::UploaderFile()
{
	//nc.setProxy("127.0.0.1", "8888", CURLPROXY_HTTP); // CURLPROXY_HTTP, CURLPROXY_SOCKS4, CURLPROXY_SOCKS4A, CURLPROXY_SOCKS5, CURLPROXY_SOCKS5_HOSTNAME
	std::string fileName = "c:\\test\\file.txt"; //only UTF-8 file names are supported on Windows
	//std::string fileName = IuCoreUtils::SystemLocaleToUtf8(fileName); <-- if you want to upload a file with ANSI filename on Windows
	nc.setUrl("http://takebin.com/action");
	nc.addQueryParamFile("file", fileName, IuCoreUtils::ExtractFileName("datas.txt"), "");
	nc.addQueryParam("fileDesc", "cool file");
	nc.doUploadMultipartData();
	if (nc.responseCode() == 200) {
		std::cout << nc.responseBody();
	}
}