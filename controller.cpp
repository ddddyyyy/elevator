//
//  controller.cpp
//  my_elevator
//
//  Copyright  2018 mdy. All rights reserved.
//

#include "controller.hpp"

int MAX_ELEVATOR=5;
int MAX_FLOOR=5;

Activity *activities;//ʱ��Ļ��n��
Elevator *elevator;//����

int mode;//��ʾģʽ
int CurrentTime=0;//��ǰ��ʱ��
int TotalTime=0;//���е���ʱ��
int GiveupTime=0;//�û����������ʱ��
int EnterTime=0;//���û�������ʱ��
int UserId = 0;//�û�id
int ElevatorId = 0;//����id
int LeavePeople=0;//�뿪���û�
int ServerdPeople=0;//������û�
int TotalWaitTime=0;//�û��ȴ���ʱ��

#define HEIGHT 25
#define WIDTH 80

using namespace std;

void gotoxy(int x,int y){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cci;
    GetConsoleCursorInfo(hOut,&cci);
    COORD pos;
    pos.X=x;
    pos.Y=y;
    SetConsoleCursorPosition(hOut,pos);
}

void HideCarsor(){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cci;
    GetConsoleCursorInfo(hOut,&cci);
    cci.bVisible=false;
    SetConsoleCursorInfo(hOut,&cci);
}


void PrintInfo(){
    printf("������Ŀ��%d\n",MAX_ELEVATOR);
    printf("¥����Ŀ��%d\n",MAX_FLOOR);
    printf("ʱ�䣺    /%d\n\n",TotalTime);

    for(int i=0;i<MAX_ELEVATOR;++i){
        printf("����%d ��ǰ¥��:   ״̬:\n",elevator[i].id);
    }
    for(int i=0;i<MAX_ELEVATOR;++i){
        gotoxy(WIDTH/2-1+i*6+4,0);
        cout<<"����"<<i<<" ";
    }
    for(int i=MAX_FLOOR-1;i>=0;--i){
        gotoxy(WIDTH/2-1,MAX_FLOOR-i+1);
        cout<<"F"<<i;
    }
}

string ConditionToString(Condition c){
    switch (c) {
    case Idle:
        return "����    ";
    case Opening:
        return "������  ";
    case Closing:
        return "������  ";
    case Moving:
        return "�ƶ���  ";
    case Decelerate:
        return "������  ";
    case Closed:
        return "������  ";
    case Opened:
        return "�Ŵ���";
    case Add:
        return "�û��ȴ�";
    default:
        return "δ֪    ";
    }
}

void PrintTime(){
    gotoxy(6,2);
    printf("%d",CurrentTime);
    for(int i=0;i<MAX_ELEVATOR;++i){
        gotoxy(15,4+i);
        cout<<elevator[i].floor<<" ";
        gotoxy(23,4+i);
        cout<<ConditionToString(elevator[i].condition);
        for(int j=MAX_FLOOR-1;j>=0;--j){
            gotoxy(WIDTH/2-1+i*6+4,MAX_FLOOR-j+1);
            if(elevator[i].floor==j){
                cout<<"["<<elevator[i].peopleNum<<"]";
            }else{
                cout<<" "<<elevator[i].w_q[j][1].rear+elevator[i].w_q[j][0].rear-2<<" ";
            }
        }
    }
    sleep(0.1);
}

/**
 ����µ��¼�
 @param time ʱ�������ʱ��
 @param fn Ҫ�������¼�
 */
void AddActivity(int time,Condition(*fn)(Elevator&),Elevator &e){
    Activity *temp = (Activity*)malloc(sizeof(Activity));
    temp->e=&e;
    temp->fn=fn;
    temp->endTime=time;
    temp->next=NULL;
    if(NULL==activities){
        activities=temp;
    }else{
        Activity *act=activities;
        while (act->next!=NULL&&act->next->endTime<=temp->endTime) {
            act=act->next;
        }
        temp->next=act->next;
        act->next=temp;
    }
}

/**
 ����û��¼�
 */
Condition AddPeople(Elevator &e){
    //��ʼ���û� start
    People *p = (People*)malloc(sizeof(People));
    if(NULL==p)return None;
    p->id = UserId++;
    p->GiveupTime = rand()%GiveupTime+CurrentTime+220;
    p->inTime=CurrentTime;
    p->InFloor=rand()%MAX_FLOOR;
    do{
        p->OutFloor=rand()%MAX_FLOOR;
    }while(p->InFloor==p->OutFloor);
    //��ʼ���û� end
    Status s;
    //�ж�����¥
    if(p->InFloor>p->OutFloor){//��¥
        s = EnQueue(e.w_q[p->InFloor][GoingDown],*p);
        e.CallDown[p->InFloor]=true;
    }else{
        s = EnQueue(e.w_q[p->InFloor][GoingUp],*p);
        e.CallUp[p->InFloor]=true;
    }
    if(s==YES)//printf("�û�%d������%d�ŵ����µ�%d¥����Ҫȥ%d¥\n",p->id,e.id,p->InFloor,p->OutFloor);
    //��һ���û��������¼����
    AddActivity(CurrentTime+rand()%EnterTime,AddPeople,elevator[rand()%MAX_ELEVATOR]);
    e.count=0;
    return Add;
}

/**
 �û��������¼�
 */
Condition InAndOut(Elevator &e){
    People p;
    //˵����������Ҫ����
    if(e.CallCar[e.floor]){
        //�������뿪
        if(IsEmpty_S(e.i_s[e.floor])!=YES){
            Pop(e.i_s[e.floor],p);
            e.peopleNum--;
           // printf("�ͻ�%d�뿪%d�ŵ�����\n",p.id,e.id);
            AddActivity(CurrentTime+MOVE_TIME,InAndOut,e);
        }else{
            e.CallCar[e.floor]=false;
        }
    }else
    {
        //����Ҫ�߽�����
        if(e.peopleNum<MAX_PEOPLE&&YES!=IsEmpty_Q(e.w_q[e.floor][e.state])){
            DeQueue(e.w_q[e.floor][e.state],p);
            Push(e.i_s[p.OutFloor],p);
            e.peopleNum++;
            e.CallCar[p.OutFloor]=true;
            AddActivity(CurrentTime+MOVE_TIME,InAndOut,e);
            ServerdPeople++;
            TotalWaitTime+=CurrentTime-p.inTime;
        }else{
            e.state==GoingDown?e.CallDown[e.floor]=false:e.CallUp[e.floor]=false;
            AddActivity(CurrentTime,Close,e);
        }
    }
    return None;
}

void init(){
    srand(time(0));
    while(1){
        printf("��������ݵ���Ŀ(���5):");
        while(!scanf("%d",&MAX_ELEVATOR)||(MAX_ELEVATOR<1||MAX_ELEVATOR>6)){while(getchar()!='\n');printf("����ȷ����\n");}
        printf("������¥�����Ŀ(���20):");
        while(!scanf("%d",&MAX_FLOOR)||MAX_FLOOR<2||MAX_FLOOR>21){while(getchar()!='\n');printf("����ȷ����\n");}
        printf("��ѡ���Ƿ�̬��ʾ���ݵ�����״̬��1Ϊ�ǣ�0Ϊ��:");
        while(!scanf("%d",&mode)||(mode!=1&&mode!=0)){while(getchar()!='\n');printf("����ȷ����1��0\n");}
        printf("���������е���ʱ��:");
        while(!scanf("%d",&TotalTime)){while(getchar()!='\n');printf("����������\n");}
        printf("�������û��������ʱ��(Ĭ�ϵĵȴ�ʱ��Ϊ220���������ֵΪ������ֵ����220Ϊ���յĵȴ�ʱ��):");
        while(!scanf("%d",&GiveupTime)){while(getchar()!='\n');printf("����������\n");}
        printf("�������û�����ϵͳ������ʱ��(��С��200):");
        while(!scanf("%d",&EnterTime)||EnterTime<200){while(getchar()!='\n');printf("����ȷ����\n");}
//        TotalTime=5000;
//        EnterTime=200;
//        GiveupTime=30;
//        mode=1;
        activities=NULL;
        CurrentTime=0;
        UserId=0;
        ElevatorId=0;
        system("cls");
        elevator = (Elevator*)malloc(sizeof(Elevator)*MAX_ELEVATOR);
        for(int i=0;i<MAX_ELEVATOR;++i){
            InitElevator(elevator[i]);
            elevator[i].id=ElevatorId++;
        }
        run();
        printf("�����������\n");
        system("pause");
        system("cls");
    }
}

void run(){
    if(mode==1){
        HideCarsor();
        PrintInfo();
    }
    //����Ϊʱ�����һ���û�
    AddActivity(CurrentTime+rand()%EnterTime,AddPeople,elevator[rand()%MAX_ELEVATOR]);
    while(++CurrentTime<=TotalTime){
        //�û���������
        for(int m=0;m<MAX_ELEVATOR;++m){
            if(elevator[m].condition==Opened){
                InAndOut(elevator[m]);
            }
            for(int j=0;j<2;++j){
                for (int i=0;i<MAX_FLOOR;++i){
                    LeavePeople+=DeletePeople(elevator[m].w_q[i][j],CurrentTime);
                }
            }
        }
        while(NULL!=activities&&activities->endTime<=CurrentTime){
            switch (activities->fn(*(activities->e))) {
                case Opened:
                    AddActivity(CurrentTime+activities->e->count,Close,*activities->e);
                    break;
                case Decelerate:
                case Closing:
                case Opening:
                case Moving:
                    AddActivity(CurrentTime+activities->e->count,Stop,*activities->e);
                    break;
                case Add:
                case Idle:
                case Closed:
                    AddActivity(CurrentTime+activities->e->count,Open,*activities->e);
                    break;
                default:
                    break;
            };
            activities=activities->next;
        }
        if(mode==1){
            PrintTime();
        }
    }
    if (mode==1)gotoxy(0,HEIGHT);
    printf("һ������%d��,%d��û�ȵ��ݾ�����,������%d����,ƽ���ȴ�ʱ��Ϊ%f\n",UserId,LeavePeople,ServerdPeople,TotalWaitTime/(ServerdPeople*1.0));
}
