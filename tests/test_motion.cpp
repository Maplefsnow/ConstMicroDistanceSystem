#include <iostream>
#include <cstdio>
#include <thread>
#include <math.h>
#include <module/Axis.h>

using namespace std;


int main() {
    DEVLIST devList[10];
    HAND devHand;
    U32 devNum;

    Acm_GetAvailableDevs(devList, 10, &devNum);
    DWORD dwDevNum = devList[0].dwDeviceNum;
    Acm_DevOpen(dwDevNum, &devHand);

    Axis axis2 = Axis(devHand, 2);
    Axis axis4 = Axis(devHand, 4);
    axis4.setPulseOutMode(1);

    axis2.setCmdPos(0);
    while(!axis2.getAxisStatus()) ;

    int num = 0;
    while(scanf("%d", &num)){
        axis2.absMove(num);
    }

    return 0;
}