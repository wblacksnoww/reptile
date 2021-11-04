#include<iostream>
#include<string>
#include<Windows.h>
#include<queue>
#include<regex>
#pragma comment(lib,"ws2_32")
 
using namespace std;
string g_sHost;
string g_sObject;
SOCKET g_socket;

//��ʼץȡ
bool StartCatch(string url);
//����url
bool AnalyseURL(string url);
//����URL\https
bool AnalyseUrl2(string url);
//����
bool Connect();
//��ȡ��ҳ
string GetHtml(string url);
//����ͼƬ
bool Download(string url, string filename);

int main() 
{
	cout << "***************************************************"<< endl;
	cout << "*                                                 *" << endl;
	cout << "*               ��ȡ��Դ                          *" << endl;
	cout << "***************************************************" << endl;

	cout << "�����һֻ֧�㣨���url����" << endl;

	string starUrl;
	cin >> starUrl;
	//�����ļ��а�
	CreateDirectory( L"./resource",NULL );
	CreateDirectory(L"./resource/image", NULL);						//����ͼƬ
	CreateDirectory(L"./resource/video", NULL);						//������Ƶ


	//��ʼץȡ

	StartCatch(starUrl);

	return 0;
}



//��ʼץȡ
bool StartCatch(string url)

{
	queue<string> q;//URL����
	q.push(url);


	while (!q.empty()) 
	{
		string currentURL = q.front();
		q.pop();
		//����url
		if (false == AnalyseURL(currentURL))
		{

			//����url2
			if (false == AnalyseUrl2(currentURL))
			{


				cout << "����ʧ��" << endl;
				return false;
			}
		}
		//��������
		if (false == Connect())
			continue;
		string html = GetHtml(currentURL);
		//cout << html << endl;

		//ƥ������е�url
		smatch mat;
		string::const_iterator start = html.begin();
		string::const_iterator end = html.end();
		regex gex("http://[^\\s'\"<>()]+");
		vector<string>vecUrl;
		while (regex_search(start, end, mat, gex))
		{

			string newstr(mat[0].first, mat[0].second);

			vecUrl.push_back(newstr);
			
			start = mat[0].second;

		}
		//�������е�url
		for (int i = 0; i < vecUrl.size(); i++) 
		{
			string filename = "./resource/image/1.jpg";
			//�ж��Ƿ���ͼƬ
			string imageUrl = vecUrl[i];
			if (imageUrl.find(".jpg") != string::npos) 
			{
				//����һ��ͼƬ
				Download(imageUrl, filename);
			
			
			}
		
		
		
		}
	}
	


	return true;
}

//����url
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
	cout << "������" << g_sHost << " " << "��Դ��" << g_sObject << endl;

	return true;
}

//����URL\https
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
	cout << "������" << g_sHost << " " << "��Դ��" << g_sObject << endl;
	return true;
}


//����
bool Connect() 
{
	//��ʼ������
	WSADATA wd;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wd))
		return false;
	//�����׽���
	g_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (g_socket == INVALID_SOCKET)
		return false;
	//��������
	hostent* p = gethostbyname(g_sHost.c_str());
	if (p == NULL)
		return false;

	//���ӷ�����
	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(80);
	memcpy(&sa.sin_addr, p->h_addr, 4);
	

	//��������
	if (connect(g_socket, (sockaddr*)&sa, sizeof(sockaddr)))
		return false;

	return true;
}

//��ȡ��ҳ
string GetHtml(string url)
{

	//��ȡ��ҳ
	//httpЭ��
	string info;
	info += "GET " + g_sObject + " HTTP/1.1\r\n";
	info += "Host: " + g_sHost + "\r\n";
	info += "Connectiong: Close\r\n\r\n";

	if (SOCKET_ERROR == send(g_socket, info.c_str(), info.length(), 0))
		return "1";

	//��������
	char ch = 0;
	string html;
	while (recv(g_socket, &ch, sizeof(char), 0))
	{
		html += ch;

		//cout << html;

	}
	//�ر��׽���
	closesocket(g_socket);
	//�ͷ�����
	WSACleanup();
	return html;
}

	//����ͼƬ
	bool Download(string url, string filename)
	{
		//��ȡ��ҳ
		//httpЭ��
		string info;
		info += "GET " + g_sObject + " HTTP/1.1\r\n";
		info += "Host: " + g_sHost + "\r\n";
		info += "Connectiong: Close\r\n\r\n";

		if (SOCKET_ERROR == send(g_socket, info.c_str(), info.length(), 0))
			return "1";

		FILE* fp = fopen(filename.c_str(), "wb");
		//��������
		char buffer[20] = { 0 };
		int nRecv;
		while (nRecv = recv(g_socket, buffer, sizeof(buffer), 0))
		{
			fwrite(buffer, 1, nRecv, fp);

			//cout << html;

		}
		//�ر��׽���
		closesocket(g_socket);
		//�ͷ�����
		WSACleanup();
		
	
	
	
	}
