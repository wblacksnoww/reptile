#include<iostream>
#include<string>
#include<Windows.h>
#include<queue>
#pragma comment(lib,"ws2_32")
 
using namespace std;
string g_sHost;
string g_sObject;
SOCKET g_socket;

//开始抓取
bool StartCatch(string url);
//解析url
bool AnalyseURL(string url);
//解析URL\https
bool AnalyseUrl2(string url);
//联网
bool Connect();
//获取网页
string GetHtml(string url);

int main() 
{
	cout << "***************************************************"<< endl;
	cout << "*                                                 *" << endl;
	cout << "*               爬取资源                          *" << endl;
	cout << "***************************************************" << endl;

	cout << "请给我一只支点（入口url）：" << endl;

	string starUrl;
	cin >> starUrl;
	//创建文件夹啊
	CreateDirectory( L"./resource",NULL );
	CreateDirectory(L"./resource/image", NULL);						//高清图片
	CreateDirectory(L"./resource/video", NULL);						//高清视频


	//开始抓取

	StartCatch(starUrl);

	return 0;
}



//开始抓取
bool StartCatch(string url)

{
	queue<string> q;//URL队列
	q.push(url);


	while (!q.empty()) 
	{
		string currentURL = q.front();
		q.pop();
		//解析url
		if (false == AnalyseURL(currentURL))
		{

			//解析url2
			if (false == AnalyseUrl2(currentURL))
			{


				cout << "解析失败" << endl;
				return false;
			}
		}
		//链接网络
		if (false == Connect())
			continue;
		string html = GetHtml(currentURL);
		cout << html << endl;

	}
	


	return true;
}

//解析url
bool AnalyseURL(string url) 
{

	if (string::npos == url.find("http://"))
		return false;
	if (url.length() <= 7)
		return false;
	int pos = url.find('/', 7);
	if (pos==string::npos)
	{
		g_sHost = url.substr(7);
		g_sObject = "/";
	}
	else 
	{
		g_sHost = url.substr(7, pos - 7);
		g_sObject = url.substr(pos);
	
		
	
	}
	if (g_sHost.empty() || g_sObject.empty())
		return  false;
	cout << "域名：" << g_sHost << " " << "资源：" << g_sObject << endl;

	return true;
}

//解析URL\https
bool AnalyseUrl2(string url)
{
	if (string::npos == url.find("https://"))
		return false;
	if (url.length() <= 8)
		return false;
	int pos = url.find('/', 8);
	if (pos == string::npos)
	{
		g_sHost = url.substr(8);
		g_sObject = '/';
	}
	else
	{
		g_sHost = url.substr(8, pos - 8);
		g_sObject = url.substr(pos);
	}
	if (g_sHost.empty())
		return false;
	cout << "域名：" << g_sHost << " " << "资源：" << g_sObject << endl;
	return true;
}


//联网
bool Connect() 
{
	//初始化网络
	WSADATA wd;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wd))
		return false;
	//创建套接字
	g_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (g_socket == INVALID_SOCKET)
		return false;
	//解析域名
	hostent* p = gethostbyname(g_sHost.c_str());
	if (p == NULL)
		return false;

	//链接服务器
	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(80);
	memcpy(&sa.sin_addr, p->h_addr, 4);
	

	//链接网络
	if (connect(g_socket, (sockaddr*)&sa, sizeof(sockaddr)))
		return false;

	return true;
}

//获取网页
string GetHtml(string url) 
{

	//获取网页
	//http协议
	string info;
	info += "GET " + g_sObject + " HTTP/1.1\r\n";
	info += "Host: " + g_sHost + "\r\n";
	info += "Connectiong: Close\r\n\r\n";

	if (SOCKET_ERROR == send(g_socket, info.c_str(), info.length(), 0))
		return "1";

	//接受数据
	char ch = 0;
	string html;
	while (recv(g_socket, &ch,sizeof(char), 0)) 
	{
		html += ch;
	
		cout << html;
	
	}

	return html;

}
