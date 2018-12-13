#include <cstdio>
#include <people.h>

int main(int argc, char const *argv[])
{
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

    return 0;
}


