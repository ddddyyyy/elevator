//
//  controller.hpp
//  my_elevator
//
//  Copyright © 2018 mdy. All rights reserved.
//

#ifndef controller_hpp
#define controller_hpp

#include "elevator.hpp"

#define MAX_ELEVATOR 5//电梯的数量

/**
 时序结构体
 */
typedef struct activity{
    int endTime;//动作结束的时间
    Elevator *e;//动作对应的电梯
    Condition(*fn)(Elevator&);//执行的动作
    struct activity *next;//下一个动作
}Activity;

void init();
void run();

Condition AddPeople(Elevator &e);
Condition InAndOut(Elevator &e);


void AddActivity(int time,Condition(*fn)(Elevator&),Elevator &e);

#endif /* controller_hpp */
