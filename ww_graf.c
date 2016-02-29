#include "service.h"

extern WINDOW *stdscr;
WINDOW *mpanel[3];
extern int end;

//�������������� ����� ��� ������
void InitScreen(void)
{
    end=0;
    initscr();
    cbreak();
    noecho();
    clear();
    curs_set(0);
    start_color();
    init_pair(1,COLOR_YELLOW,COLOR_BLUE);
    init_pair(2,COLOR_RED,COLOR_BLUE);
    //
    mpanel[0]=newwin(getmaxy(stdscr)-3,getmaxx(stdscr)*3/4-1,0,0);
    mpanel[1]=newwin(getmaxy(stdscr),getmaxx(stdscr)/4,0,getmaxx(stdscr)*3/4-1);
    mpanel[2]=newwin(3,getmaxx(stdscr)*3/4-1,getmaxy(stdscr)-3,0);
    //
    wbkgd(mpanel[0],COLOR_PAIR(1));
    wbkgd(mpanel[1],COLOR_PAIR(1));
    wbkgd(mpanel[2],COLOR_PAIR(1));
    //
    wattron(mpanel[0],COLOR_PAIR(1)| A_BOLD);
    box(mpanel[0],0,0);
    wmove(mpanel[0],0,getmaxx(mpanel[0])/2-14);
    wprintw(mpanel[0]," Messages, press F5 to quit ");
    //
    wattron(mpanel[1],COLOR_PAIR(1)| A_BOLD);
    box(mpanel[1],0,0);
    wmove(mpanel[1],0,getmaxx(mpanel[1])/2-4);
    wprintw(mpanel[1]," Members ");
    //
    wattron(mpanel[2],COLOR_PAIR(1)| A_BOLD);
    box(mpanel[2],0,0);
    wmove(mpanel[2],0,getmaxx(mpanel[2])/2-13);
    wprintw(mpanel[2]," Type your message, %ld ",(long)getpid());
    //
    curs_set(1);
    keypad(mpanel[2],TRUE);
    wmove(mpanel[2],1,1);
    wrefresh(mpanel[1]);
    wrefresh(mpanel[0]);
    wrefresh(mpanel[2]);
}

//������ ������ � ���� �� ����
//0 - �����
//1 - ������
void PrintStr(int pos, int panel, char *str, long pid)
{
    wmove(mpanel[panel],pos,1);
    wprintw(mpanel[panel],"%ld:",pid);
    if(panel==0)
    {
        wprintw(mpanel[panel],str);
        box(mpanel[panel],0,0);
        wmove(mpanel[panel],0,getmaxx(mpanel[0])/2-14);
        wprintw(mpanel[panel]," Messages, press F5 to quit ");
    }
    wmove(mpanel[2],1,1);
    wrefresh(mpanel[panel]);
}

//������� PrintStr, ���� ��� �������� � �������� �������
void PrintMsg(int pos,char *message,long int pid)
{
    PrintStr(pos,0,message,pid);
}

//������� PrintStr, ���� ��� �������� � �������� �������
void PrintClient(int pos,long int pid)
{
    PrintStr(pos,1,NULL,pid);
}

//������� ������, 0/1/2 �� ������� �������
void ClearPanel(int panel)
{
    wclear(mpanel[panel]);
    wattron(mpanel[panel],COLOR_PAIR(1)| A_BOLD);
    box(mpanel[panel],0,0);
    if(panel==0)
    {
        wmove(mpanel[0],0,getmaxx(mpanel[0])/2-14);
        wprintw(mpanel[0]," Messages, press F5 to quit ");
    }
    if(panel==1)
    {
        wmove(mpanel[panel],0,getmaxx(mpanel[panel])/2-4);
        wprintw(mpanel[panel]," Members ");
    }
    if(panel==2)
    {
        wmove(mpanel[2],0,getmaxx(mpanel[2])/2-13);
        wprintw(mpanel[2]," Type your message, %ld ",(long)getpid());
    }
    wrefresh(mpanel[panel]);
}

//������� ������, ���������� ������ � �������
void DeleteScreen(void)
{
    delwin(mpanel[0]);
    delwin(mpanel[1]);
    delwin(mpanel[2]);
    endwin();
    clear();
}

//�������� ��������� �� ������ �����
void GetMessage(char *str)
{
    mvwinstr(mpanel[2],1,1,str);
    str[getmaxx(mpanel[2])-2]='\0';
}

//����������� ���� � �������� ���������
void MessageType(void)
{
    int ch,x,y,i=0;//,j=1;
    char message[getmaxx(mpanel[2])-1];
    while((ch=wgetch(mpanel[2]))!=KEY_F(5)&&end==0)
    {
        wrefresh(mpanel[2]);
        //��������� enter
        if(ch==10)
        {
            GetMessage(message);
            ClearPanel(2);
            wmove(mpanel[2],1,1);
            i=0;
            ClientSendMsg(message);
        }
        else if(ch==KEY_BACKSPACE)
        {
            getyx(mpanel[2],y,x);
            if(x!=1)
            {
                wmove(mpanel[2],y,x-1);
                wdelch(mpanel[2]);
                box(mpanel[2],0,0);
                wmove(mpanel[2],0,getmaxx(mpanel[2])/2-13);
                wprintw(mpanel[2]," Type your message, %ld ",(long)getpid());
                wmove(mpanel[2],1,getmaxx(mpanel[2])-2);
                wprintw(mpanel[2]," ");
                wmove(mpanel[2],y,x-1);
                i--;
            }
        }
        else
        {
            //�� ���� �������� �� �������
            if(i>=getmaxx(mpanel[2])-2)
            {
                continue;
            }
            wprintw(mpanel[2],"%c",ch);
            i++;
        }
    }
}

//�������� ������� �� ������
void AddClient(long pid)
{
    int pos;
    char s;
    //���� ������ ��������� ������ �� �������
    for(pos=1;pos<200;pos++)
    {
        s=mvwinch(mpanel[1],pos,1);
        if(s==' ')
        {
            break;
        }
    }
    PrintClient(pos,pid);
}

//�������� ������� � ���������� ������ �������
void RemoveClient(int pos)
{
    int i,p;
    p=pos;
    char c,st='q';
    //������� � �������� ������� ������ ���������� �������� ������� ������ ������
    while(st!=' ')
    {
        wmove(mpanel[1],p,1);
        for(i=1;;i++)
        {
            c=mvwinch(mpanel[1],p+1,i);
            wmove(mpanel[1],p,i);
            wprintw(mpanel[1],"%c",c);
            if(c==' ')
            {
                wprintw(mpanel[1],"    ");
                break;
            }
        }
        p++;
        st=mvwinch(mpanel[1],p,1);
    }
    wrefresh(mpanel[1]);
}

//�������� ������� �������
//�� ����� pid �������, �� ������ 0 ��� ���������� ������ � ������� ������ ���
//����������
int CheckClient(long pid)
{
    int pos,i;
    long cmp;
    char str[10];
    for(pos=1;;pos++)
    {
        for(i=1;i<10;i++)
        {
            str[i-1]=mvwinch(mpanel[1],pos,i);
            if(str[0]==' ')
            {
                return 0; //������ ������� ���
            }
            if(str[i-1]==':')
            {
                str[i-1]='\0';
                break;
            }
        }
        cmp=atol(str);
        if(cmp==pid)
        {
            return pos;
        }

    }

}
