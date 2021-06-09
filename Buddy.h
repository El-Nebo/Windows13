#pragma once
#include "DataStructures/LinkedList.h"
#include <math.h>

struct linkedList * totalMem[11 ];


void intialize()
{
    for (int i = 2; i <= 10; i++)
    {
        totalMem[i] = creatLinkedList();
    }
    InsertOrdered(totalMem[10], 0);
}



int powerindex(int reqMemorySize)
{
    int powerIndex = (int)log2(reqMemorySize);
    if (reqMemorySize == (int)pow(2, powerIndex))
        return powerIndex;
    else
        return powerIndex + 1;
}

int splitMem(int powerIndex, int startIndex)
{
    int blockSize = (int)pow(2, powerIndex);
 
    powerIndex--;
        InsertOrdered(totalMem[powerIndex], startIndex + blockSize / 2);
    return 1;
}

int allocateMem(int reqMemorySize)
{
    
    int powerIndex = powerindex(reqMemorySize);
    if (powerIndex < 2)
        powerIndex = 2;



    int Splits = 0;
    int i = powerIndex;
    while (i <= 10 && isempty(totalMem[i]))
    {
        Splits++;
        i++;
    }
 

  

    int startIndex;
    startIndex = totalMem[i]->head->start;


    DeleteNode(totalMem[i], startIndex);
    while (Splits > 0)
    {
        splitMem(i, startIndex);
        i--;
        Splits--;
    }

    return startIndex;
}
void printMem()
{
    for (int i = 2; i <= 10; i++)
    {
        printf(" [%4d] ", (int)pow(2, i));
        Traverse(totalMem[i]);
        printf("\n");
    }
}

