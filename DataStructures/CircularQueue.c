#include <stdlib.h>
#include<stdio.h>
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
  
    if (q->front == NULL) 
        q->front = node;
    else
        q->back->next = node;
        
  
    q->back=node;
    q->back->next = q->front;

}
  
void QueuePop(struct Queue* q)
{
    if (q->front == NULL){
        printf("INVALID POP\n");
        return;
    }
    q->size--;

    //only one node
    if(q->front == q->back){
        free(q->front);
        q->front = q->back = NULL;
    }
    else{
        struct Node* temp = q->front;
        q->front = q->front->next;
        q->back->next = q->front;
        free(temp);
    }
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

struct abbas{
    int val;
};
int main(){
    struct Queue* q = createQueue();

    struct abbas* a = (struct abbas*)malloc(sizeof(struct abbas));
    a->val = 5555;
    

    QueuePush(q,a);

    a = (struct abbas*)malloc(sizeof(struct abbas));
    a->val = 66666;

    QueuePush(q,a);


    a = (struct abbas*)malloc(sizeof(struct abbas));
    a->val = 7777;

    QueuePush(q,a);

    for(int i = 0 ; i < 5 ; i++){

    }


    printf("%d\n",((struct abbas*)QueueFront(q))->val);
    printf("%d\n",((struct abbas*)QueueBack(q))->val);
    printf("%d\n",QueueSize(q));
    QueuePop(q);
    printf("%d\n",((struct abbas*)QueueFront(q))->val);
    printf("%d\n",QueueSize(q));

    QueuePop(q);
    printf("%d\n",QueueSize(q));
    printf("%d\n",((struct abbas*)QueueFront(q))->val);
    printf("%d\n",((struct abbas*)QueueBack(q))->val);
    QueuePop(q);
    printf("%d\n",QueueSize(q));


}