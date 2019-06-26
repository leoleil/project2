#define _CRT_SECURE_NO_WARNINGS 1
#include "MessageParser.h"
#include "MySQLInterface.h"

string getTime()//��ȡʱ���ַ������
{
	time_t timep;
	time(&timep); //��ȡtime_t���͵ĵ�ǰʱ��
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));//�����ں�ʱ����и�ʽ��
	return tmp;
}
template <class Type> string numToString(Type& num)
{
	stringstream ss;
	ss << num;
	string s1 = ss.str();
	return s1;
}
//ģ�庯������string���ͱ���ת��Ϊ���õ���ֵ���ͣ��˷��������ձ������ԣ�  
template <class Type> Type stringToNum(const string& str)
{
	istringstream iss(str);
	Type num;
	iss >> num;
	return num;
}
string getType(UINT16 num) {
	switch (num)
	{
	case 1:
		return "INT";
	case 2:
		return "INT";
	case 3:
		return "INT";
	case 4:
		return "INT";
	case 5:
		return "BIGINT";
	case 6:
		return "BIGINT";
	case 7:
		return "FLOAT";
	case 8:
		return "DOUBLE";
	case 9:
		return "VARCHAR(1)";
	case 10:
		return "VARCHAR(255)";
	case 11:
		return "VARCHAR(255)";
	case 12:
		return "BLOB)";
	case 13:
		return "BLOB";
	case 14:
		return "BLOB";
	case 15:
		return "BOOL";
	case 16:
		return "BIGINT";
	default:
		return "";
	}
}
DWORD WINAPI messagePasing(LPVOID lpParameter) {
	unordered_map<string, int> MAP;//������ǵ�һ����������
	int db_da_flag = 1;//��һ�����ݱ������ı�־
	//�������ݿ����

	MySQLInterface* db_test = new MySQLInterface;
	if (db_test->connectMySQL("127.0.0.1", "root", "", "teleinfodb", 3306)) {
		
		db_test->writeDataToDB("CREATE TABLE `�ֶζ����` (`����` int(11) NOT NULL AUTO_INCREMENT,`�ֶ���` varchar(20) NOT NULL,`��������` int(11) NOT NULL,`��Сֵ` double DEFAULT NULL,`���ֵ` double DEFAULT NULL,`��λ` varchar(10) NOT NULL,`��ʾ��־` int(11) NOT NULL,`�豸��` varchar(20) NOT NULL,PRIMARY KEY(`����`))");
		db_test->writeDataToDB("CREATE TABLE `�豸��ϵ��` (`����` int(11) NOT NULL AUTO_INCREMENT,`�豸��` varchar(20) NOT NULL,`���豸��` varchar(20) DEFAULT NULL,PRIMARY KEY(`����`))");
		db_test->closeMySQL();
	}
	else {
		//����ʧ��
		cout << db_test->errorNum << ":" << db_test->errorInfo << endl;
		return 0;
	}
	cout << "messagePasing starting..." << endl;
	
	while (1) {
		//Sleep(10);
		EnterCriticalSection(&g_CS);//����ؼ������
		//����Ƿ��б�����Դ��Դ
		if (MESSAGE_VECTOR.empty()) {
			LeaveCriticalSection(&g_CS);//�뿪�ؼ������
			continue;
		}
		LeaveCriticalSection(&g_CS);//�뿪�ؼ������
		//������Դ����
		//������Ҫ���ǵ�ÿ�ζ��ܱ�����������
		//cout << "paring..." << endl;
		EnterCriticalSection(&g_CS);//����ؼ������
		//���ļ��ϴ�С
		int setLen = MESSAGE_VECTOR.size();
		LeaveCriticalSection(&g_CS);//�뿪�ؼ������
		for (int i = 0; i < setLen; i++) {
			//Sleep(1);
			EnterCriticalSection(&g_CS);//����ؼ������
			BYTE byte_data[70*K];
			memcpy(byte_data, MESSAGE_VECTOR[i].val, 70*K);//�����ĳ�������ȡ��
			//byte_data = MESSAGE_VECTOR[i].val;
			LeaveCriticalSection(&g_CS);//�뿪�ؼ������
			//�жϱ���
			UINT16 type;//��������
			UINT16 length;//���ĳ���
			memcpy(&type, byte_data, 2);
			memcpy(&length, byte_data + 2, 2);
			//cout << "type is :" << type << endl;
			//#####################���Ľ���###############################
			
			if (type == 1000) {//���屨��
				//�������屨��
				//����ָ��
				int ptr = 4;//�Ѿ�����ǰ�����ֽ�
				//ʱ���
				long long timestamp;
				memcpy(&timestamp, byte_data + ptr, sizeof(long long));
				ptr = ptr + sizeof(long long);
				//���ܱ�ʶ
				bool flag;
				memcpy(&flag, byte_data + ptr, 1);
				ptr = ptr + 1;
				//�豸��
				char name[40];
				memcpy(name, byte_data + ptr, 40);
				//д��־
				MySQLInterface* db_test = new MySQLInterface;
				string sql_difinition = "insert into ϵͳ��־�� (����,�¼�����,����) values ('ͨ��ģ��',1020,'ͨ�����Ļ��յ�";
				sql_difinition = sql_difinition + name + "ң�ⱨ���屨��');";
				if (db_test->connectMySQL("127.0.0.1", "root", "", "mangeinfodb", 3306)) {
					db_test->writeDataToDB(sql_difinition);
					db_test->closeMySQL();
				}
				else {
					//����ʧ��
					cout << db_test->errorNum << ":" << db_test->errorInfo << endl;
					return 0;
				}
				cout << "| ͨ��ģ�� | ";
				cout << getTime();
				cout << "| ���ն��屨��";
				cout.setf(ios::left);
				cout.width(37);
				cout << name;
				cout << "|" << endl;
				MAP[name] = 0;//����豸
				ptr = ptr + 40;
				//���豸��
				char parentName[40];
				memcpy(parentName, byte_data + ptr, 40);
				ptr = ptr + 40;
				//ʶ���ֶ�����
				vector<field> fields;
				while (ptr < length) {
					field sub_field;
					//�ֶ���
					memcpy(sub_field.name, byte_data + ptr, 40);
					ptr = ptr + 40;
					//�ֶ����ͱ��
					memcpy(&sub_field.type, byte_data + ptr, 2);
					ptr = ptr + 2;
					//��Сֵ
					memcpy(&sub_field.min, byte_data + ptr, 8);
					ptr = ptr + 8;
					//���ֵ
					memcpy(&sub_field.max, byte_data + ptr, 8);
					ptr = ptr + 8;
					//��λ
					memcpy(sub_field.unit, byte_data + ptr, 8);
					ptr = ptr + 8;
					//��ʾ��ʶ
					memcpy(&sub_field.display, byte_data + ptr, 1);
					ptr = ptr + 1;
					fields.push_back(sub_field);
				}
				//������
				MySQLInterface* db = new MySQLInterface;
				//�������ݿ�
				if (db->connectMySQL("127.0.0.1", "root", "", "teleinfodb", 3306)) {
					//��ѯ�Ƿ��иö����ֶ�
					string f_sql = "select * from �ֶζ���� where �豸�� =";
					f_sql = f_sql + "'" + name + "';";
					//cout << f_sql << endl;
					vector<vector <string>>res;
					if (!db->getDatafromDB(f_sql, res)) {
						cout << db->errorNum << endl;
						cout << db->errorInfo << endl;
					}
					if (res.size()!=0) {
						cout << name << "�豸�ڶ�������Ѿ�����" << endl;
					}
					else {
						//�������򴴽�
						
						//�����ֶζ���
						for (int i = 0; i < fields.size(); i++) {
							//���������sql���
							string sql = "insert into �ֶζ���� (�ֶ���,��������,���ֵ,��Сֵ,��λ,��ʾ��־,�豸��) values(";
							sql = sql + "'" + fields[i].name + "',";
							sql = sql + numToString<UINT16>(fields[i].type) + ",";
							//���û�������Сֵ
							if (fields[i].max == NEW_NULL) {
								sql = sql + "NULL,";
							}
							else {
								sql=sql+ numToString<double>(fields[i].max) + ",";
							}
							if (fields[i].min == NEW_NULL) {
								sql = sql + "NULL,";
							}
							else {
								sql = sql + numToString<double>(fields[i].min) + ",";
							}
							
							sql = sql + "'" + fields[i].unit + "',";
							sql = sql + numToString<bool>(fields[i].display) + ",";
							sql = sql + "'" + name + "');";
							//cout << sql << endl;
							if (!db->writeDataToDB(sql)) {
								cout << db->errorNum << endl;
								cout << db->errorInfo << endl;
							}
						}
						//���������ݵı�
						string d_sql = "create table ";
						d_sql = d_sql + name + " (���� INT AUTO_INCREMENT,ʱ�� BIGINT,";
						for (int i = 0; i < fields.size(); i++) {
							d_sql = d_sql + fields[i].name + " " + getType(fields[i].type);
							if (i != fields.size() - 1)d_sql = d_sql + ",";
						}
						d_sql = d_sql + ",primary key(����));";
						//cout << d_sql << endl;
						if (!db->writeDataToDB(d_sql)) {
							//cout << db->errorNum << endl;
							//cout << db->errorInfo << endl;
						}
						//������ϵ��
						string r_sql = "insert into �豸��ϵ��(�豸��,���豸��) values('";
						r_sql = r_sql + name + "','" + parentName + "');";
                        if (!db->writeDataToDB(r_sql)) {
							//cout << db->errorNum << endl;
							//cout << db->errorInfo << endl;
						}
						//Sleep(10);
						//���ñ���ɾ��
						EnterCriticalSection(&g_CS);//����ؼ������
													//���ĳ�-1
						MESSAGE_VECTOR.erase(MESSAGE_VECTOR.begin() + i);
						i--;
						LeaveCriticalSection(&g_CS);//�뿪�ؼ������

					}
					//�ر�����
					db->closeMySQL();
				}
				else {
					//cout << db->errorNum << endl;
					//cout << db->errorInfo << endl;
				}
				
			}
			else if (type == 2000) {
				//�������ݱ���
				//����ָ��
				int ptr = 4;//�Ѿ����������ֽ�
				//ʱ���
				long long timestamp;
				memcpy(&timestamp, byte_data + ptr, sizeof(long long));
				ptr = ptr + sizeof(long long);
				//���ܱ�ʶ
				bool flag;
				memcpy(&flag, byte_data + ptr, sizeof(bool));
				ptr = ptr + sizeof(bool);
				//�豸��
				char name[40];
				memcpy(name, byte_data + ptr, 40);
				//����ǵ�һ������Ҫ���
				if (MAP[name] == 0) {
					MAP[name] = 1;
					MySQLInterface* db_test = new MySQLInterface;
					string sql = "insert into ϵͳ��־�� (����,�¼�����,����) values ('ͨ��ģ��',1021,'ͨ�����Ļ��յ���һ��";
					sql = sql + name + "ң�ⱨ�����ݱ���');";
					cout << "| ͨ��ģ�� | ";
					cout << getTime();
					cout << "| �������ݱ���";
					cout.setf(ios::left);
					cout.width(37);
					cout << name;
					cout << "|" << endl;
					if (db_test->connectMySQL("127.0.0.1", "root", "", "mangeinfodb", 3306)) {
						db_test->writeDataToDB(sql);
						db_test->closeMySQL();
					}
					else {
						//����ʧ��
						//cout << db_test->errorNum << ":" << db_test->errorInfo << endl;
						return 0;
					}
				}
				ptr = ptr + 40;

				//������ݿ��д��ֶ�
				//ʶ���������
				MySQLInterface* db = new MySQLInterface;
				//�������ݿ�
				if (db->connectMySQL("127.0.0.1", "root", "", "teleinfodb", 3306)) {
					
					//������
					//��ѯ�ֶ���������
					vector<vector <string>>res;
					string ff_sql = "select ��������,�ֶ��� from �ֶζ���� where �豸��=";
					ff_sql = ff_sql + "'" + name + "' order by ����;";
					if (db->getDatafromDB(ff_sql, res)) {
						if (res.size() == 0) {
							cout << name << "�豸������" << endl;
						}
						//���ݿ�������
						else {
							string sql = "insert into ";
							sql = sql + name + "(ʱ��,";
							string d_sql = " values(";
							d_sql = d_sql + numToString<long long>(timestamp) + ",";
							for (int i = 0; i < res.size(); i++) {
								//���ͺ���
								UINT16 code = 0;
								code = stringToNum<UINT16>(res[i][0]);
								sql = sql + res[i][1];
								if (i < res.size() - 1)sql = sql + ",";
								if (i == res.size() - 1)sql = sql + ")";
								if (code == 1) {//16λ����
									INT16 data = 0;
									memcpy(&data, byte_data + ptr, sizeof(INT16));
									ptr = ptr + sizeof(INT16);
									d_sql = d_sql + numToString<INT16>(data);
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 2) {//16λ�޷�������
									UINT16 data = 0;
									memcpy(&data, byte_data + ptr, sizeof(UINT16));
									ptr = ptr + sizeof(UINT16);
									d_sql = d_sql + numToString<UINT16>(data);
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 3) {//32λ����
									INT32 data = 0;
									memcpy(&data, byte_data + ptr, sizeof(INT32));
									ptr = ptr + sizeof(INT32);
									d_sql = d_sql + numToString<INT32>(data);
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 4) {//32λ�޷�������
									UINT32 data = 0;
									memcpy(&data, byte_data + ptr, sizeof(UINT32));
									ptr = ptr + sizeof(UINT32);
									d_sql = d_sql + numToString<UINT32>(data);
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 5) {//64λ����
									INT64 data = 0;
									memcpy(&data, byte_data + ptr, sizeof(INT64));
									ptr = ptr + sizeof(INT64);
									d_sql = d_sql + numToString<INT64>(data);
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 6) {//64λ�޷�������
									UINT64 data = 0;
									memcpy(&data, byte_data + ptr, sizeof(UINT64));
									ptr = ptr + sizeof(UINT64);
									d_sql = d_sql + numToString<UINT64>(data);
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 7) {//�����ȸ�����ֵ
									float data = 0;
									memcpy(&data, byte_data + ptr, sizeof(float));
									ptr = ptr + sizeof(float);
									d_sql = d_sql + numToString<float>(data);
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 8) {//˫���ȸ�����ֵ
									double data = 0;
									memcpy(&data, byte_data + ptr, sizeof(double));
									ptr = ptr + sizeof(double);
									d_sql = d_sql + numToString<double>(data);
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 9) {//�ַ�����
									char data;
									memcpy(&data, byte_data + ptr, sizeof(char));
									ptr = ptr + sizeof(char);
									d_sql = d_sql + "'" +data + "'";
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 10) {//���ַ�������
									char data[15];
									memcpy(&data, byte_data + ptr, sizeof(char) * 15);
									ptr = ptr + sizeof(char) * 15;
									d_sql = d_sql + "'" + data + "'";
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 11) {//���ַ�������
									char data[255];
									memcpy(&data, byte_data + ptr, sizeof(char) * 255);
									ptr = ptr + sizeof(char) * 255;
									d_sql = d_sql + "'" + data + "'";
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								//��bug
								else if (code == 12) {//���ֽ�����
									char data[255];
									memcpy(&data, byte_data + ptr, sizeof(char) * 255);
									ptr = ptr + sizeof(char) * 255;
									d_sql = d_sql + "'" + data + "'";
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 13) {//���ֽ�����
									char data[32 * K];
									memcpy(&data, byte_data + ptr, sizeof(char) * 32 * K);
									ptr = ptr + sizeof(char) * 32 * K;
									d_sql = d_sql + "'" + data + "'";
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 14) {//���ֽ�����
									char data[60 * K];
									memcpy(&data, byte_data + ptr, sizeof(char) * 60 * K);
									ptr = ptr + sizeof(char) * 60 * K;
									d_sql = d_sql + "'" + data + "'";
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 15) {//������
									bool data;
									memcpy(&data, byte_data + ptr, sizeof(bool));
									ptr = ptr + sizeof(bool);
									d_sql = d_sql + numToString<bool>(data);
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 16) {//ʱ���
									long long data;
									memcpy(&data, byte_data + ptr, sizeof(long long));
									ptr = ptr + sizeof(long long);
									d_sql = d_sql + numToString<long long>(data);
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else {
									cout << "δ�����������ʹ���" << endl;
								}
							}
							d_sql = d_sql + ");";
							sql = sql + d_sql;
							//cout << sql << endl;
							if (!db->writeDataToDB(sql)) {
								//cout << db->errorNum << endl;
								//cout << db->errorInfo << endl;
							}
							//Sleep(10);
							EnterCriticalSection(&g_CS);//����ؼ������
														//���ĳ�-1
							MESSAGE_VECTOR.erase(MESSAGE_VECTOR.begin() + i);
							i--;
							LeaveCriticalSection(&g_CS);//�뿪�ؼ������
						}
						
					}
					else {
						//cout << db->errorNum << endl;
						//cout << db->errorInfo << endl;
					}
					//�ر�����
					db->closeMySQL();
				}
				else {
					//cout << db->errorNum << endl;
					//cout << db->errorInfo << endl;
				}
			}
			else {
				EnterCriticalSection(&g_CS);//����ؼ������
				STOP = 1;
				LeaveCriticalSection(&g_CS);//�뿪�ؼ������
				//������
				//cout << "δ֪���ĸ�ʽ" << endl;
			}
			
			//���±��ĳصĴ�С
			//Sleep(1);
			EnterCriticalSection(&g_CS);//����ؼ������
			setLen =(int) MESSAGE_VECTOR.size();
			//system("cls");
			//cout << "Ŀǰ���ĳش�С��" << MESSAGE_VECTOR.size() << endl;
			LeaveCriticalSection(&g_CS);//�뿪�ؼ������
		}
		
	}
	return 0;
}


