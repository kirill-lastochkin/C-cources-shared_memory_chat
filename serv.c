#include "service.h"



int shmid,semid[SEMNUM],end;
key_t key[KEYNUM];
size_t shmsize=(MAX_CLIENT_NUM)*sizeof(pid_t)+sizeof(struct My_msg)+sizeof(int);

int ProcInit(void)
{

    int i,*num,chk;
    //pid_t *pid[MAX_CLIENT_NUM];
    //struct My_msg *msg;
    void *ptr;

    for(i=0;i<KEYNUM;i++)
    {
        key[i]=ftok(FTOK_PATH,FTOK_INT(i));
    }
    end=0;
    atexit(ExitFunc);
    //стрктура разделяемой памяти:
    //int количество клиентов на данный момент
    //struct My_msg сообщение, которое всем нжно напечатать у себя
    //pid_t для каждого подключенно клиента
    shmid=shmget(key[0],shmsize,IPC_CREAT|0666|IPC_EXCL);
    if(shmid==-1)
    {
        if(errno==EEXIST)
        {
            shmid=shmget(key[0],shmsize,0);
            shmctl(shmid,IPC_RMID,NULL);
            shmid=shmget(key[0],shmsize,IPC_CREAT|0666|IPC_EXCL);
            if(shmid==-1)
            {
                perror("creat\n");
                exit(1);
            }
        }
        else
        {
            perror("creat\n");
            exit(1);
        }
    }
    //семафоры создаем
    for(i=0;i<SEMNUM;i++)
    {
        semid[i]=semget(key[i+1],1,IPC_CREAT|0666|IPC_EXCL);
        if(semid[i]==-1)
        {
            if(errno==EEXIST)
            {
                semid[i]=semget(key[i+1],1,0);
                chk=semctl(semid[i],0,IPC_RMID);
                if(chk==-1)
                {
                    perror("remove sem\n");
                }
                semid[i]=semget(key[i+1],1,IPC_CREAT|0666|IPC_EXCL);
                if(semid[i]==-1)
                {
                    perror("creat sem1\n");
                    exit(1);
                }
            }
            else
            {
                perror("creat sem2\n");
                exit(1);
            }
        }
    }
    ptr=shmat(shmid,NULL,0);
    num=(int*)ptr;
    //msg=(struct My_msg*)(ptr+sizeof(int));
    *num=0;
    signal(SIGINT,IntHandler);
    while(end==0)
    {
        pause();
    }
    return 0;
}


void IntHandler(int arg)
{
    end=1;
    ExitFunc();
}


void ExitFunc(void)
{
    int i;
    shmid=shmget(key[0],shmsize,IPC_CREAT|0666|IPC_EXCL);
    if(shmid==-1&&errno==EEXIST)
    {
        shmid=shmget(key[0],shmsize,0);
        shmctl(shmid,IPC_RMID,NULL);
    }
    for(i=0;i<SEMNUM;i++)
    {
        semid[i]=semget(key[i+1],1,IPC_CREAT|0666|IPC_EXCL);
        if(semid[i]==-1)
        {
            if(errno==EEXIST)
            semid[i]=semget(key[i+1],1,0);
        }
        semctl(semid[i],0,IPC_RMID);
    }
}
