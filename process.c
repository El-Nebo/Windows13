#include "headers.h"

int *remainingtime;
int SCH_Proc_Shm;
int main(int agrc, char *argv[])
{
    initClk();

    remainingtime = (int*)initShm(65,&SCH_Proc_Shm);

    int prevClk = -1;
    while (*remainingtime > 0)
    {
        while(getClk() == prevClk);
        (*remainingtime)--;
        prevClk = getClk();
    }

    destroyClk(false);
    return 0;
}