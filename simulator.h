#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int * pageNum;
    int * frameNum;
    struct TLBElement* next;
} TLBElement;

TLBElement* create(int pageNum, int frameNum, TLBElement* next)
{
    TLBElement* newNode = (TLBElement*)malloc(sizeof(TLBElement));
    if(newNode == NULL)
    {
        printf("Error creating a new node.\n");
        exit(0);
    }

    newNode->pageNum = calloc(1, sizeof(int));
    newNode->frameNum = calloc(1, sizeof(int));

    *newNode->pageNum = pageNum;
    *newNode->frameNum = frameNum;

    newNode->next = next;
 
    return newNode;
}

TLBElement* append(TLBElement* head, int pageNum, int frameNum)
{
    if(head == NULL)
    {        
        return NULL;
    }

    /* go to the last TLBElement */
    TLBElement *cursor = head;
    while(cursor->next != NULL)
    {
        cursor = cursor->next;
    }

    /* create a new TLBElement */
    TLBElement* newNode =  create(pageNum, frameNum, NULL);
    cursor->next = newNode;

    return head;
}

TLBElement* removeFront(TLBElement* head)
{
    if(head == NULL)
    {
        return NULL;
    }

    TLBElement *front = head;
    head = head->next;
    front->next = NULL;

    /* is this the last TLBElement in the list */
    if(front == head)
    {
        head = NULL;
    }

    free(front->pageNum);
    free(front->frameNum);
    free(front);
    return head;
}

/*
    remove TLBElement from the back of the list
*/
TLBElement* removeBack(TLBElement* head)
{
    if(head == NULL)
    {
        return NULL;
    }

    TLBElement *cursor = head;
    TLBElement *back = NULL;
    while(cursor->next != NULL)
    {
        back = cursor;
        cursor = cursor->next;
    }

    if(back != NULL)
    {
        back->next = NULL;
    }

    /* if this is the last TLBElement in the list*/
    if(cursor == head)
    {
        head = NULL;
    }

    free(cursor->pageNum);
    free(cursor->frameNum);
    free(cursor);

    return head;
}

TLBElement* removeNode(TLBElement* head,TLBElement* nd)
{
    if(nd == NULL)
    {
        return NULL;
    }

    /* if the TLBElement is the first TLBElement */
    if(nd == head)
    {
        return removeFront(head);
    }

    /* if the TLBElement is the last TLBElement */
    if(nd->next == NULL)
    {
        return removeBack(head);
    }

    /* if the TLBElement is in the middle */
    TLBElement* cursor = head;
    while(cursor != NULL)
    {
        if(cursor->next == nd)
        {
            break;
        }
        cursor = cursor->next;
    }

    if(cursor != NULL)
    {
        TLBElement* tmp = cursor->next;
        cursor->next = tmp->next;
        tmp->next = NULL;
        free(tmp);
    }
    return head;

}