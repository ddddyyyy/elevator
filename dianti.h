#ifndef _DIANTI_H_
#define _DIANTI_H_
#include <stdio.h>
#include <stdlib.h>
 
#define GoingUp 1//匀速上升
#define GoingDown 2//匀速下降
#define SpeedUp 3//加速上升
#define SpeedDown 4//加速下降
#define SlowUp 5//减速上升准备停靠
#define SlowDown 6//减速下降准备停靠
#define Idle 7//空闲
#define Stop 8//停止且已关门
#define DoorOpen 9//停止且门已打开
#define DoorOpening 10
#define DoorCloseing 11
 
#define	CloseTest 40	//电梯关门测试时间
#define OverTime  300	//电梯停候超时时间
#define Accelerate 15	//加速时间
#define UpTime	51	//上升时间
#define DownTime 61	//下降时间
#define UpDecelerate 14	//上升减速
#define DownDecelerate 23	//下降减速
#define DoorTime	20	//开门关门时间
#define InOutTime	25	//进出电梯时间
 
#define MaxTime 10000
#define MaxFloor 5
#define BaseFloor 1
 
//初始每层电梯等待队列和栈 
#define Init() ({int i;\
	for(i=0;i<MaxFloor;i++){\
		Stack[i].next=NULL;\
		Queue[i].next=NULL;\
	}\
	activity.next=NULL;\
})
typedef struct Person{
	int Id;
	int OutFloor;
	int GiveupTime;
	struct Person* next;
}Person;
 
typedef struct Activity{
	int time;
	void(*fn)(void);
	struct Activity* next;
}Activity;
 
typedef struct Person_Ele{
	int Id;
	struct Person_Ele* next;
}Person_Ele;
 
int AddQueue(int floor,struct Person* p); 
void AddAct(int time,void(*fn)(void));
void TestPeople();
void DoTime();
void Input(void);
//以下函数与电梯决策有关 
void testinout(void);
void doclosedoor(void);
void doopendoor(void);
void doout(void);
void doin(void);
void doup(void);
void dodown(void);
void domove(void);
void doslow(void);
void tofirst();
int GetWhere(void);
#endif
