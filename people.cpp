#include "people.h"

//队列操作---start---
//初始化
Status InitQueue(Queue &Q){
    Q.elem = (People*)malloc(sizeof(People)*MAX_QUEUE_SIZE);
    if (NULL == Q.elem){return OVERFLOW;}
    Q.front = Q.rear = 0;
    Q.maxSize = MAX_QUEUE_SIZE;
    return OK;
}
//入队
Status EnQueue(Queue &Q,People e){
    if ((Q.rear+1)%Q.maxSize==Q.front){return OVERFLOW;}
    Q.elem[Q.rear] = e;
    Q.rear = (Q.rear+1)%Q.maxSize;
    return OK;
}
//出队
Status DeQueue(Queue &Q,People &e){
    if(Q.front == Q.rear){return NO;}
    e = Q.elem[Q.front];    
    Q.front = (Q.front+1)%Q.maxSize;
    return OK;
}
//队列操作---end---
