//
//  elevator.cpp
//  my_elevator
//
//  Copyright  2018 mdy. All rights reserved.
//

#include "elevator.hpp"

extern int MAX_FLOOR;

void InitElevator(Elevator &e){
    e.condition=Idle;
    e.state=GoingUp;
    e.floor=1;
    e.peopleNum=0;
    e.count=0;
    e.CheckDelay=false;
    e.IsClosing=false;
    e.w_q = (Queue**)malloc(sizeof(Queue*)*MAX_FLOOR);
    e.i_s = (Stack*)malloc(sizeof(Stack)*MAX_FLOOR);
    e.CallUp = (bool*)malloc(sizeof(bool)*MAX_FLOOR);
    e.CallCar = (bool*)malloc(sizeof(bool)*MAX_FLOOR);
    e.CallDown = (bool*)malloc(sizeof(bool)*MAX_FLOOR);
    for (int i=0;i<(MAX_FLOOR);++i){
        e.w_q[i] = (Queue*)malloc(sizeof(Queue)*2);
        InitQueue(e.w_q[i][GoingDown],MAX_QUEUE_SIZE);
        InitQueue(e.w_q[i][GoingUp],MAX_QUEUE_SIZE);
        InitStack(e.i_s[i],MAX_PEOPLE);
        e.CallUp[i]=e.CallDown[i]=e.CallCar[i]=false;
    }
}

Condition Open(Elevator &e){
    switch (e.condition) {
        case Closed:
            return GetNextStep(e);
        case Idle:
            //�Ƿ���
            if(IsOpen(e)){
                e.condition = Opening;
                e.count=OC_TIME;
                e.CheckDelay=false;
                return e.condition;
            }else{
                Condition c = GetNextStep(e);
                //�ص����ݲ�
                if(c==None&&e.CheckDelay&&e.floor!=1){
                    e.condition=Moving;
                    e.state=e.floor-1>0?GoingDown:GoingUp;
                    e.count=Accelerate+e.floor-1>0?DownTime:UpTime;
                    return e.condition;
                }else{
                    e.CheckDelay=false;
                    return c;
                }
            }
        //���ŵ�ʱ��������µ�����
        case Closing:
            //����ǵ�ǰ�����������ȡ������
            if(IsOpen(e)){
                e.IsClosing=false;
                e.condition=Opening;
                e.count=CANCLE_CLOSE_TIME;
                return e.condition;
            }else{
                return None;
            }
        default:
            return None;
    }
}

Condition Stop(Elevator &e){
    switch (e.condition) {
        case Opening://�������
            e.condition = Opened;e.count=TEST_TIME;
            break;
        case Closing://���ŵ�״̬��Ϊ�ѹ���
            if(e.IsClosing){
                e.IsClosing=false;
                e.condition=Closed;
                e.count=0;
            }else{
                return None;
            }
            break;
        case Decelerate://�������,˵���Ѿ�����Ŀ��¥��
            if(IsNeedUp(e)&&e.state==GoingDown&&!e.CallDown[e.floor])e.state=GoingUp;
            else if(IsNeedDown(e)&&e.state==GoingUp&&!e.CallUp[e.floor])e.state=GoingDown;
            if(e.CheckDelay&&e.floor==1){
                e.condition=Idle;
                e.count=0;
                e.CheckDelay=false;
            }else{//����
                e.condition=Opening;
                e.count=OC_TIME;
            }
            break;
        case Moving:
            //����Ƿ�Ҫֹͣ
            e.state==GoingUp?e.floor+=1:e.floor-=1;
            if(CheckStop(e)){
                e.condition=Decelerate;
                e.count=e.state==GoingUp?UpDecelerate:DownDecelerate;
            }else{
                //��ֹͣ������ƶ�
                e.count=e.state==GoingUp?UpTime:DownTime;
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
            //ȷ���Ƿ�����Ҫ������
            if(!e.CallCar[e.floor]&&((e.state==GoingDown&&!e.CallDown[e.floor])||(!e.CallUp[e.floor]&&e.state==GoingUp))){
                e.condition=Closing;
                e.IsClosing=true;
                e.count=OC_TIME;
            }else{
                //�������ù���ʱ��
                e.count=TEST_TIME;
            }
            return e.condition;
        default:
            return None;
    }
}

//�жϵ����Ƿ�Ҫֹͣ
bool CheckStop(Elevator e){
    if(e.floor>=MAX_FLOOR||e.floor<0)return true;
    //�Ƿ���һ������Ҫȥ
    if(e.CallCar[e.floor]) return true;
    //û��ȥ�����Ƿ���������һ�㰴�˵��ݰ�ť
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
    //ȷ���Ƿ�ص��˱��ݲ�
    if(e.CheckDelay&&e.floor==1)return true;
    return false;
}

//ȷ�ϵ��ݵ���һ���ж�
Condition GetNextStep(Elevator &e){
    bool up = IsNeedUp(e);
    bool down = IsNeedDown(e);
    //���������¥�������򷵻�
    if((e.state==GoingUp&&up)||(e.state==GoingDown&&!down&&up)){
        e.state=GoingUp;e.condition=Moving;e.count=Accelerate+UpTime;e.CheckDelay=false;
    }else if((e.state==GoingDown&&down)||(e.state==GoingUp&&!up&&down)){
        e.state=GoingDown;e.condition=Moving;e.count=Accelerate+DownTime;e.CheckDelay=false;
    }else{
        //û������¥�����������õ��ݵĳ�ʱʱ��,�������ó�ʱ��־λ
        if(e.condition!=Idle){e.condition=Idle;e.count=DELAY_TIME;e.CheckDelay=true;}
        //����Ǳ������Ѿ�����״̬�ˣ���ֱ�ӷ����������κζ���Ķ���
        else return None;
    }
    return e.condition;
}

//�ж��Ƿ�Ҫ����
bool IsOpen(Elevator &e){
    if(e.CallCar[e.floor]||(e.CallDown[e.floor]&&e.state==GoingDown)||(e.CallUp[e.floor]&&e.state==GoingUp)) return true;
    if(e.condition==Idle){
        if(e.CallDown[e.floor]){e.state = GoingDown;return true;}
        else if (e.CallUp[e.floor]){e.state = GoingUp;return true;}
    }
    return false;
}

//�ж��Ƿ����ƶ����߲������
bool IsNeedUp(Elevator e){
    for(int i=e.floor+1;i<MAX_FLOOR;i++)
        if(e.CallCar[i]||e.CallDown[i]||e.CallUp[i]) return true;
    return false;
}
//�ж��Ƿ����ƶ����Ͳ������
bool IsNeedDown(Elevator e){
    for(int i=0;i<=e.floor-1;i++)
        if(e.CallCar[i]||e.CallDown[i]||e.CallUp[i]) return true;
    return false;
}
