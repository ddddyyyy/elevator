#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <people.h>

/*
E1.[在 1 层停候] 若有人按下一个按钮，则调用 Controler 将电梯转入活动 E3 或 E6。 
E2.[要改变状态?] 如果电梯处于 GoingUp(或 GoingDown)状态，但该方向的楼层却无人
等待，则要看反方向楼层是否有人等候，而决定置 State 为 GoingDown(或 GoingUp)还是 Idle。 
E3.[开门] 置D1和D2为非0值，预置300个t后启动活动E9和76个t后启动E5，然后预
置20个t后转到E4。
E4.[让人出入] 如果 Elevator 不空且有人的 OutFloor=Floor，则按进入的倒序每隔 25 个 t 让这
类人立即转到他们的动作 M6。Elevator 中不再有要离开的人时，如果 Queue[Floor]不空，则以 25 个 t 的速度让他们依次转到 M5。Queue[Floor]空时，置 D1 为 0，D3≠0，而且等候某个其它活动 的到来。
E5.[关门] 每隔 40 个 t 检查 D1，直到是 D1=0(若 D1≠0，则仍有人出入)。置 D3 为 0 并预 置电梯再 20 个 t 后启动活动 E6(再关门期间，若有人到来，则如 M2 所述，门再次打开)。
E6.[准备移动] 置 CallCar[Floor]为 0，而且若 State≠GoingDown 则置 CallUp[Floor]为 0，若 State≠GoingUp 则置 CallDown[Floor]为 0。调用 Controler 函数。
如果 State=Idle，则即使已经执行了 Controler，亦转到 E1。否则如果 D2≠0，则取消电梯活 动 E9。最后，如果 State=GoingUp，则预置 15 个 t 后(电梯加速)转到 E7;如果 State=GoingDown，
则预置 15 个 t 后(电梯加速)转到 E8。
E7.[上升一层] 置 Floor 加 1 并等候 51 个 t。如果现在 CallCar[Floor]=1 或 CallUp[Floor]=1，
或者如果((Floor=1 或 CallDown[Floor]=1)且 CallUp[j]=CallDown[j]= CallCar[j]=0 对于所有 j>Floor)，则预置 14 个 t 后(减速)转到 E2。否则重复 E7。
E8.[下降一层] 除了方向相反之外，与 E7 类似，但那里的 51 和 14 个 t 此时分别改为 61 和 23 个 t(电梯下降比上升慢)。
E9.[置不活动指示器] 置 D2 为 0 并调用 Controler 函数(E9 是由 E3 预置的，但几乎总是被 E6 取消了)。
*/
enum E_Action
{
    E1,E2,E3,E4,E5,E6,E7,E8,E9
};
/*
C1.[需要判断?] 若 State≠Idle，则返回。
C2.[应该开门?] 如果电梯处于 E1 且 CallUp[1],CallDown[1]或 CallCar[1]非 0，则预置 20 个 t 后启动 E3，并返回。
C3.[有按钮按下?] 找最小的 j≠Floor，使得 CallUp[j],CallDown[j]或 CallCar[j]非 0，并转到 C4。但如果不存在这样的 j，那么，如果 Controler 正为 E6 所调用，则置 j 为 1，否则返回。
C4.[置 State] 如果 Floor>j，则置 State 为 GoingDown;如果 Floor<j，则置 State 为 GoingUp。 C5.[电梯静止?] 如果电梯处于 E1 而且 j≠1，则预置 20 个 t 后启动 E6。返回。
*/
enum Condition
{
    C1,C2,C3,C4
};

enum State
{
    Idle,GoingDown,GoingUp
};


#define MAX_FLOOR 5 //电梯层数
#define OC_TIME 20*T //开关门的时间
#define DELAY_TIME 300*T //在某层静止的超时时间
#define TEST_TIME 40*T //有人进出时，电梯每隔40t测试一次，若无人进出，则关门


typedef struct{
    State state;
    bool CallUp[MAX_FLOOR];//向上的按钮
    bool CallDown[MAX_FLOOR];//向下的按钮
    bool CallCar[MAX_FLOOR];//那一层的按钮被按下了
    int floor;//当前的楼层
    /*
     *D1 -- 值为 0，除非人们正在进入和离开电梯;
     *D2 -- 值为 0，如果电梯已经在某层停候 300t 以上;
     *D3 -- 值为 0，除非电梯门正开着又无人进出电梯;
    */
    bool D[2];
    Queue w_q[MAX_FLOOR];//每一层的排队队列
    Queue i_q[MAX_QUEUE_SIZE];
}Elevator;

#endif