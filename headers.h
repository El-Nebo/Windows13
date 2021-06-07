#include <stdio.h> //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

typedef short bool;
#define true 1
#define false 0

#define SHKEY 300

//========================================================================
///////////////////////STRUCTS/////////////////////////////////////////////
struct Process
{
    int id ; 
    int arrivalTime ; 
    int runTime; 
    int priority ;
    int status; // 0 reday 1 running
    int IsProcess;
}; 
typedef struct Process Process;


struct Message
{
    int mtype;
    Process process;
};
typedef struct Message Message;

struct msgschedulerprocess 
{
    long mtype ; 
    int remainingTime ;  
}; 
typedef struct msgchedulerprocess msgschedulerprocess;
///////////////////////STRUCTS/////////////////////////////////////////////
//=======================================================================


///==============================
//don't mess with this variable//
int *shmaddr; //
//===============================

//========================================================================
///////////////////////CLOCK/////////////////////////////////////////////
int getClk()
{
    return *shmaddr;
}

/*
 * All processes call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
*/
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *)shmat(shmid, (void *)0, 0);
}

/*
 * All processes call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
*/

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}
///////////////////////CLOCK/////////////////////////////////////////////
//========================================================================



//========================================================================
///////////////////////MSGQUEUE/////////////////////////////////////////////

int initMsgQueue(char key)
{
    key_t msgqKey = ftok("keyfile", key);
    int msgqId = msgget(msgqKey, 0666 | IPC_CREAT);

    if (!~msgqId)
    {
        perror("Error in creating of message queue");
        exit(-1);
    }
    return msgqId;
}

void sendMessage(int msgqId, Process process)
{
    Message message;
    message.process = process;
    message.mtype = 1;
    int send_val = msgsnd(msgqId, &message, sizeof(message.process), !IPC_NOWAIT);
    if (send_val == -1)
        perror("Error in sending the process");
}

Process receiveMessage(int msgqId)
{
    Process process;
    Message message;
    int rec_val = msgrcv(msgqId, &message, sizeof(message.process), 0, !IPC_NOWAIT);
    process = message.process;
    if (rec_val == -1)
        perror("Error in receive");
    
    return process;
}

///////////////////////MSGQUEUE/////////////////////////////////////////////
//========================================================================

//========================================================================
///////////////////////SHARED MEMORRY/////////////////////////////////////////////
void *initShm(char key, int *id)
{
    key_t shmKey = ftok("keyfile", key);
    int shmId = shmget(shmKey, 4096, 0666 | IPC_CREAT);
    *id = shmId;

    if (!~shmId)
    {
        perror("Error in creating of message queue");
        exit(-1);
    }

    void *addr = shmat(shmId, (void *)0, 0);
    return addr;
}
///////////////////////SHARED MEMORRY/////////////////////////////////////////////
//========================================================================