#include "headers.h"

void FCSC();
void SJP();
void HPF();
void SRTN();
void RR();

int choseAlgo = -1;
int quanta;
int PG_SCH_MsgQ;
int *ProcessRemainingTime;
int Sch_P_Shm_ID;
int main(int argc, char *argv[])
{
    choseAlgo = atoi(argv[1]);
    quanta = atoi(argv[2]);
    printf("%d %d\n",choseAlgo,quanta);

    initClk();

    PG_SCH_MsgQ = initMsgQueue(66);
    ProcessRemainingTime = (int*)initShm(65,&Sch_P_Shm_ID);
    switch (choseAlgo)
    {
    case 0:
        FCSC();
        break;
    case 1:
        SJP();
        break;
    case 2:
        HPF();
        break;
    case 3:
        SRTN();
        break;
    case 4:
        RR();
        break;
    }

    msgctl(PG_SCH_MsgQ, IPC_RMID, (struct msqid_ds *)0);
    shmctl(Sch_P_Shm_ID, IPC_RMID, NULL);
    destroyClk(true);
}

void FCSC(){
    int prevClk = -1;
    while(1){
        while(getClk() == prevClk);
        printf("SCh: Current Time %d\n",getClk());
        int curtime = getClk();
        int cnt = 0;
        while(1){
            Process p = receiveMessage(PG_SCH_MsgQ);
            if(p.IsProcess == 0) break;
            printf("%d\t", p.id);
            printf("%d\t", p.arrivalTime);
            printf("%d\t", p.runTime);
            printf("%d\n", p.priority);
        }
    }
}
void SJP(){
    
}
void HPF(){
    
}
void SRTN(){
    
}
void RR(){
    
}


// //first come first serve
// void fcfs()
// {
//     struct Process *head;
//     struct Queue *q = createQueue();
//     int pid = 0;
//     //**********************************************
//     key_t key_id = ftok("key", 100);
//     int msg_id = msgget(key_id, 0666 | IPC_CREAT);
//     struct msgschedulerprocess msg_sch_proc;
//     //************************************************
//     while (true)
//     {
//         //**************************************Reciving processes************************************
//         int recievedprocs = msgrcv(msg_id, &recPack, sizeof(recPack.procs), 0, !IPC_NOWAIT);

//         if (recievedprocs == -1)
//         {
//             perror("error in recieveng from generator to scheduler ");
//             break;
//         }

//         else if (1)
//         {
//             recievedProcs = recPack.procs;
//             //set all recieved proceeses as ready
//             recievedProcs.status = 0;
//             printf("process Arrival Time %d \n ", recievedProcs.arrivalTime);
//             printf("process running Time %d \n ", recievedProcs.runTime);
//             printf("process priority %d \n ", recievedProcs.priority);
//             printf("process id  %d \n ", recievedProcs.id);

//             //**************************************************************************
//             head = &recievedProcs;
//             struct Node *n = createNode(head);
//             QueuePush(q, n);
//             head = QueueFront(q);
//             if (head->status == 0)
//             {
//                 head->status = 1;
//                 pid = fork();
//                 if (pid == -1)
//                     perror("error in fork");
//                 else if (pid == 0) /* only execute this if child(process) */
//                 {
//                     system("gcc process.c -o process.out");
//                     char geClk[500];
//                     sprintf(geClk, "%d", getClk());
//                     char ruTime[500];
//                     sprintf(ruTime, "%d", head->runTime);
//                     char algo[500];
//                     sprintf(algo, "%d", choseAlgo);
//                     execl("process.out", "process", geClk, ruTime, algo, NULL);
//                 }
//                 else //scheduler
//                 {
//                     int msgRcv = msgrcv(msg_id, &msg_sch_proc, sizeof(msg_sch_proc.remainingTime), 0, !IPC_NOWAIT);
//                     if (msgRcv == -1)
//                         perror("Error in REceiveing from process\n");
//                     else
//                     {
//                         printf("Current process id is %d and it is remaining = %d", head->arrivalTime, msg_sch_proc.remainingTime);
//                         if (msg_sch_proc.remainingTime == 0)
//                             QueuePop(q);
//                     }
//                 }
//             }
//         }
//     }
// }