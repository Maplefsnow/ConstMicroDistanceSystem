#include <iostream>
#include <thread>
#include <math.h>
#include <module/Axis.h>
#include <module/Group.h>

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

    HAND groupHand; U16 gpState;
    Acm_GpAddAxis(&groupHand, axis2.getHand());
    Acm_GpGetState(groupHand, &gpState);
    cout << gpState << endl;

    axis2.setCmdPos(0);

    cout << "MOVING..." << endl;

    int A = 3000, omega = 1, f = 64;
    for(int i=0; i<f; i++) {
        axis2.absMove( A * sin(omega * 3.14/(f/2) * i) );
        while(axis2.getAxisStatus() != 1) ;
    }

    return 0;
}