/*
 * ��� ���������� ������� ��������� ����� -D server -lpthread, ����� ww_que.c main.c
 * ��� ���������� ������� ��������� ����� -D chat -lpthread -lncurses, ����� main.c ww_graf.c ww_thr.c ps_clients.c
 *
 * */

#include "service.h"

//extern int end;
//extern pthread_t msgwait,swait;

int main(void)
{
#ifdef chat
    InitScreen();
    InitSystem();
    ClientConnectChat(1);
    MessageType();
    ClientConnectChat(0);

    DeleteScreen();
    EndClient();

#endif
#ifdef server

    ProcInit();
    printf("server end \n");

#endif
#ifdef test

    InitSystem();

#endif
    return 0;
}

