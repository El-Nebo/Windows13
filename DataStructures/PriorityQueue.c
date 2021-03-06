#include "../headers.h"
#define MAXSIZE 50

struct Node {
    Process value;
    int priority;
    //int index;
}; typedef struct Node Node;

struct Priority_Queue {
    Node* NodeArray[50];
    int size;
};typedef struct Priority_Queue Priority_Queue;

Node* createPNode(Process* val,int p)
{
    Node* nNode = (Node*)malloc(sizeof(Node));
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

void Swap(Node* a, Node* b){
    Node tempval = *a;
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
 

void Priority_QueuePush(Priority_Queue* q, void* val, int p)
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
    void* result = q->NodeArray[0];
 
    q->size = q->size - 1;
    q->NodeArray[0] = q->NodeArray[q->size];
 
    shiftDown(q,0);
    free(result);
}
 
Node Priority_QueuePeek(Priority_Queue* q)
{
    if(q->size <= 0){
        printf("INVALID Peek\n");
        Node End;
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

struct abbas{
    int val;
};

Process p;
Priority_Queue* q;
void push(int a,int c){
    p.id = a;
    p.priority = c;
    Priority_QueuePush(q,&p,p.priority);
    printf("%d %d\n", Priority_QueuePeekValue(q).id,Priority_QueuePeekValue(q).priority);

}

void pop(){
    Priority_QueuePop(q);
    printf("%d %d\n", Priority_QueuePeekValue(q).id,Priority_QueuePeekValue(q).priority);
}

void s(){
    printf("%d\n",q->size);
}
int main(){
    q = create_Priority_Queue();
    //printf("%d %d %d\n", Priority_QueuePeekValue(q).id, Priority_QueuePeekValue(q).runTime,Priority_QueuePeekValue(q).priority);
    
    push(1,10);
    push(2,9);
    push(3,8);
    push(4,7);
    push(5,6);
    push(6,5);
    push(7,4);
    push(8,3);
    push(9,2);
    push(10,1);
    pop();
    pop();
    pop();
    pop();
    pop();
    pop();
    pop();
    pop();
    pop();
    //pop();
    
    
    return 0;
}