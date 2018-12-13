#include "people.h"
#include "elevator.h"

//时序系统中的动作
typedef struct Action{
    //动作结束的时间
    int endTime;
    //执行的动作
	void(*f)(void);
    //下一个动作
	struct Action* next;
}Action;

int currentTime=0;
int GiveupTime;
Elevator elevator;

//初始化
void Init(){

}
//添加人
Status AddPeople(Elevator E){
    People *p = (People*)malloc(sizeof(People));
    if(NULL==p)return OVERFLOW;
    p->GiveupTime = GiveupTime;
    p->inTime=currentTime;
    p->InFloor=rand()%MAX_FLOOR;
    do{
        p->OutFloor=rand()%MAX_FLOOR;
    }while(p->InFloor==p->OutFloor);
    //将新加入的人入队
    EnQueue(E.w_q[p->InFloor],*p);
    return OK;
}
//检测状态
void CheckState(){

}

//时序中的动作

//人进出
void InAndOut(){

}
//人等待
void Wait(){

}

//电梯上升
void Up(){

}
//电梯下降
void Down(){

}
//电梯开关门
void OpenAndClose(){
    
}
