//
//  elevator.cpp
//  my_elevator
//
//  Copyright  2018 mdy. All rights reserved.
//

#include "elevator.hpp"

void InitElevator(Elevator &e){
    e.condition=Idle;
    e.state=GoingUp;
    e.floor=1;
    e.InOutTime=0;
    e.peopleNum=0;
    e.count=0;
    e.CheckDelay=false;
    for (int i=0;i<(MAX_FLOOR+1);++i){
        InitQueue(e.w_q[i][GoingDown],MAX_QUEUE_SIZE);
        InitQueue(e.w_q[i][GoingUp],MAX_QUEUE_SIZE);
        InitStack(e.i_s[i],MAX_PEOPLE);
        e.CallUp[i]=e.CallDown[i]=e.CallCar[i]=false;
    }
}

Condition Open(Elevator &e){
    switch (e.condition) {
        case Closed:
//            printf("========电梯%d在%d楼关门了========\n",e.id,e.floor);
            return GetNextStep(e);
        case Idle:
            if(IsOpen(e)){
                e.condition = Opening;
                e.count=OC_TIME;
                e.CheckDelay=false;
                return e.condition;
            }else{
                Condition c = GetNextStep(e);
                if(c==Idle&&e.CheckDelay&&e.floor!=1){
                    e.condition=Moving;
                    e.state=e.floor-1>0?GoingDown:GoingUp;
                    e.count=Accelerate+e.floor-1>0?DownTime:UpTime;
                    return e.condition;
                }else{
                    return c;
                }
            }
        case Closing:
            e.IsClosing=false;
            e.condition=Opening;
            e.count=CANCLE_CLOSE_TIME;
            return e.condition;
        default:
            return None;
    }
}

Condition Stop(Elevator &e){
    switch (e.condition) {
        case Opening://开门完成
//            printf("========电梯%d在%d楼开门了========\n",e.id,e.floor);
            e.condition = Opened;e.count=TEST_TIME;
            break;
        case Closing://将门的状态设为已关门
            if(e.IsClosing){
                e.condition=Closed;
                e.count=0;
            }else{
                return None;
            }
            break;
        case Decelerate://减速完成,说明已经到达目标楼层
            if(IsNeedUp(e)&&e.state==GoingDown&&!e.CallDown[e.floor])e.state=GoingUp;
            else if(IsNeedDown(e)&&e.state==GoingUp&&!e.CallUp[e.floor])e.state=GoingDown;
            if(e.CheckDelay&&e.floor==1){
                e.condition=Idle;
                e.count=0;
                e.CheckDelay=false;
            }else{//开门
                e.condition=Opening;
//                printf("========电梯%d开门中========\n",e.id);
                e.count=OC_TIME;
            }
            break;
        case Moving:
            e.state==GoingUp?e.floor+=1:e.floor-=1;
            //检查是否要停止
            if(CheckStop(e)){
                e.condition=Decelerate;
                e.count=e.state==GoingUp?UpDecelerate:DownDecelerate;
                e.CheckDelay=false;
            }else{
                //不停止则继续移动
                e.state==GoingUp?e.count=UpTime:e.count==DownTime;
            }
            break;
        default:
            break;
    }
    return e.condition;
}

Condition Close(Elevator &e){
    switch (e.condition) {
        case Opened:
            if(!e.CallCar[e.floor]&&((e.state==GoingDown&&!e.CallDown[e.floor])||(!e.CallUp[e.floor]&&e.state==GoingUp))){
//                printf("========电梯%d关门中========\n",e.id);
                e.condition=Closing;
                e.IsClosing=true;
                e.count=OC_TIME;
            }
            return e.condition;
        default:
            return None;
    }

}

//判断电梯是否要停止
bool CheckStop(Elevator e){
    //是否这一层有人要去
    if(e.CallCar[e.floor]) return true;
    //没人去则检查是否有人在这一层按了电梯按钮
    if((e.state==GoingUp&&e.CallUp[e.floor])||(e.state==GoingDown&&e.CallDown[e.floor]))
        return true;
    if(e.state==GoingDown&&e.CallUp[e.floor]&&!IsNeedDown(e)){
        e.state=GoingUp;
        return true;
    }
    if(e.state==GoingUp&&e.CallDown[e.floor]&&!IsNeedUp(e)){
        e.state=GoingDown;
        return true;
    }
    if(e.CheckDelay&&e.floor==1)return true;
    return false;
}

//确认电梯的下一步行动
Condition GetNextStep(Elevator &e){
    bool up = IsNeedUp(e);
    bool down = IsNeedDown(e);
    //如果有上下楼的请求则返回
    if((e.state==GoingUp&&up)||(e.state==GoingDown&&!down&&up)){
        e.state=GoingUp;e.condition=Moving;e.count=Accelerate+UpTime;e.CheckDelay=false;
    }else if((e.state==GoingDown&&down)||(e.state==GoingUp&&!up&&down)){
        e.state=GoingDown;e.condition=Moving;e.count=Accelerate+DownTime;e.CheckDelay=false;
    }else{
        //没有上下楼的请求，则设置电梯的超时时间,并且设置超时标志位
        if(e.condition!=Idle){e.condition=Idle;e.count=DELAY_TIME;e.CheckDelay=true;}
    }
    return e.condition;
}

//判断是否要开门
bool IsOpen(Elevator &e){
    if(e.CallCar[e.floor]||(e.CallDown[e.floor]&&e.state==GoingDown)||(e.CallUp[e.floor]&&e.state==GoingUp)) return true;
    if(e.condition==Idle){
        if(e.CallDown[e.floor]){e.state = GoingDown;return true;}
        else if (e.CallUp[e.floor]){e.state = GoingUp;return true;}
    }
    return false;
}

//判断是否有移动到高层的请求
bool IsNeedUp(Elevator e){
    for(int i=e.floor+1;i<=MAX_FLOOR;i++)
        if(e.CallCar[i]||e.CallDown[i]||e.CallUp[i]) return true;
    return false;
}
//判断是否有移动到低层的请求
bool IsNeedDown(Elevator e){
    for(int i=MIN_FLOOR;i<=e.floor-1;i++)
        if(e.CallCar[i]||e.CallDown[i]||e.CallUp[i]) return true;
    return false;
}
