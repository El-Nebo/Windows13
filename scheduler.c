#include "headers.h"
#include "DataStructures/Queue.h"
#include "DataStructures/PriorityQueue.h"

#define QUEUE 1
#define PRIORITY_QUEUE 0

#define BY_REMAININGTIME 1
#define BY_PRIORITY 0
#define BY_RUNTIME 2

void FCFS();
void SJP();
void HPF();
void SRTN();
void RR();

void FreeProcess(Process*);
int AllocateMemory(Process*, int*);

void FreeProcess(Process*a){}
int AllocateMemory(Process*a, int*b){return 1;}

void MemoryStuff(void *, int ,Process*,int);

FILE *schedularFile;

int choseAlgo = -1;
int quanta;
int PG_SCH_MsgQ;
int *ProcessRemainingTime;
int Sch_P_Shm_ID;
void handler(int signum);
int EXIT = 0;

Queue* WaitingList;
int main(int argc, char *argv[])
{   
    signal(SIGINT,handler);
    choseAlgo = atoi(argv[1]);
    quanta = atoi(argv[2]);
    printf("Scheduler: %d %d\n",choseAlgo,quanta);

    initClk();

    PG_SCH_MsgQ = initMsgQueue(66);
    ProcessRemainingTime = (int*)initShm(65,&Sch_P_Shm_ID);

    WaitingList = createQueue();

    schedularFile = fopen("schedular.log", "w");

    fprintf(schedularFile, "#At time x process y (Status)\n");

    switch (choseAlgo)
    {
    case 0:
        FCFS();
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

    fflush(schedularFile);
    fclose(schedularFile);

    msgctl(PG_SCH_MsgQ, IPC_RMID, (struct msqid_ds *)0);
    shmctl(Sch_P_Shm_ID, IPC_RMID, NULL);
    destroyClk(true);
}
void handler(int signum){
    EXIT = 1;
}
//========================================================================
///////////////////////Processes Operations/////////////////////////////////////////////
int forkProcess(Process* p){
    p->pid = fork();
    if(p->pid == -1)
        perror("Error forking a process");
    else if (p->pid == 0){
        execl("process.out","process",NULL);
    }

    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    //&&&&&&&&&&&&&&&&&&&& TODO: Write on file &&&&&&&&&&&&&&&&&&&
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    fprintf(schedularFile, " At time %d process %d started\n",getClk(),p->id);
    fflush(schedularFile);
}

void stopProcess(Process* p){
    //printf("YYYYYYYYY %d\n",p->pid);
    kill(p->pid,SIGSTOP);
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    //&&&&&&&&&&&&&&&&&&&& TODO: Write on file &&&&&&&&&&&&&&&&&&&
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    fprintf(schedularFile, " At time %d process %d stoped\n",getClk(),p->id);
    fflush(schedularFile);

    *ProcessRemainingTime = -1;
}

void contProcess(Process* p){
    kill(p->pid,SIGCONT);
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    //&&&&&&&&&&&&&&&&&&&& TODO: Write on file &&&&&&&&&&&&&&&&&&&
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    fprintf(schedularFile, " At time %d process %d resumed\n",getClk(),p->id);
    fflush(schedularFile);
}

void EndProcess(void* qq, int QType, Process* p, int priorty){
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    //&&&&&&&&&&&&&&&& TODO: Calc WT w el7agat dh &&&&&&&&&&&&&&&&
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    //&&&&&&&&&&&&&&&&&&&& TODO: Write on file &&&&&&&&&&&&&&&&&&&
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    fprintf(schedularFile, " At time %d process %d finished\n",getClk(),p->id);
    fflush(schedularFile);


    //===============Memeory Stuff=====================
    FreeProcess(p);


    int waitingPrcsCnt = WaitingList->size;
    for(int i = 0 ; i < waitingPrcsCnt ; i++){
        Process temp = QueueFront(WaitingList);
        QueuePop(WaitingList);
        MemoryStuff(qq,QType,&temp,priorty);
    }
    //--------------------------------------------------
    *ProcessRemainingTime = -1;
}
///////////////////////Processes Operations/////////////////////////////////////////////
//========================================================================

void MemoryStuff(void * qq, int QType,Process* p,int priorty){
    int UpperLimit;
    int AllocateMem = AllocateMemory(p,&UpperLimit);

    if(AllocateMem == -1){
        QueuePush(WaitingList,p);
    }
    else{
        p->MemoryOffset = AllocateMem;
        if(QType == QUEUE){ // qq is normal queue
            QueuePush((Queue*)qq,p);
        }else{//qq is priority queue 
            Priority_QueuePush((Priority_Queue*)qq,p,priorty);
        }

    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    //&&&&&&&&&&&&&&&&&&&& TODO: Write on file &&&&&&&&&&&&&&&&&&&
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    }
}

//========================================================================
///////////////////////Algos Implementation/////////////////////////////////////////////
void FCSC_receiveComingProcesses(Queue* q){
    while(1){
            Process p = receiveMessage(PG_SCH_MsgQ);
            if(p.IsProcess == 0) break;
            //QueuePush(q,&p);
            MemoryStuff((void*)q, QUEUE, &p,-1);
        }
}
void FCFS(){
    int prevClk = -1;
    Queue* q = createQueue();
    *ProcessRemainingTime = -1;
    int CPU_idle = 0;
    Process runningProcess;
    while(!EXIT){
        while(getClk() == prevClk);
        printf("SCh: Current Time %d\n",getClk());
        int curtime = getClk();
        FCSC_receiveComingProcesses(q);//receive coming processes from process generator

        if (*ProcessRemainingTime == 0){// a process has just started
            EndProcess(q,QUEUE,&runningProcess,-1);
        }
        if(*ProcessRemainingTime == -1){//No running Process
            if(QueueSize(q) == 0){//No waiting Process-- Do nothing and calc CPU utilization
                CPU_idle++;
            }
            else{//There is a waiting process
                runningProcess = QueueFront(q);
                QueuePop(q);
                *ProcessRemainingTime = runningProcess.runTime+1;
                forkProcess(&runningProcess);
            }
        }
    }
    DestroyQueue(q);
}
//-------------------------------------------------------------------
void SJP_receiveComingProcesses(Priority_Queue* q){
    while(1){
            Process p = receiveMessage(PG_SCH_MsgQ);
            if(p.IsProcess == 0) break;
            //Priority_QueuePush(q,&p,p.runTime);
            MemoryStuff((void*)q, PRIORITY_QUEUE, &p,p.runTime);
        }
}

void SJP(){
    int prevClk = -1;
    Priority_Queue* q = create_Priority_Queue();
    *ProcessRemainingTime = -1;
    int CPU_idle = 0;
    Process runningProcess;
    while(!EXIT){
        while(getClk() == prevClk);
        printf("SCh: Current Time %d\n",getClk());
        int curtime = getClk();
        SJP_receiveComingProcesses(q);//receive coming processes from process generator

        if (*ProcessRemainingTime == 0){// a process has just started
            EndProcess(q,QUEUE,&runningProcess,BY_RUNTIME);
        }
        if(*ProcessRemainingTime == -1){//No running Process
            if(Priority_QueueSize(q) == 0){//No waiting Process-- Do nothing and calc CPU utilization
                CPU_idle++;
            }
            else{//There is a waiting process
                runningProcess = Priority_QueuePeekValue(q);
                Priority_QueuePop(q);
                *ProcessRemainingTime = runningProcess.runTime+1;
                forkProcess(&runningProcess);
            }
        }
    }
    Destroy_Priority_Queue(q);

}
//-------------------------------------------------------------------
void HPF_recieveComingProcess(Priority_Queue *pq)
{
    while (1)
    {
        Process p = receiveMessage(PG_SCH_MsgQ);
        if (p.IsProcess == 0)
            break;

        Priority_QueuePush(pq, &p, p.priority);
    }
}

void HPF(){
  int prevClk = -1;
    int firstProc = true;
    Process runningProcess;
    runningProcess.priority = 100;
    Priority_Queue *pq = create_Priority_Queue();
    *ProcessRemainingTime = -1;
    while (!EXIT)
    {
        while (getClk() == prevClk)
            ;
            prevClk = getClk();
        printf("SCh: Current Time %d\n", getClk());
        int curtime = getClk();
        HPF_recieveComingProcess(pq);
        for(int i = 0 ; i<pq->size ; i++){
            printf("%d %d %d\n", pq->NodeArray[i]->value.id,
            pq->NodeArray[i]->value.priority,
            pq->NodeArray[i]->value.remainingTime);
        }
        //printf("Abbbbb\n");
        if (firstProc == true && pq->size > 0)
        {
            runningProcess = Priority_QueuePeekValue(pq); // for first process
            Priority_QueuePop(pq);
            *ProcessRemainingTime = runningProcess.runTime + 1;
            forkProcess(&runningProcess);

            firstProc = false;
            continue;
        }
        //printf("\n process remaing time %d \n ", *ProcessRemainingTime);

        if (*ProcessRemainingTime == 0){
            EndProcess(pq,PRIORITY_QUEUE,&runningProcess,runningProcess.priority);
        }

        if (pq->size > 0)
        {
            Process pS = Priority_QueuePeekValue(pq);
            Priority_QueuePop(pq);
            if (pS.priority < runningProcess.priority) // if the next process has a pariority more than the running
            {
                printf("\n process remaing time %d \n ", *ProcessRemainingTime);
                runningProcess.remainingTime = *ProcessRemainingTime;
                Priority_QueuePush(pq, &runningProcess, runningProcess.priority);
                stopProcess(&runningProcess);
            }

            if (*ProcessRemainingTime == -1)
            {
                runningProcess = pS;
                //Priority_QueuePop(pq); // pop pS
                *ProcessRemainingTime = runningProcess.remainingTime + 1;

                if (runningProcess.remainingTime == runningProcess.runTime) //a new process(first time)
                    forkProcess(&runningProcess);
                else
                    contProcess(&runningProcess);
            }else{
                Priority_QueuePush(pq,&pS,pS.priority);
            }
        }
    }

    Destroy_Priority_Queue(pq);
}
//-------------------------------------------------------------------
void SRTN_receiveComingProcesses(Priority_Queue* q){
    while(1){
            Process p = receiveMessage(PG_SCH_MsgQ);
            if(p.IsProcess == 0) break;
            //Priority_QueuePush(q,&p,p.remainingTime);
            MemoryStuff((void*)q, PRIORITY_QUEUE, &p,p.remainingTime);
        }
    //printf("TTTTTTTTT  %d\n",q->size);
}

void SRTN(){
    int prevClk = -1;
    Priority_Queue* q = create_Priority_Queue();
    *ProcessRemainingTime = -1;
    int CPU_idle = 0;
    Process runningProcess;
    runningProcess.remainingTime = 0;
    while(!EXIT){
        while(getClk() == prevClk);
        printf("SCh: Current Time %d\n",getClk());
        int curtime = getClk();
        SRTN_receiveComingProcesses(q);//receive coming processes from process generator

        if (*ProcessRemainingTime == 0){// a process has just finished
            EndProcess(q,PRIORITY_QUEUE,&runningProcess,BY_REMAININGTIME);
        }
        
        if(q->size > 0){
            Process Next = Priority_QueuePeekValue(q);
            Priority_QueuePop(q);
            if(Next.remainingTime < *ProcessRemainingTime){
                runningProcess.remainingTime = *ProcessRemainingTime;
                Priority_QueuePush(q,&runningProcess,runningProcess.remainingTime);
                stopProcess(&runningProcess);
            }

            if(*ProcessRemainingTime == -1){//either last process has finished or stoped
                runningProcess = Next;
                *ProcessRemainingTime = runningProcess.remainingTime+1;

                if(runningProcess.remainingTime == runningProcess.runTime)//a new process(first time)
                    forkProcess(&runningProcess);
                else 
                    contProcess(&runningProcess);
            }   else{
                Priority_QueuePush(q,&Next,Next.remainingTime);
            }
        }
    }
    Destroy_Priority_Queue(q);
}
void RR(){
    
}
///////////////////////Algos Implementation/////////////////////////////////////////////
//========================================================================
