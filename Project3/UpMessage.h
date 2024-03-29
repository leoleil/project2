#pragma once
#include "Message.h"
class UpMessage :
	public Message
{
public:
	UpMessage(UINT16 messageId, long long dateTime, bool encrypt, UINT32 taskNum, char* fileName, char* expandName, bool endFlag);
	UpMessage();
	~UpMessage();
private:
	UINT32 taskNum;//任务编号
	char fileName[32];//文件名
	char expandName[8];//拓展名
	bool endFlag;//结束标志
	int size=0;//数据块长度
	char* data;//数据块
public:
	UINT32 getterTaskNum();

	void getterFileName(char* fileName);

	void getterExpandName(char* expandName);

	bool getterEndFlag();

	int getterDataSize();

	void setterDataSize(int size);

	void getterData(char* data, int & size);

	void setterData(char* data, int size);
public:
	//为数据传输创造一个message的接口
	void createMessage(char* buf, int & message_size, int buf_size);
	//通过数据包来解析数据接口
	void messageParse(char* buf);
};

