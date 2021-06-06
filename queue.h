#include <stdlib.h>
struct Node {
    void* value;
    struct Node* next;
};
  
struct Queue {
    int size;
    struct Node *front, *back;
};
  
struct Node* createNode(void* val)
{
    struct Node* nNode = (struct Node*)malloc(sizeof(struct Node));
    nNode->value = val;
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
  
void QueuePush(struct Queue* q, void* val)
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

void* QueueFront(struct Queue* q){
    if(q->front == NULL) return NULL;
    return q->front->value;
}

void* QueueBack(struct Queue* q){
    if(q->back == NULL) return NULL;
    return q->back->value;
}

int QueueSize(struct Queue* q){
    return q->size;
}