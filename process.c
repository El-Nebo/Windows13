#include "headers.h"

/* Modify this file as needed*/


int main(int agrc, char *argv[])
{
    initClk();
    //Getting data from scheduler
int startingTime=atoi(argv[1]);
int runTime=atoi(argv[2]);
int algo=atoi(argv[3]);
 int finishTime=startingTime+runTime; 
 int remainingtime; 
  //initialize msg_queue with scheduler
key_t key_id = ftok("key", 100);
int msg_id = msgget(key_id, 0666 | IPC_CREAT);
struct msgschedulerprocess msg;
 
//choose algo
switch (algo)
{
    //----------------------------------fcfs---------------------------------------
case 0:
     
      while (getClk()!=finishTime)
    {
        remainingtime=finishTime-getClk();
        int sendval = msgsnd(msg_id, &msg, sizeof(msg.remainingTime), !IPC_NOWAIT);
            if (sendval == -1)
        perror("error in sending from generator to scheduler ");
    }
    remainingtime--;
    int sendval = msgsnd(msg_id, &msg, sizeof(msg.remainingTime), !IPC_NOWAIT);
    break;

default:
    break;
}

    
   /* //TODO The process needs to get the remaining time from somewhere
    //remainingtime = ??;
    while (remainingtime > 0)
    {
        // remainingtime = ??;
    }*/

    destroyClk(false);

    return 0;
}
