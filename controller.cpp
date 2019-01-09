//
//  controller.cpp
//  my_elevator
//
//  Copyright  2018 mdy. All rights reserved.
//

#include "controller.hpp"

int MAX_ELEVATOR=5;
int MAX_FLOOR=5;

Activity *activities;//时序的活动链n表
Elevator *elevator;//电梯

int mode;//显示模式
int CurrentTime=0;//当前的时间
int TotalTime=0;//运行的总时间
int GiveupTime=0;//用户放弃的最大时间
int EnterTime=0;//新用户到来的时间
int UserId = 0;//用户id
int ElevatorId = 0;//电梯id
int LeavePeople=0;//离开的用户
int ServerdPeople=0;//服务的用户
int TotalWaitTime=0;//用户等待的时间

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
    printf("电梯数目：%d\n",MAX_ELEVATOR);
    printf("楼层数目：%d\n",MAX_FLOOR);
    printf("时间：    /%d\n\n",TotalTime);

    for(int i=0;i<MAX_ELEVATOR;++i){
        printf("电梯%d 当前楼层:   状态:\n",elevator[i].id);
    }
    for(int i=0;i<MAX_ELEVATOR;++i){
        gotoxy(WIDTH/2-1+i*6+4,0);
        cout<<"电梯"<<i<<" ";
    }
    for(int i=MAX_FLOOR-1;i>=0;--i){
        gotoxy(WIDTH/2-1,MAX_FLOOR-i+1);
        cout<<"F"<<i;
    }
}

string ConditionToString(Condition c){
    switch (c) {
    case Idle:
        return "待机    ";
    case Opening:
        return "开门中  ";
    case Closing:
        return "关门中  ";
    case Moving:
        return "移动中  ";
    case Decelerate:
        return "减速中  ";
    case Closed:
        return "关门了  ";
    case Opened:
        return "门打开了";
    case Add:
        return "用户等待";
    default:
        return "未知    ";
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
 添加新的事件
 @param time 时间结束的时间
 @param fn 要发生的事件
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
 添加用户事件
 */
Condition AddPeople(Elevator &e){
    //初始化用户 start
    People *p = (People*)malloc(sizeof(People));
    if(NULL==p)return None;
    p->id = UserId++;
    p->GiveupTime = rand()%GiveupTime+CurrentTime+220;
    p->inTime=CurrentTime;
    p->InFloor=rand()%MAX_FLOOR;
    do{
        p->OutFloor=rand()%MAX_FLOOR;
    }while(p->InFloor==p->OutFloor);
    //初始化用户 end
    Status s;
    //判断上下楼
    if(p->InFloor>p->OutFloor){//下楼
        s = EnQueue(e.w_q[p->InFloor][GoingDown],*p);
        e.CallDown[p->InFloor]=true;
    }else{
        s = EnQueue(e.w_q[p->InFloor][GoingUp],*p);
        e.CallUp[p->InFloor]=true;
    }
    if(s==YES)//printf("用户%d进入了%d号电梯下的%d楼，他要去%d楼\n",p->id,e.id,p->InFloor,p->OutFloor);
    //下一个用户到来的事件添加
    AddActivity(CurrentTime+rand()%EnterTime,AddPeople,elevator[rand()%MAX_ELEVATOR]);
    e.count=0;
    return Add;
}

/**
 用户进出的事件
 */
Condition InAndOut(Elevator &e){
    People p;
    //说明电梯有人要出来
    if(e.CallCar[e.floor]){
        //所有人离开
        if(IsEmpty_S(e.i_s[e.floor])!=YES){
            Pop(e.i_s[e.floor],p);
            e.peopleNum--;
           // printf("客户%d离开%d号电梯中\n",p.id,e.id);
            AddActivity(CurrentTime+MOVE_TIME,InAndOut,e);
        }else{
            e.CallCar[e.floor]=false;
        }
    }else
    {
        //有人要走进电梯
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
        printf("请输入电梯的数目(最多5):");
        while(!scanf("%d",&MAX_ELEVATOR)||(MAX_ELEVATOR<1||MAX_ELEVATOR>6)){while(getchar()!='\n');printf("请正确输入\n");}
        printf("请输入楼层的数目(最多20):");
        while(!scanf("%d",&MAX_FLOOR)||MAX_FLOOR<2||MAX_FLOOR>21){while(getchar()!='\n');printf("请正确输入\n");}
        printf("请选择是否动态显示电梯的运行状态（1为是，0为否）:");
        while(!scanf("%d",&mode)||(mode!=1&&mode!=0)){while(getchar()!='\n');printf("请正确输入1或0\n");}
        printf("请输入运行的总时间:");
        while(!scanf("%d",&TotalTime)){while(getchar()!='\n');printf("请输入数字\n");}
        printf("请输入用户的最长容忍时间(默认的等待时间为220，以输入的值为界的随机值加上220为最终的等待时间):");
        while(!scanf("%d",&GiveupTime)){while(getchar()!='\n');printf("请输入数字\n");}
        printf("请输入用户进入系统的最长间隔时间(不小于200):");
        while(!scanf("%d",&EnterTime)||EnterTime<200){while(getchar()!='\n');printf("请正确输入\n");}
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
        printf("按任意键返回\n");
        system("pause");
        system("cls");
    }
}

void run(){
    if(mode==1){
        HideCarsor();
        PrintInfo();
    }
    //首先为时序添加一个用户
    AddActivity(CurrentTime+rand()%EnterTime,AddPeople,elevator[rand()%MAX_ELEVATOR]);
    while(++CurrentTime<=TotalTime){
        //用户进出电梯
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
    printf("一共来了%d人,%d人没等电梯就走了,服务了%d个人,平均等待时间为%f\n",UserId,LeavePeople,ServerdPeople,TotalWaitTime/(ServerdPeople*1.0));
}
