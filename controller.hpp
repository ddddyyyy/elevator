//
//  controller.hpp
//  my_elevator
//
//  Created by 中山附一 on 2018/12/17.
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
    int endTime;
    Elevator *e;
    Condition(*fn)(Elevator&);
    struct activity *next;
}Activity;

void init();
void run();

Condition AddPeople(Elevator &e);
Condition InAndOut(Elevator &e);


void AddActivity(int time,Condition(*fn)(Elevator&),Elevator &e);

#endif /* controller_hpp */
