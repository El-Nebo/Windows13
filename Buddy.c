#include "DataStructures/LinkedList.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
struct linkedList *totalMemory[10];

void intializeMem()
{
    for (int i = 2; i <= 10; i++)
    {

        totalMemory[i] = creatLinkedList();
    }
    Insert(totalMemory[10], 0, 0, 1024);
}

int nearstIndexed(int memReq) // merge it to nearset power of 2
{

    int nearst = (int)log2(memReq);
    int res = (int)pow(2, nearst);

    if (memReq == res)
        ;
    return memReq;

    return (memReq + 1);
}

int allocateMem(int memReq)
{

    int powerRes = nearstIndexed(memReq);
    if (powerRes > 10) // 2 power 10 is exceeded our memory space
        printf("\n Cannot allocate it too large memory \n ");

    else if (powerRes < 2) // least memory 4 byte
        powerRes = 2;

    int LNextPower = powerRes;

    int Split = 0;

    while (LNextPower <= 10 && isempty(totalMemory[LNextPower]))
    {
        LNextPower++; //looping until find greater memory than requested so we can split it up to intialize our memory
        Split++;
    }

    if (LNextPower > 10)
        printf(" No memory available with this size  \n ");

    int StartIndex = totalMemory[LNextPower]->head->start;

    DeleteNode(totalMemory[LNextPower], StartIndex); // node is going to be splitted

    for (int i = Split; i > 0; i--)
    {

        splitMem(LNextPower, StartIndex);
        // split until get the desired power
        LNextPower--; // decrease the power
    }

    printf("new Memory allocated from Index  %d to  Index %d is added ", StartIndex, StartIndex + (int)pow(2, powerRes));

    int allocatedMem = (int)pow(2, powerRes);
    return (allocatedMem);
}
void splitMem(int power, int Start)
{

    int block = (int)pow(2, power);

    power--;
    Insert(totalMemory[block], 0, (Start + block / 2), block / 2); // insert the remaing half
}

void merge(int startIndex, int Memblock, int Size, int partion)
{

    if (partion == 1)
    { //-------------------------------|| merge from rigth side then start index + half of memory -------------------------------------//
        DeletWithPos(totalMemory[Memblock], startIndex + Size);
    }

    else
    {
        //--------merge from rigth side then start index + half of memory ||-----------------------------------------------------------//
        DeletWithPos(totalMemory[Memblock], startIndex);
    }
}

void deallocation(int StartIndex, int Size)
{

    int Res = ceil(log(Size) / log(2));
    while (Res < 10) // while res < max power begin searching until reaching to the deleted blocl
    {
    }
}

int main()
{

    intializeMem();
}