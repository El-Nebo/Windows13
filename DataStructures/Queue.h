#pragma once
#include <stdlib.h>
#include <stdio.h>
#include "../headers.h"

struct Node {
    Process value;
    struct Node* next;
};
typedef struct Node Node;

struct Queue {
    int size;
    struct Node *front, *back;
};
typedef struct Queue Queue;
  
struct Node* createNode(Process* val)
{
    struct Node* nNode = (struct Node*)malloc(sizeof(struct Node));
    nNode->value = *val;
    nNode->next = NULL;
    return nNode;
}
  
struct Queue* createQueue()
{
    struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue));
    q->front = q->back = NULL;
    q->size = 0;
    return q;
}
  
void QueuePush(struct Queue* q, Process* val)
{
    struct Node* node = createNode(val);
    q->size++;
  
    if (q->back == NULL) {
        q->front = q->back = node;
        return;
    }
  
    q->back->next = node;
    q->back = node;
}
  
int QueuePop(struct Queue* q)
{
    if (q->front == NULL)
        return -1;
    
    q->size--;
    struct Node* temp = q->front;
  
    q->front = q->front->next;
  
    if (q->front == NULL)
        q->back = NULL;
  
    free(temp);
    return 0;
}

Process QueueFront(struct Queue* q){
    if(q->front == NULL){
        Process End;
        End.IsProcess = 0;
        return End;
    } 
    return q->front->value;
}

Process QueueBack(struct Queue* q){
    if(q->back == NULL){
        Process End;
        End.IsProcess = 0;
        return End;
    } 
    return q->back->value;
}

int QueueSize(struct Queue* q){
    return q->size;
}

// int main(){
//     struct Queue* q = createQueue();
    
//     Process p;
//     p.id = 55;
//     p.runTime = 12;
//     QueuePush(q,&p);

//     p.id = 66;
//     p.runTime = 13;
//     QueuePush(q,&p);

//     p.id = 77;
//     p.runTime = 14;
//     QueuePush(q,&p);

//     printf("%d %d \n",QueueFront(q).id , QueueFront(q).runTime);
//     printf("%d %d \n",QueueBack(q).id , QueueBack(q).runTime);


//     QueuePop(q);
//     printf("%d %d \n",QueueFront(q).id , QueueFront(q).runTime);
//     printf("%d %d \n",QueueBack(q).id , QueueBack(q).runTime);

//     QueuePop(q);
//     printf("%d %d \n",QueueFront(q).id , QueueFront(q).runTime);
//     printf("%d %d \n",QueueBack(q).id , QueueBack(q).runTime);

// }