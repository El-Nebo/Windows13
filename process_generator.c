#include "headers.h"
// void clearResources(int sig);

enum Algo
{
    FCSC, // First Come First Serve
    SJP,  // Shortest Job First
    HPF,  // Preemtivr Highest priorty  first
    SRTN, //  Shortest Remaining Time Next
    RR    // Round Robin
};

int main(int argc, char *argv[])
{
    // signal(SIGINT, clearResources);

    FILE *Process_Data;

    if (argc < 2)
    {

        printf(" unsufficient Data please provide the file path ");
        exit(-1);
    }

    char dummy[100];

    struct processData *Processdata = malloc(sizeof(struct processData) * 100); // 100 formemmory size can be changed later

    Process_Data = fopen(argv[1], "r");
    int processCount = 0;
    struct processData temp;

    // note this 4 dummy for the entry of the input #id arrival runtime priority
    fscanf(Process_Data, "%s", dummy);
    printf(" %s ", dummy);
    fscanf(Process_Data, "%s", dummy);
    printf(" %s ", dummy);

    fscanf(Process_Data, "%s", dummy);
    printf("%s ", dummy);

    fscanf(Process_Data, "%s", dummy);
    printf("%s\n  ", dummy);

    ///////////////////////////////////////////////////////////////////

    while (fscanf(Process_Data, "%d", &temp.id))
    {

        fscanf(Process_Data, "%d", &temp.arrivalTime);
        fscanf(Process_Data, "%d", &temp.runTime);
        fscanf(Process_Data, "%d", &temp.priority);

        if (Processdata[processCount - 1].id == temp.id)
            break;

        Processdata[processCount] = temp;

        printf("%d ", Processdata[processCount].id);
        printf("%d ", Processdata[processCount].arrivalTime);
        printf("%d ", Processdata[processCount].runTime);
        printf("%d  \n ", Processdata[processCount].priority);
        processCount++;
    }

    int chosenAlgo = -1;
    printf(" \n First Come First Serve = 0 ,   Shortest Job First  = 1 ,Preemtive Highest priorty  first = 2 \n");
    printf(" Shortest Remaining Time Next = 3 ,  Round Robin = 4 \n");
    scanf("%d", &chosenAlgo);

    /////////////////////////////////////////////////////////////////////////////////
    // creating clk and scheduler process and start execute them
    int Clk_ID = fork();
    if (Clk_ID == -1)
    {
        printf(" \n Error in forking clock  \n ");
    }

    else if (Clk_ID == 0)
    {
        printf("\n fork clock successfull\n ");

        execl("clk.out", "clk", NULL);
    }

    int Scheduler_ID = fork();
    if (Scheduler_ID == -1)
        printf("\n error in forking schedular ");

    else if (Scheduler_ID == 0)
    {
        printf("\n fork Scheduler successfull\n ");
        system("gcc scheduler.c -o scheduler.out");
        execl("scheduler.out", "scheduler", NULL);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////

    // beginning of sending all the intialiezed process to the scheduler to deal with them one by one

    key_t key_id = ftok("key", 66);
    int msg_id = msgget(key_id, 0666 | IPC_CREAT);

    struct msgEntryBuff intialization;
    intialization.Algonum = chosenAlgo;
    intialization.numProccess = processCount;
    //   printf( " procces count in genrator %d " , processCount ) ;
    intialization.mtype = 1;

    int sendval = msgsnd(msg_id, &intialization, sizeof(intialization.Algonum) + sizeof(intialization.numProccess), !IPC_NOWAIT);

    if (sendval == -1)
        perror("error in sending from generator to scheduler ");

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    // TODO Initialization
    // 1. Read the input files.
    // 2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
    // 3. Initiate and create the scheduler and clock processes.
    // 4. Use this function after creating the clock process to initialize clock.
    initClk();
    // To get time use this function.
    int Count = 0;

    while (true)
    {

        int currentTime = getClk();

        struct processesBuff package;

        while (Processdata[Count].arrivalTime == currentTime)
        { //  prepare the package and fly it to scheduler

            printf("Current Time is %d\n", currentTime);

            package.mtype = 1; // any dummy
            Processdata[Count].validData=true;
            package.procs = Processdata[Count];

            int sendprocs = msgsnd(msg_id, &package, sizeof(package.procs), !IPC_NOWAIT);

            if (sendval == -1)
                perror("error in sending from generator to scheduler ");

            Count++;
        }
        
            package.mtype = 1; // any dummy
            Processdata[Count].validData=false;
            package.procs = Processdata[Count];

            int sendprocs = msgsnd(msg_id, &package, sizeof(package.procs), !IPC_NOWAIT);

            if (sendval == -1)
                perror("error in sending from generator to scheduler ");
        

        // communicaation with scheduler and sending to him the process which is ready for manipulation on it

        //prepare the package which are gonna send to the scheduler
    }

    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    destroyClk(true);
}

// void clearResources(int signum)
// {
//     //TODO Clears all resources in case of interruption
// }
