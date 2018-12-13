#include "dianti.h"
 
int Time=0;
int CallUp[MaxFloor]={0,};
int CallDown[MaxFloor]={0,};
int CallCar[MaxFloor]={0,};
int Floor=BaseFloor;
int State=Idle;
int PersonId=0;
Activity activity={0,NULL,NULL};
Person_Ele Stack[5]={0,};
Person Queue[5]={0,};
 
int main(){
	Init();
	Input();
	DoTime();
	return 0;
}
 
int AddQueue(int floor,Person* p){//加入相应层的客户等待队列 
	Person* tmp=&Queue[floor];//这始终加在链表的最后一位， 
	while(tmp->next!=NULL){
		tmp=tmp->next;
	}
	tmp->next=p;
	return 0;
}
 
void AddAct(int time,void(*fn)(void)){//将一个活动加入定时器，时间到了会调用这个函数 
	time=Time+time;					//这个函数参数必须是void，返回值也必须是void
	struct Activity* act;
	act=(struct Activity*)malloc(sizeof(struct Activity));
	act->next=NULL;
	act->fn=fn;
	act->time=time;
	struct Activity* p=&activity;
	while(p->next!=NULL){
		if(p->next->time>time)
			break;
		p=p->next;
	}
	act->next=p->next;
	p->next=act;
}
 
void TestPeople(){//这是检测每层队列是否有人放弃，有人放弃就将他踢出队列 
	int i;//这个函数每个时间都会被调用，效率相对较低 
	for(i=0;i<MaxFloor;i++){
		Person* p=Queue[i].next;
		Person* q=&Queue[i];
		if(p==NULL)
			continue;
		while(p!=NULL){
			if(p->GiveupTime<=Time){
				if(Floor==i&&(State>=Idle))
					break;
				q->next=p->next;
				printf("用户%d放弃了等待!\n",p->Id);
				free(p);
				p=q->next;
				continue;
			}
			q=p;
			p=p->next;
		}
	}
}
 
 
 
void Input(void){//输入人员信息，这个需要手动调用一次，之后就根据定时器调用了 
	Person* p = (Person*)malloc(sizeof(Person));
	int infloor,outfloor,giveuptime,intertime;
	while(1){
		printf("请输入用户的起始楼层:");
		scanf("%d",&infloor);
		printf("请输入用户的目标的楼层:");
		scanf("%d",&outfloor);
		printf("请输入用户的最长容忍时间:");
		scanf("%d",&giveuptime);
		printf("请输入下一个用户的到来时间:");
		scanf("%d",&intertime);
		if(!(infloor<0||infloor>MaxFloor-1||outfloor<0||outfloor>MaxFloor-1)&&(infloor!=outfloor))
			break;
		printf("错误的用户信息录入!\n");
	}
	p->Id=PersonId++;
	p->GiveupTime=giveuptime+Time;
	p->next=NULL;
	p->OutFloor=outfloor;
	if(outfloor>infloor)
		CallUp[infloor]=1;
	else
		CallDown[infloor]=1;
	AddQueue(infloor,p);
	AddAct(intertime,Input);
}
 
void testinout(void){//检测有无人进出 
	if(Queue[Floor].next||Stack[Floor].next)
		AddAct(CloseTest,testinout);
	else{
		State=DoorCloseing;
		CallUp[Floor]=0;
		CallDown[Floor]=0;
		CallCar[Floor]=0;
		AddAct(DoorTime,doclosedoor);
	}
}
 
void doclosedoor(void){//电梯门关了 
	printf("电梯门关了\n");
	State=Stop;
}
 
void doopendoor(void){//打开电梯门 
	printf("电梯门开了\n");
	State=DoorOpen;//门打开了 
	AddAct(CloseTest,testinout);
	if(Stack[Floor].next)
		AddAct(InOutTime,doout);
	else{//没人出，就看有没有进的 
		if(Queue[Floor].next)
			AddAct(InOutTime,doin);
	}
}
 
void doout(void){
	//根据栈出人，如果没有看是否有人进 
	if(Stack[Floor].next){
		Person_Ele* p=Stack[Floor].next;
		Stack[Floor].next=p->next;
		;//显示信息 
		printf("用户%d走出电梯\n",p->Id);
		free(p);
	}
	if(Stack[Floor].next){
		AddAct(InOutTime,doout);
	}else{
		if(Queue[Floor].next)
			AddAct(InOutTime,doin);
	}
}
 
void doin(void){//人进入电梯，这里不用关电梯门它会定时关的 
	Person* p=Queue[Floor].next;
	if(p){
		Queue[Floor].next=p->next;
		Person_Ele* pe=(Person_Ele*)malloc(sizeof(Person_Ele));
		int in=p->OutFloor;
		CallCar[in]=1;//置位请求 
		pe->next=Stack[in].next;
		pe->Id=p->Id;
		Stack[in].next=pe;
		printf("用户%d走入电梯\n",p->Id);
		free(p);
	}
	if(Queue[Floor].next){
		AddAct(InOutTime,doin);
	}
}
 
int GetWhere(void){
	static int old=0;//保存上一次电梯的方向，保证电梯尽可能在一个方向走 
	int isup=0,isdown=0;
	int i;
	for(i=Floor+1;i<MaxFloor;i++){
		if(CallDown[i]||CallUp[i]||CallCar[i])
			isup=1;
	}
	for(i=Floor-1;i>=0;i--){
		if(CallDown[i]||CallUp[i]||CallCar[i])
			isdown=1;
	}
	if(isup==0&&isdown==0){
		return 0;
	}
	if(old==0){
		if(isdown) old=GoingDown;
		if(isup) old=GoingUp;
		return old;
	}
	if(old==GoingUp&&isup)
		return old;
	else if(old==GoingDown&&isdown)
		return old;
	else if(isdown) 
		old=GoingDown;
	else if(isup) 
		old=GoingUp;
	else
		printf("在选择方向时发生错误!\n");
	return old;
}
 
void tofirst(void){//去第一层
	if(State!=Idle||Floor==BaseFloor)
		return;
	printf("长时间没人请求电梯!将进入%d层\n",BaseFloor);
	CallCar[BaseFloor]=2;//给电梯一个虚拟的去1层的请求，这并不会开门 
}
 
void doslow(void){//电梯停了 
	printf("电梯停了，当前层是%d\n",Floor);
	State=Stop;
}
 
void doup(void){ 
	Floor++;
	printf("电梯正在上升!现已到了%d层!\n",Floor);
	if(CallDown[Floor]||CallUp[Floor]||CallCar[Floor]){
		State=SlowUp;
		AddAct(UpDecelerate,doslow);
	}else{
		if(Floor==MaxFloor-1){
			State=SlowUp;
			AddAct(UpDecelerate,doslow);
		}else{
			AddAct(UpTime,doup);
		}
	}
}
 
void dodown(void){
	Floor--;
	printf("电梯正在下降!现已到了%d层!\n",Floor);
	if(CallUp[Floor]||CallDown[Floor]||CallCar[Floor]){
		State=SlowDown;
		AddAct(DownDecelerate,doslow);
	}else{
		if(Floor==0){
			State=SlowDown;
			AddAct(DownDecelerate,doslow);
		}else{
			AddAct(DownTime,dodown);
		}
	}
}
 
void domove(void){//加速完成，将进入正常速度 
	if(State==SpeedUp){
		printf("电梯已加速上升!\n");
		State=GoingUp;
		AddAct(UpTime,doup);
	}else{
		printf("电梯已加速下降!\n");
		State=GoingDown;
		AddAct(DownTime,dodown);
	}
}
 
void Controler(void){
	if(State==Idle||State==Stop){ 
		if(CallUp[Floor]||CallDown[Floor]||CallCar[Floor]){
			//当前层有请求，需要开门进出
			if(CallCar[BaseFloor]==2){
				CallCar[BaseFloor]=0;
				State=Idle;
				printf("现在在%d层,无人请求电梯!\n",BaseFloor);
				return;
			}
			State=DoorOpening;
			AddAct(DoorTime,doopendoor);
		}
		else{
			//当前层无请求，判断其他层请求
			int whitch=GetWhere();
			if(whitch==GoingUp){
				State=SpeedUp;
				AddAct(Accelerate,domove);
			}else if(whitch==GoingDown){
				State=SpeedDown;
				AddAct(Accelerate,domove);
			}else{
				State=Idle;
				if(Floor!=BaseFloor)
					AddAct(OverTime,tofirst);
			}
		}
	}
	//否则电梯忙碌 
	return;
}
 
void DoTime(){
	//此函数用于模拟时钟 
	while(1){
		if(Time>MaxTime)
			return;
		TestPeople();//两个始终都会被调用的函数 
		Controler(); 
		struct Activity* p=activity.next;
		if(p==NULL){
			Time=MaxTime;
		}
		if(p&&Time>=p->time){//取出活动队头的，检测定时是否到了 
			activity.next=p->next;
			p->fn();
			free(p);
		}
		Time++;
	}
}
