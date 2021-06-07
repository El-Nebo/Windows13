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

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption

    kill(getppid(),SIGKILL);
}


int main(int argc, char *argv[])
{
    //signal(SIGINT, clearResources);

    FILE *Process_Data;

    if (argc < 3)
    {
        printf(" unsufficient Data please provide the file path and chosen algo");
        exit(-1);
    }

    char dummy[100];

    struct processData* Processdata = malloc(sizeof(struct processData*) * 100); // 100 formemmory size can be changed later

    Process_Data = fopen(argv[1], "r");
    int processCount = 0;
    struct processData temp;

    ///////////////////////////////////////////////////////////////////
    char input[200];
    while(fgets(input,200,Process_Data)){
        if(input[0] == '#') continue;
        char numtemp[4][10];
        int k = 0,z=0;
        for(int i = 0 ; i < strlen(input); i++){
            if(input[i] != '\t') numtemp[k][z++] = input[i];
            else{
                numtemp[k][z] = '\0';
                z = 0;
                k++;
            }
        }
        numtemp[3][z] = '\0';
        Processdata[processCount].id = atoi(numtemp[0]);
        Processdata[processCount].arrivalTime= atoi(numtemp[1]);
        Processdata[processCount].runTime= atoi(numtemp[2]);
        Processdata[processCount].priority= atoi(numtemp[3]);

        printf("%d\t", Processdata[processCount].id);
        printf("%d\t", Processdata[processCount].arrivalTime);
        printf("%d\t", Processdata[processCount].runTime);
        printf("%d\n", Processdata[processCount].priority);

        processCount++;
    }
    char Algoschoic[5][50] = {
        "First Come First Serve = 0",
        "Shortest Job First",
        "Preemtive Highest priorty  first",
        "Shortest Remaining Time Next",
        "Round Robin"
    };
    int chosenAlgo = atoi(argv[2])-1;
    printf("---Chosen Algo is: %s\n",Algoschoic[chosenAlgo]);

    //________________________________TODO: Read other parameters


    /////////////////////////////////////////////////////////////////////////////////
    // creating clk and scheduler process and start execute them
    int Clk_ID = fork();
    if (Clk_ID == -1){
        printf(" \n Error in forking clock  \n");
    }

    else if (Clk_ID == 0){
        printf("\n fork clock successfull\n");
        execl("clk.out", "clk", NULL);
    }

    int Scheduler_ID = fork();
    if (Scheduler_ID == -1)
        printf("\n error in forking schedular\n");

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
    intialization.mtype = 1;

    int sendval = msgsnd(msg_id, &intialization, sizeof(intialization.Algonum) + sizeof(intialization.numProccess), !IPC_NOWAIT);

    if (sendval == -1)
        perror("error in sending from generator to scheduler ");

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    initClk();
    printf("clock: %d\n", getClk());
    int Count = 0;
    int prevClk = 0;
    while (true)
    {
        while(getClk() == prevClk){}
        int currentTime = getClk();
        printf("PG: Current Time is %d\n", currentTime);

        struct processesBuff package;

        while(Processdata[Count].arrivalTime == currentTime){
            package.IsProcess = 1;
            package.procs = Processdata[Count];

            int sendprocs = msgsnd(msg_id, &package, sizeof(package.procs)+sizeof(package.IsProcess), !IPC_NOWAIT);
            if (sendval == -1)
                perror("error in sending from generator to scheduler ");

            Count++;
        }

        package.IsProcess = -1;
        int sendprocs = msgsnd(msg_id, &package, sizeof(package.procs)+sizeof(package.IsProcess), !IPC_NOWAIT);
        if (sendval == -1)
            perror("error in sending from generator to scheduler ");

        // int tempCount = 0;
        // package.mtype = 1; // any dummy
        // while (Processdata[Count+tempCount].arrivalTime == currentTime) tempCount++;
        // package.NumProcsinClk = tempCount;

        // package.procs = (struct processData*)malloc(sizeof(struct processData)*tempCount);

        // tempCount = 0;

        // while (Processdata[Count].arrivalTime == currentTime)//  prepare the package and fly it to scheduler
        //     package.procs[tempCount++] = Processdata[Count++];


        // int sendprocs = msgsnd(msg_id, &package, sizeof(package.procs)+sizeof(package.NumProcsinClk), !IPC_NOWAIT);

        // if (sendval == -1)
        //     perror("error in sending from generator to scheduler ");


        // package.mtype = 1; // any dummy
        // //Processdata[Count].validData=false;
        // package.procs = Processdata[Count];

        // int sendprocs = msgsnd(msg_id, &package, sizeof(package.procs), !IPC_NOWAIT);

        // if (sendval == -1)
        //     perror("error in sending from generator to scheduler ");
        

        // communicaation with scheduler and sending to him the process which is ready for manipulation on it

        //prepare the package which are gonna send to the scheduler

        prevClk = currentTime;
    }

    destroyClk(true);
}


