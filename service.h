#ifndef _SERVICE_H
#define _SERVICE_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <ncurses.h>
#include <pthread.h>
#include <time.h>
#include <sys/syscall.h>
#include <errno.h>
#include <malloc.h>


//��������� ��� ftok
#define FTOK_PATH "/home/"
#define FTOK_INT(x) 'a'+x

//������������ ����� ������ ���������
#define MSG_MAX_SZ 120

#define MAX_CLIENT_NUM 50

#define KEYNUM 4
#define SEMNUM 3
//������� id ��� ����������
#define semcounter semid[0]
#define semlocker semid[1]
#define semwait semid[2]

//�������� ���������
struct My_msg
{
    long mtype;
    char msg[MSG_MAX_SZ]; //����� ���������
    pid_t pid;       //���� ��� ���� ����������� ���������
    int registered;  //���� ��� �������� ������ �����������
};

//----------------------��������� �����----------------------------
//��������/�����������/��������
int ProcInit(void);
void ExitFunc(void);
//��������� ������� ��� ���������� ������
void IntHandler(int arg);


//---------------------���������� �����----------------------------
//�������
//������������� ������
void InitScreen(void);
//������ � ���� ��������� id ������� � ���������
void PrintMsg(int pos, char *message, long pid);
//������ id � ���� ���������
void PrintClient(int pos, long pid);
//������� ���� 0-1-2: �����-�������-����
void ClearPanel(int panel);
//������� ������
void DeleteScreen(void);
//������ ������
void PrintStr(int pos, int panel, char *str, long int pid);
//������� ��������� �� ������ �����
void GetMessage(char *str);
//������ � �������� ��������� (����������� ��������� �����)
void MessageType(void);
//�������� ������� �� ������
void AddClient(long int pid);
//������� ������� � ������
void RemoveClient(int pos);
//��������� ������� ������� �� ������
int CheckClient(long pid);

//������ �������
//������������� �������
void InitSystem(void);
//�����������/���������� �������
int ClientConnectChat(int type);
//�������� ��������� �� ������
void ClientSendMsg(char* msg);

void* ReceiverFunc(void *arg);
void USR1Handler(int arg);

void EndClient(void);

void USR2Handler(int arg);

#endif
