//
//  people.hpp
//  my_elevator
//
//  Created by 中山附一 on 2018/12/17.
//  Copyright © 2018 mdy. All rights reserved.
//

#ifndef people_hpp
#define people_hpp


#include <cstdlib>
#include <iostream>
#include <string>
#include <cstdio>
#include <time.h>
#include <unistd.h>
#include <windows.h>

enum Status
{
    YES,NO,OVERFLOW
};
#define T  1  //单位时间
#define MOVE_TIME 25*T //人的进出时间
#define MAX_QUEUE_SIZE 10 //队列的最长长度

typedef struct{
    int id;//人的标志
    int InFloor;//该人进入哪层楼
    int OutFloor;//他要去哪层楼
    int GiveupTime;//他能容忍的等候时间
    int inTime; //进入系统的时间
}People;

//排队队列（优先队列）
typedef struct {
    People *elem;
    int rear;
    int maxSize;
}Queue;
//电梯乘客栈
typedef struct
{
    People *elem;
    int top;
    int size;
}Stack;

//从队列中删除人
int DeletePeople(Queue &Q,int time);

//优先队列（最小堆实现）操作---start---
//初始化
Status InitQueue(Queue &Q,int size);
//入队
Status EnQueue(Queue &Q,People e);
//出队
Status DeQueue(Queue &Q,People &e);
//是否为空
Status IsEmpty_Q(Queue Q);
//是否满
Status IsFull_Q(Queue Q);
//得到队列第一个元素
Status GetTop_Q(Queue Q,People &e);
//打印队列
void PrintQueue(Queue Q);
//队列操作---end---

//初始化
Status InitStack(Stack &S,int size);
//出栈
Status Pop(Stack &S,People &e);
//push
Status Push(Stack &S,People &e);
//栈S是否为空
Status IsEmpty_S(Stack S);
//栈S是否满
Status IsFull_S(Stack S);
//打印栈的内容
void PrintStack(Stack S);

#endif /* people_hpp */
