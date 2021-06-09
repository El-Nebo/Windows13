#include "headers.h"
#include "DataStructures/Queue.h"
#include "DataStructures/PriorityQueue.h"
#include "DataStructures/LinkedList.h"

#define QUEUE 1
#define PRIORITY_QUEUE 0

#define BY_REMAININGTIME 1
#define BY_PRIORITY 0
#define BY_RUNTIME 2

#define STARTED 0
#define STOPED 1
#define RESUMED 2
#define FINISHED 3

void FCFS();
void SJP();
void HPF();
void SRTN();
void RR();

void FreeProcess(Process*);
int AllocateMemory(Process*, int*);

//void FreeProcess(Process*a){}
//int AllocateMemory(Process*a, int*b){return 1;}

void MemoryStuff(void *, int ,Process*,int);

void handler(int signum);

FILE *schedularFile;
FILE *schedulerFinalFile;
FILE *MemoryFile;
//--------------general 
int choseAlgo = -1;
int PG_SCH_MsgQ;
int *ProcessRemainingTime;
int Sch_P_Shm_ID;
int EXIT = 0;
int NUMProcesses;
int UNCHANGED_NUMProcesses;
//--------------Summary 
double WTA_SUM = 0;
double Waiting_SUM =0; 
int Processes_Count = 0;
int Execution_SUM = 0;
int FinishTime;
Queue* WaitingList;
//--------------Memory 
int memPolicy = 0;
int AllocateMemory(Process *p, int *UpperLimit);
int firstFit(Process *p, int *UpperLimit);
NODE* indicator;
int bestFit(Process *p, int *UpperLimit);
struct linkedList *memory;
int covertMemoryToCircularLinkedlisty(struct Process *p, int *UpperLimit);
int nextFit(struct Process *p, int *UpperLimit,int);

int main(int argc, char *argv[])
{   
    printf("TTTTTTTTTTJJJJTJTJTJ\n");
    signal(SIGINT,handler);
    choseAlgo = atoi(argv[1]);
    int quantum = atoi(argv[2]);
    NUMProcesses = atoi(argv[3]);
    memPolicy = atoi(argv[4]);

    UNCHANGED_NUMProcesses = NUMProcesses;
    printf("Scheduler: %d %d %d %d\n",choseAlgo,quantum, NUMProcesses,memPolicy);

    initClk();

    PG_SCH_MsgQ = initMsgQueue(66);
    ProcessRemainingTime = (int*)initShm(65,&Sch_P_Shm_ID);

    WaitingList = createQueue();
    memory = creatLinkedList();

    schedularFile = fopen("schedular.log", "w");
    schedulerFinalFile = fopen("scheduler.perf","w");
    MemoryFile = fopen("memory.log","w");

    fprintf(schedularFile, "#At time x process y state arr w total z remain y wait k\n");
    fprintf(schedularFile, "#At time x allocated y bytes for process z from i to j\n");

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
        RR(quantum);
        break;
    }

    fprintf(schedulerFinalFile,"CPU utilization = %.2f%%\n",Execution_SUM*100.0/(FinishTime-1));
    fprintf(schedulerFinalFile,"Avg WTA = %.2f%%\n",WTA_SUM/Processes_Count);
    fprintf(schedulerFinalFile,"Avg Waiting = %.2f%%\n",Waiting_SUM/Processes_Count);

    fflush(schedularFile);
    fclose(schedularFile);
    
    fflush(schedulerFinalFile);
    fclose(schedulerFinalFile);

    fflush(MemoryFile);
    fclose(MemoryFile);

    msgctl(PG_SCH_MsgQ, IPC_RMID, (struct msqid_ds *)0);
    shmctl(Sch_P_Shm_ID, IPC_RMID, NULL);
    destroyClk(true);
}
void handler(int signum){
    if(signum == SIGINT)
        EXIT = 1;
}
//========================================================================
///////////////////////Processes Operations/////////////////////////////////////////////
void PrintLine_SchedulerLog(Process* p,int status, int time){
    char msg[4][10] = {"started","stoped","resumed","finished"};
    fprintf(schedularFile,"At time %d process %d %s arr %d total %d remain %d wait %d",
    time,
    p->id,
    msg[status],
    p->arrivalTime,
    p->runTime,
    p->remainingTime,
    (time-p->arrivalTime) - (p->runTime-p->remainingTime));
    
    if(status == FINISHED){
        fprintf(schedularFile," TA %d WTA %.2f",
        time - p->arrivalTime,
        (time - p->arrivalTime)*1.0/p->runTime);
    }
    fprintf(schedularFile,"\n");
    fflush(schedularFile);
}

void PrintLine_Memory(Process* p,int status, int time){
    char temp[10];
    if(status == STARTED) strcpy(temp,"allocated");
    else if (status == FINISHED) strcpy(temp,"freed");
    fprintf(schedularFile, "#At time %d %s %d bytes for process %d from %d to %d\n",
    time,
    temp,
    p->MemorySize,
    p->id,
    p->MemoryOffset,
    p->MemoryOffset+p->MemorySize
    );
}
//================================================================
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
    PrintLine_SchedulerLog(p,STARTED, getClk());
    PrintLine_Memory(p,STARTED, getClk());
    // fprintf(schedularFile, " At time %d process %d started\n",getClk(),p->id);
    // fflush(schedularFile);
}

void stopProcess(Process* p){
    //printf("YYYYYYYYY %d\n",p->pid);
    kill(p->pid,SIGSTOP);
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    //&&&&&&&&&&&&&&&&&&&& TODO: Write on file &&&&&&&&&&&&&&&&&&&
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    PrintLine_SchedulerLog(p,STOPED, getClk());
    // fprintf(schedularFile, " At time %d process %d stoped\n",getClk(),p->id);
    // fflush(schedularFile);

    *ProcessRemainingTime = -1;
}

void contProcess(Process* p){
    kill(p->pid,SIGCONT);
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    //&&&&&&&&&&&&&&&&&&&& TODO: Write on file &&&&&&&&&&&&&&&&&&&
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    PrintLine_SchedulerLog(p,RESUMED, getClk());
    // fprintf(schedularFile, " At time %d process %d resumed\n",getClk(),p->id);
    // fflush(schedularFile);
}

void EndProcess(void* qq, int QType, Process* p, int priorty){
    p->remainingTime = 0;
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    //&&&&&&&&&&&&&&&& TODO: Calc WT w el7agat dh &&&&&&&&&&&&&&&&
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

    WTA_SUM += (getClk() - p->arrivalTime)*1.0/p->runTime;
    Waiting_SUM += (getClk()-p->arrivalTime) - (p->runTime-p->remainingTime);
    Processes_Count++;
    Execution_SUM += p->runTime;
    FinishTime = getClk();

    if(Processes_Count == UNCHANGED_NUMProcesses) EXIT = 1;
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    //&&&&&&&&&&&&&&&&&&&& TODO: Write on file &&&&&&&&&&&&&&&&&&&
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    PrintLine_SchedulerLog(p,FINISHED, getClk());
    PrintLine_Memory(p,FINISHED, getClk());
    // fprintf(schedularFile, " At time %d process %d finished\n",getClk(),p->id);
    // fflush(schedularFile);


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
void receiveComingProcesses(void* q,int QType, int prioritytype){
    while(NUMProcesses > 0){
            Process p = receiveMessage(PG_SCH_MsgQ);
            if(p.IsProcess == 0) break;
            NUMProcesses--;
            //QueuePush(q,&p);
            int z;
            if(prioritytype == BY_RUNTIME) z = p.runTime;
            else if (prioritytype == BY_PRIORITY) z = p.priority;
            else if (prioritytype == BY_REMAININGTIME) z = p.remainingTime;
            MemoryStuff((void*)q, QType, &p,prioritytype);
        }
}

// void FCSC_receiveComingProcesses(Queue* q){
//     while(NUMProcesses > 0){
//             Process p = receiveMessage(PG_SCH_MsgQ);
//             if(p.IsProcess == 0) break;
//             //QueuePush(q,&p);
//             MemoryStuff((void*)q, QUEUE, &p,-1);
//         }
// }

void FCFS(){
    int prevClk = -1;
    Queue* q = createQueue();
    *ProcessRemainingTime = -1;
    int CPU_idle = 0;
    Process runningProcess;
    while(!EXIT){
        while(getClk() == prevClk);
        prevClk = getClk();
        printf("SCh: Current Time %d\n",getClk());
        int curtime = getClk();
        if(NUMProcesses>0)  receiveComingProcesses(q,QUEUE,-1);//receive coming processes from process generator

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
// void SJP_receiveComingProcesses(Priority_Queue* q){
//     while(NUMProcesses > 0){
//             Process p = receiveMessage(PG_SCH_MsgQ);
//             if(p.IsProcess == 0) break;
//             //Priority_QueuePush(q,&p,p.runTime);
//             MemoryStuff((void*)q, PRIORITY_QUEUE, &p,p.runTime);
//         }
// }

void SJP(){
    int prevClk = -1;
    Priority_Queue* q = create_Priority_Queue();
    *ProcessRemainingTime = -1;
    int CPU_idle = 0;
    Process runningProcess;
    while(!EXIT){
        while(getClk() == prevClk);
        prevClk = getClk();
        printf("SCh: Current Time %d\n",getClk());
        int curtime = getClk();
        if(NUMProcesses>0)  receiveComingProcesses(q,PRIORITY_QUEUE,BY_RUNTIME);//receive coming processes from process generator

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
// void HPF_recieveComingProcess(Priority_Queue *pq)
// {
//     while (NUMProcesses > 0)
//     {
//         Process p = receiveMessage(PG_SCH_MsgQ);
//         if (p.IsProcess == 0)
//             break;
//         MemoryStuff((void*)pq, PRIORITY_QUEUE, &p,p.priority);
//         //Priority_QueuePush(pq, &p, p.priority);
//     }
// }

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
        if(NUMProcesses>0)  receiveComingProcesses(pq,PRIORITY_QUEUE,BY_PRIORITY);
        // for(int i = 0 ; i<pq->size ; i++){
        //     printf("%d %d %d\n", pq->NodeArray[i]->value.id,
        //     pq->NodeArray[i]->value.priority,
        //     pq->NodeArray[i]->value.remainingTime);
        // }
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
                //printf("\n process remaing time %d \n ", *ProcessRemainingTime);
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

// void SRTN_receiveComingProcesses(Priority_Queue* q){
//     while(1){
//             Process p = receiveMessage(PG_SCH_MsgQ);
//             if(p.IsProcess == 0) break;
//             //Priority_QueuePush(q,&p,p.remainingTime);
//             MemoryStuff((void*)q, PRIORITY_QUEUE, &p,p.remainingTime);
//         }
//     //printf("TTTTTTTTT  %d\n",q->size);
// }

void SRTN(){
    int prevClk = -1;
    Priority_Queue* q = create_Priority_Queue();
    *ProcessRemainingTime = -1;
    int CPU_idle = 0;
    Process runningProcess;
    runningProcess.remainingTime = 0;
    while(!EXIT){
        while(getClk() == prevClk);
        prevClk = getClk();
        printf("SCh: Current Time %d\n",getClk());
        if(getClk() == 68){
            for(int i = 0 ; i<q->size ; i++){
                printf("%d %d %d\n", q->NodeArray[i]->value.id,
                q->NodeArray[i]->value.priority,
                q->NodeArray[i]->value.remainingTime);
            }
        }
        if(NUMProcesses>0)  receiveComingProcesses(q,PRIORITY_QUEUE,BY_REMAININGTIME);//receive coming processes from process generator
        if(getClk() == 68){
            for(int i = 0 ; i<q->size ; i++){
                printf("%d %d %d\n", q->NodeArray[i]->value.id,
                q->NodeArray[i]->value.priority,
                q->NodeArray[i]->value.remainingTime);
            }
        }
        
        //printf("Abbbbb\n");
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

//-------------------------------------------------------------------

// void RR_receiveComingProcesses(Queue* q){
//     while(1){
//             Process p = receiveMessage(PG_SCH_MsgQ);
//             if(p.IsProcess == 0) break;
//             //QueuePush(q,&p);
//             MemoryStuff((void*)q, QUEUE, &p,-1);
//         }
// }
void RR(int quantum){
    printf("RR began %d\n", quantum);
    int prevClk = -1;
    Queue* q = createQueue();
    *ProcessRemainingTime = -1;
    int CPU_idle = 0;
    Process runningProcess;
    runningProcess.remainingTime = 0;
    int Qcnt = 0;

    while(!EXIT){
        while(getClk() == prevClk);
        prevClk = getClk();
        int curtime = getClk();
        printf("SCh: Current Time %d\n",getClk());
        if(NUMProcesses>0)  receiveComingProcesses(q,QUEUE,-1);//receive coming processes from process generator

        Qcnt--;

        // int aa = q->size;
        // for(int i = 0 ; i < aa ; i++){
        //     printf("%d %d\n",QueueFront(q).id,QueueFront(q).remainingTime);
        // }

        if(*ProcessRemainingTime == 0){
            EndProcess(q,QUEUE,&runningProcess,-1);
            Qcnt = quantum;
        }

        if(q->size > 0 && (Qcnt == 0 || *ProcessRemainingTime == -1)){
            Qcnt = quantum;

            if(*ProcessRemainingTime != -1){
                runningProcess.remainingTime = *ProcessRemainingTime;
                QueuePush(q,&runningProcess);
                stopProcess(&runningProcess);
            }

            runningProcess = QueueFront(q);
            QueuePop(q);

            *ProcessRemainingTime = runningProcess.remainingTime+1;

            if(runningProcess.remainingTime == runningProcess.runTime)//a new process(first time)
                    forkProcess(&runningProcess);
                else 
                    contProcess(&runningProcess);
        }
        if(Qcnt == 0){
            Qcnt = quantum;
        }
    }
    DestroyQueue(q);
}
///////////////////////Algos Implementation/////////////////////////////////////////////
//========================================================================

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//========================================================================
///////////////////////Memory/////////////////////////////////////////////
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

void FreeProcess(Process* p){
    VirtualDelete(memory,p->id);
}
int AllocateMemory(Process *p, int *UpperLimit)
{
    return 1;
    if (memPolicy == 0)
        return firstFit(p, UpperLimit);
    else if (memPolicy == 1)
         return nextFit(p, UpperLimit,0);
    else if (memPolicy == 2)
        return bestFit(p, UpperLimit);
}

int firstFit(struct Process *p, int *UpperLimit)
{
    /*if (UpperLimit > 1024)
        return -1;*/
    //first process
    NODE* prev = (NODE*)malloc(sizeof(NODE));
    if (memory->size == 0)
    {
        Insert(memory, p->id, 0, p->MemorySize);
        Insert(memory, -p->id,p->MemorySize,1024-p->MemorySize);
        p->MemoryOffset = 0;
        //UpperLimit += p->MemorySize;
        return 0;
    }
    else
    {
        NODE *ptr = memory->head;
        while (ptr != NULL && ((ptr->process_id > 0) || (ptr->process_id <= -1 && ptr->length < p->MemorySize)))
        {
            prev = ptr;
            ptr = ptr->next;
        }
        //printf("%d\n",memory->head->process_id);
        //not found
        if (ptr == NULL && (1024 - (prev->start + prev->length)) < p->MemorySize)
        {
            return -1;
        }
        else
        {
            //found in tail
            if (ptr == NULL)
            {
                Insert(memory, p->id, prev->start + prev->length, p->MemorySize);
                p->MemoryOffset = prev->start + prev->length;
                return p->MemoryOffset;
            }
                //found in between allocated in existed node
            else
            {
                //printf("TTTTTTTTTTTTTTTT\n");
                NODE *temp = getPrev(memory, ptr);
                int len = ptr->length;
                int start = ptr->start;
                int id = ptr->process_id;
                DeleteNode(memory, ptr->process_id);
                p->MemoryOffset = start;
                if (p->MemorySize == len)
                    InsertNextTo(memory, temp, p->id, start, p->MemorySize);
                else
                {
                    InsertNextTo(memory, temp, id, start + p->MemorySize, len - (p->MemorySize));
                    InsertNextTo(memory, temp, p->id, start, p->MemorySize);
                }
                return p->MemoryOffset;
            }
        }
    }
}
//******************
int nextFit(struct Process *p, int *UpperLimit,int times)
{
    /*if (UpperLimit > 1024)
        return -1;*/
    //first process
    //if(firstFit(p,UpperLimit) == -1) return -1;
    NODE* prev = (NODE*)malloc(sizeof(NODE));
    if (memory->size == 0)
    {
        Insert(memory, p->id, 0, p->MemorySize);
        Insert(memory, -p->id,p->MemorySize,1024-p->MemorySize);
        indicator = memory->head->next;
        p->MemoryOffset = 0;
        //UpperLimit += p->MemorySize;
        return 0;
    }
    else
    {
        NODE *ptr = indicator;
        while (ptr != NULL && ((ptr->process_id > 0) || (ptr->process_id <= -1 && ptr->length < p->MemorySize)))
        {
            prev = ptr;
            ptr = ptr->next;
        }
        //printf("%d\n",memory->head->process_id);
        //not found
        if (ptr == NULL && (1024 - (prev->start + prev->length)) < p->MemorySize)
        {
            indicator = memory->head;
            if(times == 1) return -1;
            return nextFit(p,UpperLimit,1);
        }
        else
        {
            //found in tail
            // if (ptr == NULL)
            // {
            //     Insert(memory, p->id, prev->start + prev->length, p->MemorySize);
            //     indicator = prev->next->next;
            //     p->MemoryOffset = prev->start + prev->length;
            //     return p->MemoryOffset;
            // }
            //found in between allocated in existed node
            // {
            //printf("TTTTTTTTTTTTTTTT\n");
            NODE *temp = getPrev(memory, ptr);
            int len = ptr->length;
            int start = ptr->start;
            int id = ptr->process_id;
            DeleteNode(memory, ptr->process_id);
            p->MemoryOffset = start;
            if (p->MemorySize == len){
                InsertNextTo(memory, temp, p->id, start, p->MemorySize);
                indicator = temp->next;
                if(indicator == NULL) indicator = memory->head;
            }
            else
            {
                InsertNextTo(memory, temp, id, start + p->MemorySize, len - (p->MemorySize));
                InsertNextTo(memory, temp, p->id, start, p->MemorySize);
                indicator = temp->next->next;
            }
            return p->MemoryOffset;
            // }
        }
    }
}
//================

int bestFit(struct Process *p, int *UpperLimit)
{
    /*if (UpperLimit > 1024)
        return -1;*/
    //first process
    NODE* prev = (NODE*)malloc(sizeof(NODE));
    if (memory->size == 0)
    {
        Insert(memory, p->id, 0, p->MemorySize);
        Insert(memory, -p->id, p->MemorySize, 1024 - p->MemorySize);
        p->MemoryOffset = 0;
        //UpperLimit += p->MemorySize;
        return 0;
    }
    else
    {
        NODE *ptr = memory->head;
        NODE *nowAprropriate = NULL;
        //NODE *wasAprropriate = NULL;
        //NODE*Temp=NULL;
        for (size_t i = 0; i < memory->size; i++)
        {
            //first place
            if(ptr->length>=p->MemorySize&&nowAprropriate==NULL&&ptr->process_id<0)
            {
                nowAprropriate=ptr;
            }
                //any other place
            else if(ptr->length>=p->MemorySize&&ptr->process_id<0)
            {
                if((nowAprropriate->length)>(ptr->length))
                {
                    nowAprropriate=ptr;
                }
            }
            ptr=ptr->next;
        }
        ptr=nowAprropriate;
        //printf("%d\n",memory->head->process_id);
        //not found
        if (ptr == NULL)
        {
            return -1;
        }
        else
        {
            //printf("TTTTTTTTTTTTTTTT\n");
            NODE *temp = getPrev(memory, ptr);
            int len = ptr->length;
            int start = ptr->start;
            int id = ptr->process_id;
            DeleteNode(memory, ptr->process_id);
            p->MemoryOffset = start;
            if (p->MemorySize == len)
                InsertNextTo(memory, temp, p->id,start, p->MemorySize);
            else
            {
                InsertNextTo(memory, temp, -p->id, start + p->MemorySize, len - (p->MemorySize));
                InsertNextTo(memory, temp, p->id, start, p->MemorySize);
            }
            return p->MemoryOffset;
        }
    }
}