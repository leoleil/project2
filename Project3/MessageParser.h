#pragma once
#include <iostream>  
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "winsock2.h" 
#include <time.h>
#include <Windows.h>

using namespace std;



const int M = 1048576;//1M
const int K = 1024;   //1K
const double NEW_NULL = 1000000000;//���������NULL

struct message_set;
struct message_byte;
struct field;
struct data;

typedef struct message_set message_set;
typedef struct message_byte message_byte;
typedef struct field field;
typedef struct data data;



struct message_byte
{
	//70K
	BYTE val[70*K];
};

//���屨���ֶ�
struct field
{
	char name[20];
	UINT16 type;
	double min;
	double max;
	char unit[8];
	bool display;
};
struct data
{
	BYTE byte_data[64 * K];
};

////����һ�����ĳر�����յ���ȫ������
//vector <message_byte> MESSAGE_VECTOR;//���ĳ�
//CRITICAL_SECTION g_CS;//ȫ�ֹؼ�����ζ���
extern  vector <message_byte> MESSAGE_VECTOR;//���ĳ�
extern CRITICAL_SECTION g_CS;//ȫ�ֹؼ�����ζ���
extern int STOP;//ֹͣ��ʶ
				
string getTime();//��ȡʱ���ַ������
//����API

//���Ľ����߳����
DWORD WINAPI messagePasing(LPVOID lpParameter);
string getType(UINT16 num);
