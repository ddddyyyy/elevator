//
//  controller.cpp
//  my_elevator
//
//  Created by 中山附一 on 2018/12/17.
//  Copyright © 2018 mdy. All rights reserved.
//

#include "controller.hpp"

Activity *activities;//时序的活动链n表
Elevator *elevator;//电梯

int CurrentTime=0;//当前的时间
int TotalTime=0;//运行的总时间
int GiveupTime=0;//用户放弃的最大时间
int _EnterTime=0;//最长的新用户进入系统时间
int EnterTime=0;//新用户到来的时间
int UserId = 0;//用户id
int ElevatorId = 0;//电梯id
int LeavePeople=0;//离开的用户
int ServerdPeople=0;//服务的用户
int TotalWaitTime=0;//用户等待的时间

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
        while (act->next!=NULL&&act->next->endTime<temp->endTime) {
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
    p->GiveupTime = rand()%GiveupTime+CurrentTime;
    p->inTime=CurrentTime;
    p->InFloor=rand()%MAX_FLOOR+1;
    do{
        p->OutFloor=rand()%MAX_FLOOR+1;
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
    if(s==OK)printf("用户%d进入了%d号电梯下的%d楼，他要去%d楼\n",p->id,e.id,p->InFloor,p->OutFloor);
    //下一个用户到来的事件添加
    AddActivity(CurrentTime+rand()%EnterTime,AddPeople,elevator[rand()%MAX_ELEVATOR]);
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
            printf("客户%d离开%d号电梯中\n",p.id,e.id);
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
            printf("客户%d进入%d号电梯中\n",p.id,e.id);
            AddActivity(CurrentTime+MOVE_TIME,InAndOut,e);
            ServerdPeople++;
            TotalWaitTime+=CurrentTime-p.inTime;
        }else{
            e.state==GoingDown?e.CallDown[e.floor]=false:e.CallUp[e.floor]=false;
            AddActivity(CurrentTime,close,e);
        }
    }
    return None;
}

void init(){
    //     printf("请输入运行的总时间:");
    //     scanf("%d",&TotalTime);
    //     printf("请输入用户的最长容忍时间:");
    //     scanf("%d",&GiveupTime);
    //     printf("请输入下一个用户的最长到来时间:");
    //     scanf("%d",&_EnterTime);
    TotalTime = 5000;
    GiveupTime = 130;
    EnterTime = 300;
    srand((unsigned)time(NULL));
    elevator = (Elevator*)malloc(sizeof(Elevator)*MAX_ELEVATOR);
    for(int i=0;i<MAX_ELEVATOR;++i){
        InitElevator(elevator[i]);
        elevator[i].id=ElevatorId++;
    }
    run();
}

void run(){
    AddActivity(CurrentTime+rand()%EnterTime,AddPeople,elevator[rand()%MAX_ELEVATOR]);
    while(++CurrentTime<=TotalTime){
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
                    AddActivity(CurrentTime+activities->e->count,close,*activities->e);
                    break;
                case Decelerate:
                case Closing:
                case Opening:
                case Moveing:
                    AddActivity(CurrentTime+activities->e->count,stop,*activities->e);
                    break;
                case Add:
                case Idle:
                case Closed://if(elevator[0].condition!=Idle)printf("关门啦测试\n");
                    AddActivity(CurrentTime+activities->e->count,open,*activities->e);
                    break;
                default:
                    break;
            };
            activities=activities->next;
        }
    }
     printf("一共来了%d人,%d人没等电梯就走了,服务了%d个人,平均等待时间为%f\n",UserId-1,LeavePeople,ServerdPeople,TotalWaitTime/(ServerdPeople*1.0));
}
