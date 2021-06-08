#pragma once
#include "../headers.h"
#define MAXSIZE 50

struct PNode {
    Process value;
    int priority;
    //int index;
}; typedef struct PNode PNode;

struct Priority_Queue {
    PNode* NodeArray[50];
    int size;
};typedef struct Priority_Queue Priority_Queue;

PNode* createPNode(Process* val,int p)
{
    PNode* nNode = (PNode*)malloc(sizeof(PNode));
    nNode->value = *val;
    nNode->priority = p;
    return nNode;
}

Priority_Queue* create_Priority_Queue()
{
    Priority_Queue* q = (Priority_Queue*)malloc(sizeof(Priority_Queue));
    q->size = 0;
    return q;
}

void Destroy_Priority_Queue(Priority_Queue* q){
    for(int i = 0 ; i < q->size ; i++)
        free(q->NodeArray[i]);
    free(q);
}

int parent(int i){
    return (i - 1) / 2;
}
int leftChild(int i){
    return ((2 * i) + 1);
}
int rightChild(int i){
    return ((2 * i) + 2);
}

void Swap(PNode* a, PNode* b){
    PNode tempval = *a;
    *a = *b;
    *b = tempval;
}

void shiftUp(Priority_Queue* q, int i)
{
    while (i > 0 && q->NodeArray[parent(i)]->priority > q->NodeArray[i]->priority) {
        Swap(q->NodeArray[parent(i)], q->NodeArray[i]);
        i = parent(i);
    }
}
 
void shiftDown(Priority_Queue* q, int i)
{
    int maxIndex = i;
 
    int l = leftChild(i);
 
    if (l < q->size && q->NodeArray[l]->priority < q->NodeArray[maxIndex]->priority) {
        maxIndex = l;
    }
 
    int r = rightChild(i);
 
    if (r < q->size && q->NodeArray[r]->priority < q->NodeArray[maxIndex]->priority) {
        maxIndex = r;
    }
 
    if (i != maxIndex) {
        Swap(q->NodeArray[i], q->NodeArray[maxIndex]);
        shiftDown(q,maxIndex);
    }
}
 

void Priority_QueuePush(Priority_Queue* q, Process* val, int p)
{
    q->NodeArray[q->size] = createPNode(val,p);
    
    //q->NodeArray[q->size]->index = q->size;

    shiftUp(q,q->size);

    q->size = q->size + 1;
 
}
 

void Priority_QueuePop(Priority_Queue* q)
{
    if(q->size == 0){
        printf("INVALID POP\n");
        return;
    }
    PNode* result = q->NodeArray[0];
 
    q->size = q->size - 1;
    q->NodeArray[0] = q->NodeArray[q->size];
 
    shiftDown(q,0);
    free(result);
}
 
PNode Priority_QueuePeek(Priority_Queue* q)
{
    if(q->size <= 0){
        printf("INVALID Peek\n");
        PNode End;
        return End;
    }
    return *q->NodeArray[0];
}

Process Priority_QueuePeekValue(Priority_Queue* q){
    return Priority_QueuePeek(q).value;
}

int Priority_QueueSize(Priority_Queue* q){
    return q->size;
}