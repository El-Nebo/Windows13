#include "headers.h"

int main(int argc, char *argv[])
{
    initClk();

    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.



key_t key_id = ftok ( "key" ,66 ) ; 
int msg_id = msgget(key_id , 0666|IPC_CREAT) ; 

 struct msgEntryBuff intializationrec ;

int NumProcesses = -1 ; 
int choseAlgo = -1 ; 
int recievedProcess = 0 ; 


int rec = msgrcv(msg_id , &intializationrec , sizeof(intializationrec.Algonum)+sizeof(intializationrec.numProccess) , 0 ,!IPC_NOWAIT) ; 


    if ( rec == -1 )
    perror("cannot recieve from the genrator ") ;  


    else {

    printf("Recieved from the genrator \n"); 
    printf ( "Num of proccess is recieved %d \n " , intializationrec.numProccess ) ;
    printf(" chosen Algorithm is %d \n "  ,intializationrec.Algonum ) ; 
      
    NumProcesses = intializationrec.numProccess ;
    choseAlgo = intializationrec.Algonum  ;

    }



struct processData recievedProcs  ;  

struct processesBuff recPack  ;

   while(  true ) 
    {  
        int recievedprocs = msgrcv(msg_id ,  &recPack , sizeof(recPack.procs ),0, !IPC_NOWAIT )    ; 
        
        if (recievedprocs == -1 )
        perror("error in recieveng from generator to scheduler ") ; 

    else{
       recievedProcs = recPack.procs ; 

       printf( "process Arrival Time %d \n " ,  recievedProcs.arrivalTime ) ;  
       printf( "process running Time %d \n " ,  recievedProcs.runTime ) ;  
       printf( "process priority %d \n " ,  recievedProcs.priority ) ;  
       printf( "process id  %d \n " ,  recievedProcs.id ) ;  
    }


    }



  //  destroyClk(true);
}
