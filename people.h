#ifndef PEOPLE_H
#define PEOPLE_H

#include <cstdlib>

enum Status
{
    OK,ERROR,YES,NO,OVERFLOW
};

#define T  1  //单位时间

#define MOVE_TIME 25*T //人的进出时间
#define INTER_TIME 20*T;//下一人出现的时间间隔，据此系统预置下一人进入系统的时刻。

/*
M1.[进入系统，为下一人的出现作准备]
M2.[按电钮并等候] 此时应对以下不同情况作不同的处理:
    1)Floor=InFloor 且电梯的下一个活动是 E6(电梯在本层，但正在关门); 
    2)Floor=InFloor 且 D3≠0(电梯在本层，正有人进出);
    3)其它情况，可能 D2=0 或电梯处于活动 E1(在 1 层停候)。
M3.[进入排队] 在等候队列 Queue[InFloor]末尾插入该人，并预置在 GiveupTime 个 t 之后他
若仍在队列中将实施动作 M4。
M4.[放弃] 如果 Floor≠InFloor 或 D1=0，则从 Queue[InFloor]和系统删除该人。如果
Floor=InFloor 且 D1≠0，他就继续等候(他知道马上就可进入电梯)。
M5.[进入电梯] 从 Queue[InFloor]删除该人，并把他插入到 Elevator(电梯)栈中。置
CallCar[OutFloor]为 1。
M6.[离去] 从 Elevator 和系统删除该人。
*/
enum P_Action
{
    M1,M2,M3,M4,M5,M6
};

typedef struct{
    int id;//人的标志
    int InFloor;//该人进入哪层楼
    int OutFloor;//他要去哪层楼
    int GiveupTime;//他能容忍的等候时间
    int inTime; //进入系统的时间
    
    P_Action action;//他的动作
}People;

//排队队列
#define MAX_QUEUE_SIZE 10
typedef struct {
    People *elem;
    int front;
    int rear;
    int maxSize;
}Queue;
//队列操作---start---
//初始化
Status InitQueue(Queue &Q,int size);
//入队
Status EnQueue(Queue &Q,People e);
//出队
Status DeQueue(Queue &Q,People &e);
//队列操作---end---


#endif