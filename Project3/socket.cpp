#include "Socket.h"
#include <iostream> 
#include <sstream>
#include <string>
using namespace std;

Socket::Socket()
{
}


Socket::~Socket()
{
	WSACleanup(); //�ͷ��׽�����Դ  
}

bool Socket::createSendServer(const char* ip,const int hton,const double velocity) {
	//��ʼ���׽��ֶ�̬��
	if (WSAStartup(MAKEWORD(2, 2), &S_wsd) != 0)
	{
		cout << "WSAStartup failed!" << endl;
		return false;
	}
	//�����׽���  
	sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sHost)
	{
		cout << "socket failed!" << endl;
		//WSACleanup();//�ͷ��׽�����Դ  
		return  false;
	}

	//���÷�������ַ�ͷ���˿ں�  
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(ip);
	servAddr.sin_port = htons(hton);
	int nServAddlen = sizeof(servAddr);

	//���ӷ�����  
	retVal = connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr));
	if (SOCKET_ERROR == retVal)
	{
		cout << "connect failed!" << endl;
		closesocket(sHost); //�ر��׽���  
							//WSACleanup(); //�ͷ��׽�����Դ  
		return false;
	}
	return true;
}
int Socket::sendMessage(char* message, int lenght) {
	while (1)
	{
		retVal = send(sHost, message, lenght, 0);
		if (retVal == SOCKET_ERROR)
		{
			int r = WSAGetLastError();
			if (r == WSAEWOULDBLOCK)
			{
				continue;
			}
			else
			{
				cout << "send failed!" << endl;
				return -1;
			}
		}
		else
		{
			break;
		}

	}

	return 0;
}

int Socket::offSendServer() {
	//�˳�
	shutdown(sHost, SD_SEND);
	closesocket(sHost); //�ر��׽���  
	return 0;
}

int Socket::offRecServer()
{
	//�˳�  
	closesocket(sServer);   //�ر��׽���  
	closesocket(sClient);   //�ر��׽���   
	return 0;
}

int Socket::createReceiveServer(const int port) {
	//��ʼ���׽��ֶ�̬��  
	if (WSAStartup(MAKEWORD(2, 2), &H_wsd) != 0)
	{
		cout << "WSAStartup failed!" << endl;
		return 1;
	}

	//�����׽���  
	sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sServer)
	{
		cout << "socket failed!" << endl;
		WSACleanup();//�ͷ��׽�����Դ;  
		return  -1;
	}

	//�������׽��ֵ�ַ   
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(port);
	addrServ.sin_addr.s_addr = INADDR_ANY;
	//���׽���  
	retVal = bind(sServer, (LPSOCKADDR)&addrServ, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == retVal)
	{
		cout << "bind failed!" << endl;
		closesocket(sServer);   //�ر��׽���  
		WSACleanup();           //�ͷ��׽�����Դ;  
		return -1;
	}

	//��ʼ����   
	cout << "listening ..." << endl;
	retVal = listen(sServer, 1);

	if (SOCKET_ERROR == retVal)
	{
		cout << "listen failed!" << endl;
		closesocket(sServer);   //�ر��׽���  
								//WSACleanup();           //�ͷ��׽�����Դ;  
		return -1;
	}

	//���ܿͻ�������  
	sockaddr_in addrClient;
	int addrClientlen = sizeof(addrClient);
	sClient = accept(sServer, (sockaddr FAR*)&addrClient, &addrClientlen);
	if (INVALID_SOCKET == sClient)
	{
		cout << "accept failed!" << endl;
		closesocket(sServer);   //�ر��׽���  
		WSACleanup();           //�ͷ��׽�����Դ;  
		return -1;
	}
	cout << "accept success!" << endl;
	cout << "data from cline" << endl;
	while (true) {
		//���տͻ�������
		//���buffer
		ZeroMemory(buf, BUF_SIZE);
		//��ȡ����
		retVal = recv(sClient, buf, BUF_SIZE, 0);

		if (SOCKET_ERROR == retVal)
		{
			cout << "recv failed!" << endl;
			closesocket(sServer);   //�ر��׽���
			closesocket(sClient);   //�ر��׽���       
			return -1;
		}
	}
	//�˳�  
	closesocket(sServer);   //�ر��׽���  
	closesocket(sClient);   //�ر��׽���   
	return 0;

}

