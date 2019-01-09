//
//  elevator.hpp
//  my_elevator
//
//  Copyright  2018 mdy. All rights reserved.
//

#ifndef elevator_hpp
#define elevator_hpp

#include "people.hpp"

#define MAX_PEOPLE 10 //�����ڵ������

#define OC_TIME 20*T //�����ŵ�ʱ��
#define CANCLE_CLOSE_TIME 10*T
#define DELAY_TIME 300*T //��ĳ�㾲ֹ�ĳ�ʱʱ��
#define TEST_TIME 40*T //���˽���ʱ������ÿ��40t����һ�Σ������˽����������
#define Accelerate 15*T    //����ʱ��
#define UpTime    51*T    //����ʱ��
#define DownTime 61*T    //�½�ʱ��
#define UpDecelerate 14*T      //��������
#define DownDecelerate 23*T    //�½�����


//���ݵķ���
enum State
{
    GoingUp,//����
    GoingDown,//�½�
};

/*
 *����״̬
 */
enum Condition
{
    Idle,//�Ⱥ�
    Opening,//������
    Opened,//�Ѿ�����
    Closing,//���ڹ���
    Closed,//����
    Moving,//�ƶ�
    Decelerate,//����
    None,//��
    Add//�û�����ϵͳ
};

typedef struct{
    State state;//���ݵ�ǰ�ķ���
    Condition condition;//���ݵ�ǰ���еĻ
    bool *CallUp;//���������ϵİ�ť
    bool *CallDown;//���������µİ�ť
    bool *CallCar;//�����ڵİ�ť
    int floor;//��ǰ��¥��
    Queue **w_q;//ÿһ����ŶӶ���,��һ��Ϊ��ȥ���ˣ��ڶ���Ϊ��ȥ����
    Stack *i_s;//�����ڵ��ˣ���Ŀ�������ջ
    
    int id;
    int count;//���еļ�����
    int peopleNum;//�û�������
    bool CheckDelay;//��־�Ƿ��ڼ�ⳬʱ
    bool IsClosing;//��־�Ƿ����ڹ���
}Elevator;

void InitElevator(Elevator &e);

// ���ݵĶ�������
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
