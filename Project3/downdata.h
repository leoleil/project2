#pragma once
#include <iostream>  
#include <sstream>
#include <string>
#include <fstream>  
#include <vector>
#include <unordered_map>
#include "winsock2.h" 
#include <time.h>
#include <Windows.h>
#include "MySQLInterface.h"
#include "StringNumUtils.h"
#include "DowndataSocket.h"
#include <thread> //thread ͷ�ļ�,ʵ�����й��̵߳���
#include "DownMessage.h"
#include <io.h>
#include <direct.h>

using namespace std;


//�����ϴ��߳�
DWORD WINAPI download_rec(LPVOID lpParameter);
DWORD WINAPI download(LPVOID lpParameter);