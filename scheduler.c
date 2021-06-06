#include "headers.h"
#include "queue.h"
void fcfs();
key_t key_id;
int msg_id;
struct processData recievedProcs;
struct processesBuff recPack;
int choseAlgo = -1;
int main(int argc, char *argv[])
{
    initClk();

    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.

    key_id = ftok("key", 66);
    msg_id = msgget(key_id, 0666 | IPC_CREAT);

    struct msgEntryBuff intializationrec;

    int NumProcesses = -1;

    int recievedProcess = 0;
    intializationrec.mtype=1;

    int rec = msgrcv(msg_id, &intializationrec, sizeof(intializationrec.Algonum) + sizeof(intializationrec.numProccess), 0, !IPC_NOWAIT);

    if (rec == -1)
        perror("cannot recieve from the genrator ");

    else
    {

        printf("Recieved from the genrator \n");
        printf("Num of proccess is recieved %d \n ", intializationrec.numProccess);
        printf(" chosen Algorithm is %d \n ", intializationrec.Algonum);

        NumProcesses = intializationrec.numProccess;
        choseAlgo = intializationrec.Algonum;
    }

    switch (choseAlgo)
    {
        //sirst come first serve fcfs
    case 0:
        fcfs();
        break;

    default:
        break;
    }

    //  destroyClk(true);
}

//first come first serve
void fcfs()
{
    struct processData *head;
    struct Queue *q = createQueue();
    int pid = 0;
    //**********************************************
    key_t key_id = ftok("key", 100);
    int msg_id = msgget(key_id, 0666 | IPC_CREAT);
    struct msgschedulerprocess msg_sch_proc;
    //************************************************
    while (true)
    {
        //**************************************Reciving processes************************************
        int recievedprocs = msgrcv(msg_id, &recPack, sizeof(recPack.procs), 0, !IPC_NOWAIT);

        if (recievedprocs == -1)
        {
            perror("error in recieveng from generator to scheduler ");
            break;
        }

        else if (recievedProcs.validData)
        {
            recievedProcs = recPack.procs;
            //set all recieved proceeses as ready
            recievedProcs.status = 0;
            printf("process Arrival Time %d \n ", recievedProcs.arrivalTime);
            printf("process running Time %d \n ", recievedProcs.runTime);
            printf("process priority %d \n ", recievedProcs.priority);
            printf("process id  %d \n ", recievedProcs.id);

            //**************************************************************************
            head = &recievedProcs;
            struct Node *n = createNode(head);
            QueuePush(q, n);
            head = QueueFront(q);
            if (head->status == 0)
            {
                head->status = 1;
                pid = fork();
                if (pid == -1)
                    perror("error in fork");
                else if (pid == 0) /* only execute this if child(process) */
                {
                    system("gcc process.c -o process.out");
                    char geClk[500];
                    sprintf(geClk, "%d", getClk());
                    char ruTime[500];
                    sprintf(ruTime, "%d", head->runTime);
                     char algo[500];
                    sprintf(algo, "%d", choseAlgo);
                    execl("process.out", "process", geClk, ruTime,algo,NULL);
                }
                else //scheduler
                {
                           int msgRcv = msgrcv(msg_id, &msg_sch_proc, sizeof(msg_sch_proc.remainingTime), 0, !IPC_NOWAIT);
                          if(msgRcv==-1)
                          perror("Error in REceiveing from process\n");
                          else
                          {
                              printf("Current process id is %d and it is remaining = %d",head->arrivalTime,msg_sch_proc.remainingTime);
                              if(msg_sch_proc.remainingTime==0)
                              QueuePop(q);
                          } 
                }
            }
        }
    }

}