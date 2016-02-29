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


//параметры для ftok
#define FTOK_PATH "/home/"
#define FTOK_INT(x) 'a'+x

//максимальная длина текста сообщения
#define MSG_MAX_SZ 120

#define MAX_CLIENT_NUM 50

#define KEYNUM 4
#define SEMNUM 3
//именуем id для понятности
#define semcounter semid[0]
#define semlocker semid[1]
#define semwait semid[2]

//описание сообщения
struct My_msg
{
    long mtype;
    char msg[MSG_MAX_SZ]; //текст сообщения
    pid_t pid;       //поле для пида отправителя сообщения
    int registered;  //поле для передачи номера регистрации
};

//----------------------серверная часть----------------------------
//создание/подключение/удаление
int ProcInit(void);
void ExitFunc(void);
//обработка сигнало для завершения работы
void IntHandler(int arg);


//---------------------клиентская часть----------------------------
//графика
//инициализация экрана
void InitScreen(void);
//печать в окно сообщений id клиента и сообщение
void PrintMsg(int pos, char *message, long pid);
//печать id в окно сообщений
void PrintClient(int pos, long pid);
//очистка окна 0-1-2: сообщ-клиенты-ввод
void ClearPanel(int panel);
//очистка экрана
void DeleteScreen(void);
//печать строки
void PrintStr(int pos, int panel, char *str, long int pid);
//достать сообщение из строки ввода
void GetMessage(char *str);
//печать и отправка сообщений (циклическая обработка ввода)
void MessageType(void);
//добавить клиента на панель
void AddClient(long int pid);
//удалить клиента с панели
void RemoveClient(int pos);
//проверить наличие клиента на панели
int CheckClient(long pid);

//работа клиента
//инициализация системы
void InitSystem(void);
//полключение/отключение сервера
int ClientConnectChat(int type);
//отправка сообщения на сервер
void ClientSendMsg(char* msg);

void* ReceiverFunc(void *arg);
void USR1Handler(int arg);

void EndClient(void);

void USR2Handler(int arg);

#endif
