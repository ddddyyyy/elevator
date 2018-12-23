//
//  people.cpp
//  my_elevator
//
//  Copyright © 2018 mdy. All rights reserved.
//

#include "people.hpp"

//优先队列 ---start---
//堆的相关操作--start
void swap(People *elem,int i,int m){
    elem[0] = elem[i];
    elem[i] = elem[m] ;
    elem[m] = elem[0] ;
}
void ShitfDown(People *elem,int pos,int size){
    int c,rc;
    while (pos<=size/2) {
        c=pos*2;
        rc=pos*2+1;
        if(rc<=size&&elem[rc].GiveupTime<elem[c].GiveupTime) c=rc;
        if(elem[pos].GiveupTime<elem[c].GiveupTime) return;
        swap(elem,pos,c);
        pos=c;
    }
}
//堆的相关操作--end
//初始化
Status InitQueue(Queue &Q,int size){
    Q.elem = (People*)malloc(sizeof(People)*(size+1));
    if (NULL == Q.elem){return OVERFLOW;}
    Q.rear=1;
    Q.maxSize=size+1;
    return YES;
}
//入队
Status EnQueue(Queue &Q,People e){
    if(Q.rear==Q.maxSize)return OVERFLOW;
    Q.elem[Q.rear] = e;
    int curr=Q.rear++;
    while (1!=curr&&Q.elem[curr].GiveupTime<Q.elem[curr/2].GiveupTime) {
        swap(Q.elem, curr, curr/2);
        curr/=2;
    }
    return YES;
}
//出队
Status DeQueue(Queue &Q,People &e){
    if(1==Q.rear){return NO;}
    e = Q.elem[1];
    Q.rear--;
    Q.elem[1]=Q.elem[Q.rear];
    if(Q.rear>1)ShitfDown(Q.elem,1,Q.rear-1);
    return YES;
}
Status IsEmpty_Q(Queue Q){
    return 1==Q.rear?YES:NO;
}
Status IsFull_Q(Queue Q){
    return Q.rear==Q.maxSize?YES:NO;
}
Status GetTop_Q(Queue Q,People &e){
    if(Q.rear==1)return NO;
    e = Q.elem[1];
    return YES;
}
void PrintQueue(Queue Q){
    printf("(");
    for(int i=1;i!=Q.rear;++i){
        std::cout<<Q.elem[i].id<<",";
    }
    printf(")");
}
int DeletePeople(Queue &Q,int time){
    People p;
    int num = 0;
    while (GetTop_Q(Q, p)!=NO&&p.GiveupTime<=time) {
        DeQueue(Q,p);
        //printf("客户%d离开了\n",p.id);
        ++num;
    }
    return num;
}
//队列操作---end---

//栈操作---start---
//初始化
Status InitStack(Stack &S,int size)
{
    S.elem = (People*) malloc (sizeof(People)*size);
    if (NULL == S.elem ){return NO;}
    S.size = size;
    S.top = 0;
    return YES;
}
//出栈
Status Pop(Stack &S,People &e)
{
    if(S.top==0){return OVERFLOW;}
    e = S.elem[--S.top];
    return YES;
}
//push
Status Push(Stack &S,People &e)
{
    if(S.top==S.size)return OVERFLOW;
    S.elem[S.top++] = e;
    return YES;
}
//栈S是否为空
Status IsEmpty_S(Stack S)
{
    if(S.top==0)return YES;
    else return NO;
}
void PrintStack(Stack S){
    printf("(");
    for(int i=0;i<S.top;++i){
        std::cout<<S.elem[i].id<<" ";
    }
    printf(")");
}
//栈操作---end---
