#include "service.h"

int shmid,semid[SEMNUM],end,*num,position=0,usr1=0;
void *ptr;
pid_t *pid[MAX_CLIENT_NUM];
struct My_msg *msg;
//блок-разблок достпа к памяти, счетчик прочитавших, ожидание сброса счетчика в ноль
struct sembuf lock[2]={{0,0,0},{0,1,0}},unlock={0,-1,0},setncount={0,1,0},wait0={0,0,0};
pthread_t receiver;

void InitSystem(void)
{
    int i;
    key_t key[KEYNUM];
    size_t shmsize=(MAX_CLIENT_NUM)*sizeof(pid_t)+sizeof(struct My_msg)+sizeof(int);
    for(i=0;i<KEYNUM;i++)
    {
        key[i]=ftok(FTOK_PATH,FTOK_INT(i));
    }
    end=0;
    usr1=0;
    //
    shmid=shmget(key[0],shmsize,0);
    if(shmid==-1)
    {
        perror("shmget\n");
        exit(1);
    }
    for(i=0;i<SEMNUM;i++)
    {
        semid[i]=semget(key[i+1],1,0);
        if(semid[i]==-1)
        {
            perror("semget\n");
            exit(1);
        }
    }
    ptr=shmat(shmid,NULL,0);
    num=(int*)ptr;
    msg=(struct My_msg*)(ptr+sizeof(int));
    pid[0]=(pid_t*)(ptr+sizeof(int)+sizeof(struct My_msg));
    if(*num==0)
    {
        for(i=0;i<MAX_CLIENT_NUM;i++)
        {
            *(pid[0]+i*sizeof(pid_t*))=0;
        }
    }
    signal(SIGUSR1,USR1Handler);
    signal(SIGUSR2,USR2Handler);
    pthread_create(&receiver,NULL,ReceiverFunc,NULL);
}

void EndClient(void)
{

    shmdt(ptr);
}

//1 - подключить, 0 -отключить
int ClientConnectChat(int type)
{
    int i;
    pid_t p1=getpid();
    semop(semlocker,lock,2);
    //додобвляем инфу о себе
    if(type>0)
    {
        *num+=1;
        for(i=0;i<MAX_CLIENT_NUM;i++)
        {

            if(*(pid[0]+i*sizeof(pid_t*))==0)
            {
                *(pid[0]+i*sizeof(pid_t*))=p1;
                break;
            }
        }
    }
    //или убираем
    else
    {
        *num-=1;
        for(i=0;i<MAX_CLIENT_NUM;i++)
        {
            if(*(pid[0]+i*sizeof(pid_t*))==p1)
            {
                *(pid[0]+i*sizeof(pid_t*))=0;
                break;
            }
        }
    }
    semop(semlocker,&unlock,1);
    if(type>0)
    ClientSendMsg("connected to chat");
    else
    {
        ClientSendMsg("disconnected from chat");
        for(i=0;i<MAX_CLIENT_NUM;i++)
        {
            if(*(pid[0]+i*sizeof(pid_t*))!=0)
            {
                //semop(semlocker,&unlock,1);
                kill(*(pid[0]+i*sizeof(pid_t*)),SIGUSR2);
                //semop(semlocker,lock,2);
            }
        }
    }
    return 0;
}

void ClientSendMsg(char* message)
{
    int i;
    semop(semlocker,lock,2);
    strncpy(msg->msg,message,MSG_MAX_SZ-1);
    msg->pid=getpid();
    for(i=0;i<*num;i++)
    {
        semop(semcounter,&setncount,1);
    }
    for(i=0;i<MAX_CLIENT_NUM;i++)
    {
        if(*(pid[0]+i*sizeof(pid_t*))!=0)
        {
            semop(semlocker,&unlock,1);
            kill(*(pid[0]+i*sizeof(pid_t*)),SIGUSR1);
            semop(semlocker,lock,2);
        }
    }
    semop(semlocker,&unlock,1);
    //semop(semcounter,&wait0,1);
    //position++;
    //PrintMsg(position,message,(long)msg->pid);
}

void* ReceiverFunc(void *arg)
{
    //int i,chk;
    while(1)
    {
        pause();
//        if(usr1==1)
//        {

//            usr1=0;
//        }
    }
}

void USR1Handler(int arg)
{
    int chk,i;
    //usr1=1;
    semop(semlocker,lock,2);
    position++;
    PrintMsg(position,msg->msg,(long)msg->pid);
    for(i=0;i<MAX_CLIENT_NUM;i++)
    {
        if(*(pid[0]+i*sizeof(pid_t*))!=0)
        {
            chk=CheckClient((long)*(pid[0]+i*sizeof(pid_t*)));
            if(chk==0)
            {
                AddClient((long)*(pid[0]+i*sizeof(pid_t*)));
            }
        }
    }
    semop(semcounter,&unlock,1);
    semop(semlocker,&unlock,1);
}

void USR2Handler(int arg)
{
    int pos,i;
    for(i=0;i<MAX_CLIENT_NUM;i++)
    {
        if(*(pid[0]+i*sizeof(pid_t*))==msg->pid)
        {
            pos=CheckClient((long)msg->pid);
            if(pos>0)
            {
                RemoveClient(pos);
            }
        }
    }
}
