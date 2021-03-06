//
//  elevator.hpp
//  my_elevator
//
//  Copyright  2018 mdy. All rights reserved.
//

#ifndef elevator_hpp
#define elevator_hpp

#include "people.hpp"

#define MAX_PEOPLE 10 //电梯内的最多人

#define OC_TIME 20*T //开关门的时间
#define CANCLE_CLOSE_TIME 10*T
#define DELAY_TIME 300*T //在某层静止的超时时间
#define TEST_TIME 40*T //有人进出时，电梯每隔40t测试一次，若无人进出，则关门
#define Accelerate 15*T    //加速时间
#define UpTime    51*T    //上升时间
#define DownTime 61*T    //下降时间
#define UpDecelerate 14*T      //上升减速
#define DownDecelerate 23*T    //下降减速


//电梯的方向
enum State
{
    GoingUp,//上升
    GoingDown,//下降
};

/*
 *电梯状态
 */
enum Condition
{
    Idle,//等候
    Opening,//开门中
    Opened,//已经开门
    Closing,//正在关门
    Closed,//关门
    Moving,//移动
    Decelerate,//减速
    None,//无
    Add//用户进入系统
};

typedef struct{
    State state;//电梯当前的方向
    Condition condition;//电梯当前进行的活动
    bool *CallUp;//电梯外向上的按钮
    bool *CallDown;//电梯外向下的按钮
    bool *CallCar;//电梯内的按钮
    int floor;//当前的楼层
    Queue **w_q;//每一层的排队队列,第一组为上去的人，第二组为下去的人
    Stack *i_s;//电梯内的人，按目标层数入栈
    
    int id;
    int count;//运行的计数器
    int peopleNum;//用户的数量
    bool CheckDelay;//标志是否在检测超时
    bool IsClosing;//标志是否正在关门
}Elevator;

void InitElevator(Elevator &e);

// 电梯的动作抽象
//---start-----//
Condition Open(Elevator &e);
Condition Close(Elevator &e);
Condition Stop(Elevator &e);
//---end-----//
bool CheckStop(Elevator  e);
Condition GetNextStep(Elevator &e);
bool IsNeedUp(Elevator e);
bool IsNeedDown(Elevator e);
bool IsOpen(Elevator &e);


#endif /* elevator_hpp */
