#pragma once

#include <stdio.h>
#include <stdlib.h>

struct node
{
    int start;//start of block
    int length;//length of block
    int process_id;
    struct node *next;
};
typedef struct node NODE;

struct linkedList
{
    NODE *head;
    int size;
};

struct nodesorted
{
    int start;
    struct nodesorted *next;
};
typedef struct nodesorted SNODE;


void InsertOrdered(struct linkedList *f, int start)
{
    SNODE *newnode;
    newnode = (SNODE *)malloc(sizeof(SNODE));
    newnode->start = start;

    if (f->size == 0)
    {
        f->head = newnode;
        f->head->next = NULL ;
        f->size++;
        return;
    }

    SNODE *head = f->head;
    SNODE *previous = NULL;
    SNODE *current = head;
    while (current != NULL && start > current->start)
    {
        previous = current;
        current = current->next;
    }

    if (previous == NULL)
    {
        f->head = newnode;
        newnode->next = current;
    }
    else
    {
        previous->next = newnode;
        newnode->next = current;
    }
    f->size++;
}


struct linkedList *creatLinkedList()
{
    struct linkedList *f = (struct linkedList *)malloc(sizeof(struct linkedList));
    f->head = NULL;
    f->size = 0;
    return f;
}

void Insert(struct linkedList *f,int process_id, int start,int len)
{
    NODE *newnode;
    newnode = (NODE *)malloc(sizeof(NODE));
    newnode->start = start;
    newnode->length = len;
    newnode->process_id=process_id;

    if (f->size == 0)
    {
        f->head = newnode;
        f->head->next = NULL;
        f->size++;
        return;
    }

    NODE *head = f->head;
    NODE *previous = NULL;
    NODE *current = head;
    while (current != NULL)
    {
        previous = current;
        current = current->next;
    }

    if (previous == NULL)
    {
        current->next = newnode;
        newnode->next = NULL;
    }
    else
    {
        previous->next = newnode;
        newnode->next = current;
    }
    f->size++;
}

void frontInsert(struct linkedList *f,int process_id, int start,int len)
{
    NODE *newnode;
    newnode = (NODE *)malloc(sizeof(NODE));
    newnode->start = start;
    newnode->length = len;
    newnode->process_id=process_id;
    
    newnode->next = f->head;
    f->head = newnode;
}

int DeleteNode(struct linkedList *f, int process_id)
{
    if (f->size == 0)
    {
        return 0;
    }
    NODE *head = f->head;
    NODE *previous = NULL;
    NODE *current = head;
    while (current != NULL && current->process_id !=process_id)
    {
        previous = current;
        current = current->next;
    }
    if (current != NULL)
    {
        if (current == head)
        {
            f->head = current->next;
        }
        else
        {
            previous->next = current->next;
        }
        free(current);
        f->size--;
        if (f->size == 0)
        {
            f->head = NULL;
        }
        return 1;
    }
    else
        return 0;
}
void VirtualDelete(struct linkedList *f, int process_id){
    if (f->size == 0)
    {
        return;
    }
    NODE *head = f->head;
    NODE *previous = NULL;
    NODE *current = head;
    while (current != NULL && current->process_id !=process_id)
    {
        previous = current;
        current = current->next;
    }
    if(current == head){
        if(current->next && current->next->process_id <= -1){
            f->head = current->next;
            f->head->start = 0;
            f->head->length += current->length;
            free(current);
        }
        current->process_id = -current->process_id;
    }
    else{
        if(previous->process_id <= -1){
            previous->next = current->next;
            previous->length += current->length;
            free(current);
            current = previous;
        }
        if(current->next != NULL && current->next->process_id <= -1){
            NODE* temp = current->next;
            current->next = temp->next;
            current->length += temp->length;
            free(temp);
        }
        if(previous->process_id >= 0 && (current->next == NULL||current->next->process_id >= 0)){
            current->process_id = -current->process_id;
        }
    }
    if (f->size == 0)
    {
        f->head = NULL;
    }
}
void Traverse(struct linkedList *f)
{
    NODE *head = f->head;
    // printf("head data : %d", head->data);
    NODE *current = head;
    while (current != NULL)
    {
        printf(" (%d) - (%d) - (%d) " , current->process_id,current->start,current->length);
        current = current->next;
    }
}

int findNode(struct linkedList *f, int process_id)
{
    if (f->size == 0)
    {
        return 0;
    }
    NODE *head = f->head;
    NODE *previous = NULL;
    NODE *current = head;
    while (current != NULL && current->process_id != process_id
            )
    {
        previous = current;
        current = current->next;
    }
    if (current != NULL)
        return 1;
    else
        return 0;
}

int isempty(struct linkedList *f)
{
    return (f->size == 0);
}
//--------------------------------------------------------
NODE *getPrev(struct linkedList *f, NODE *target)
{
    NODE *current = f->head;
    if (f->size == 1 || f->size == 0)
    {
        return NULL;
    }
    else
    {
        while (current != NULL && current->next != target)
        {
            current = current->next;
        }
        return current;
    }
}
void InsertNextTo(struct linkedList *f, NODE *prev, int process_id, int start, int len)
{
    NODE *newnode;
    newnode = (NODE *)malloc(sizeof(NODE));
    newnode->start = start;
    newnode->length = len;
    newnode->process_id = process_id;

    if (f->size == 0)
    {
        f->head = newnode;
        f->head->next = NULL;
        f->size++;
        return;
    }

    NODE *head = f->head;
    NODE *previous = NULL;
    NODE *current = head;
    while (current != NULL && current != prev->next)
    {
        previous = current;
        current = current->next;
    }

    if (previous == NULL)
    {
        f->head = newnode;
        newnode->next = current;
    }
    else
    {
        previous->next = newnode;
        newnode->next = current;
    }
    f->size++;
}

int DeletWithPos(struct linkedList *f, int pos)
{
    if (f->size == 0)
    {
        return 0;
    }
    NODE *head = f->head;
    NODE *previous = NULL;
    NODE *current = head;
    while (current != NULL && current->start != pos)
    {
        previous = current;
        current = current->next;
    }
    if (current != NULL)
    {
        if (current == head)
        {
            f->head = current->next;
        }
        else
        {
            previous->next = current->next;
        }
        free(current);
        f->size--;
        if (f->size == 0)
        {
            f->head = NULL;
        }
        return 1;
    }
    else
        return 0;
}