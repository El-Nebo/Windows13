#include <stdlib.h>
#include <stdio.h>
#define MAXSIZE 50

struct Node {
    void* value;
    int priority;
};

struct Queue {
    struct Node* NodeArray[50];

    int size;
};

struct Node* createNode(void* val,int p)
{
    struct Node* nNode = (struct Node*)malloc(sizeof(struct Node));
    nNode->value = val;
    nNode->priority = p;
    return nNode;
}

struct Queue* createQueue()
{
    struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue));
    q->size = 0;
    return q;
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

void Swap(struct Node* a, struct Node* b){
    void* tempval = a->value;
    int tempp = a->priority;
    a->value = b->value;
    a->priority = b->priority;
    b->value = tempval;
    b->priority=tempp;
   // a = b;
    //b = temp;
}
void shiftUp(struct Queue* q, int i)
{
    while (i > 0 && q->NodeArray[parent(i)]->priority > q->NodeArray[i]->priority) {
 
        Swap(q->NodeArray[parent(i)], q->NodeArray[i]);
 
        i = parent(i);
    }
}
 
void shiftDown(struct Queue* q, int i)
{
    int maxIndex = i;
 
    int l = leftChild(i);
 
    if (l <= q->size && q->NodeArray[l]->priority < q->NodeArray[maxIndex]->priority) {
        maxIndex = l;
    }
 
    int r = rightChild(i);
 
    if (r <= q->size && q->NodeArray[r]->priority < q->NodeArray[maxIndex]->priority) {
        maxIndex = r;
    }
 
    if (i != maxIndex) {
        Swap(q->NodeArray[i], q->NodeArray[maxIndex]);
        shiftDown(q,maxIndex);
    }
}
 

void QueuePush(struct Queue* q, void* val, int p)
{
    q->NodeArray[q->size] = createNode(val,p);
    // q->NodeArray[q->size]->value = val;
    // q->NodeArray[q->size]->priority = p;
    //q->H[q->size] = q->numNodes;
    shiftUp(q,q->size);

    q->size = q->size + 1;
 
}
 

void QueuePop(struct Queue* q)
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
 
struct Node* QueuePeek(struct Queue* q)
{
    return q->NodeArray[0];
}

void* QueuePeekValue(struct Queue* q){
    return QueuePeek(q)->value;
}

struct abbas{
    int val;
};

int main(){
    struct Queue* q = createQueue();

    struct abbas* a= (struct abbas*)malloc(sizeof(struct abbas));
    a->val = 55;
    QueuePush(q,a,2);

    a= (struct abbas*)malloc(sizeof(struct abbas));
    a->val = 66;
    QueuePush(q,a,3);

    a= (struct abbas*)malloc(sizeof(struct abbas));
    a->val = 77;
    QueuePush(q,a,1);

    struct Node* n = (struct Node*)QueuePeek(q);
    printf("%d %d \n",((struct abbas*)n->value)->val,n->priority);
    
    QueuePop(q);


    a= (struct abbas*)malloc(sizeof(struct abbas));
    a->val = 1;
    QueuePush(q,a,0);

    n = (struct Node*)QueuePeek(q);
    printf("%d %d \n",((struct abbas*)n->value)->val,n->priority);
    
    QueuePop(q);

    n = (struct Node*)QueuePeek(q);
    printf("%d %d \n",((struct abbas*)n->value)->val,n->priority);
    
    QueuePop(q);

     n = (struct Node*)QueuePeek(q);
     printf("%d %d \n",((struct abbas*)n->value)->val,n->priority);

    QueuePop(q);
    return 0;
}