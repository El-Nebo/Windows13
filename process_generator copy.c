#include "headers.h"
void clearResources(int sig);

int PG_SCH_MsgQ;
int EXIT = 0;
int schedID;
int memPolicy;

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);

    FILE *Process_Data;

    if (argc < 3)
    {
        printf(" unsufficient Data please provide the file path and chosen algo");
        exit(-1);
    }

    Process Processdata[100];// = malloc(sizeof(struct processData*) * 100); // 100 formemmory size can be changed later

    Process_Data = fopen(argv[1], "r");
    int processCount = 0;
    Process temp;

    ///////////////////////////////////////////////////////////////////
    char input[200];
    while(fgets(input,200,Process_Data)){
        if(input[0] == '#') continue;
        char numtemp[5][10];
        int k = 0,z=0;
        for(int i = 0 ; i < strlen(input); i++){
            if(input[i] != '\t') numtemp[k][z++] = input[i];
            else{
                numtemp[k][z] = '\0';
                z = 0;
                k++;
            }
        }
        numtemp[4][z] = '\0';
        Processdata[processCount].id = atoi(numtemp[0]);
        Processdata[processCount].arrivalTime= atoi(numtemp[1]);
        Processdata[processCount].runTime= atoi(numtemp[2]);
        Processdata[processCount].priority= atoi(numtemp[3]);
        Processdata[processCount].IsProcess = 1;
        Processdata[processCount].remainingTime = Processdata[processCount].runTime;
        Processdata[processCount].MemorySize = atoi(numtemp[4]);

        printf("%d\t", Processdata[processCount].id);
        printf("%d\t", Processdata[processCount].arrivalTime);
        printf("%d\t", Processdata[processCount].runTime);
        printf("%d\t", Processdata[processCount].priority);
        printf("%d\n", Processdata[processCount].MemorySize);

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
    //------------------------------------------

    memPolicy = atoi(argv[4])-1;
    char MEMPOLICIES[4][50] = {
        "First Fit",
        "Next Fit",
        "Best Fit",
        "Buddy :("
    };
    printf("---Chosen memory policy is: %s\n",MEMPOLICIES[memPolicy]);
    //________________________________TODO: Read other parameters
    char args[3][4];
    sprintf(args[0], "%d", chosenAlgo);
    sprintf(args[1], "%d", processCount);
    sprintf(args[2], "%d", memPolicy);
    //strcpy(args[1],"0\0");
    //args[1] = "0\n";
    /////////////////////////////////////////////////////////////////////////////////
    // creating clk and scheduler process and start execute them

    int Clk_ID = fork();
    if (Clk_ID == 0){
        system("gcc clk.c -o clk.out");
        execl("clk.out","clk",NULL);
    }
    schedID = fork();
    if (schedID == 0){
        system("gcc scheduler.c -o scheduler.out -lm");
        execl("scheduler.out", "scheduler", args[0],argv[3],args[1],args[2]);
        //printf("Abbas\n");
    }
        

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    PG_SCH_MsgQ = initMsgQueue(66);

    initClk();
    printf("clock: %d\n", getClk());
    int Count = 0;
    int prevClk = -1;
    while (!EXIT && Count < processCount)
    {
        while(getClk() == prevClk){}
        int currentTime = getClk();
        //printf("Process Generator: Current Time is %d\n", currentTime);

        while(Count < processCount && Processdata[Count].arrivalTime == currentTime){
            sendMessage(PG_SCH_MsgQ,Processdata[Count]);
            Count++;
        }
        Process End_Connection;
        End_Connection.IsProcess = 0;
        sendMessage(PG_SCH_MsgQ,End_Connection);

        prevClk = currentTime;
    }
    while(!EXIT);
    destroyClk(true);
}



void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    msgctl(PG_SCH_MsgQ, IPC_RMID, (struct msqid_ds *)0);
    destroyClk(true);
    EXIT = 1;
    signal(SIGINT, SIG_DFL);
    kill(schedID,SIGINT);
}

