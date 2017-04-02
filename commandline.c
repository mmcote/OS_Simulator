#include "simulator.h"

void printTLB(TLBElement* head)
{
    TLBElement* cursor = head;
    while(cursor != NULL)
    {
        printf("Page Number: %d\n", cursor->pageNum);
        cursor = cursor->next;
    }
}

// This is used originally to create the list
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

    newNode->pageNum = pageNum;
    newNode->frameNum = frameNum;

    newNode->next = next;
 
    return newNode;
}

/* Append will be used for the TLB to add a newly loaded entry into the TLB
As for LRU, if this is just used once it will be the least
recently used.
As for FIFO, we can just grab from the head.
*/
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

/* removeFront will be used especially in the FIFO eviction policy as new
elements being inserted will just be added to the back and therefore the first
in will be those residing in the front.
*/
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

/* removeNode TLBElement from the anywhere in the TLB will be used especially 
in the LRU eviction policy as we will always be removing the least recently 
used which may be anywhere in the TLB.
*/
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

/* makeMostRecent will take a pageNum, find the node, and make it most recent.
In this case making the page the most recent involves bringing the element to 
back of the list.
*/
TLBElement* makeMostRecent(TLBElement* head, int pageNum)
{
    TLBElement* priorCursor = NULL;
    TLBElement* cursor = head;

    /* find the pageNum in the TLB */
    while (1)
    {
        if (cursor->pageNum == pageNum)
        {
            break;
        }

        /* If there has not yet been a match, if the next is null then it must 
        just not present
        */
        if (cursor->next == NULL)
        {
            return NULL;
        }

        // Set the prior cusor
        priorCursor = cursor;
        // Set the next cursor to be checked
        cursor = cursor->next;
    }

    // Must be the last element already
    if (cursor->next == NULL)
    {
        return head;
    }

    // Change the prior's next element to that of the element's next
    priorCursor->next = cursor->next;
    // Set the current element next to NULL
    cursor->next = NULL;

    TLBElement* lastCursor = priorCursor->next;
    // Get the last element of the current list
    while(1)
    {
        if (lastCursor->next == NULL)
        {
            break;
        }
        lastCursor = lastCursor->next;
    }

    // Set the last element to the given pageNum element
    lastCursor->next = cursor;

    return head;
}

TLBElement* lookupTLB(int pageNum, TLBElement* TLB)
{
    TLBElement* cursor = TLB;
    while(cursor->next != NULL)
    {
        if (*cursor->pageNum == pageNum)
        {
            return cursor;
        }
        cursor = cursor->next;
    }
    return NULL;
}

TLBElement* addToTLB(TLBElement* TLB, int pageNum, int frameNum)
{
    if (TLB == NULL)
    {
        return create(pageNum, frameNum, NULL);
    }
    return append(TLB, pageNum, frameNum);
}

void swapLRU()
{

}

void swapFIFO()
{

}

void takeIn()
{

}

void roundRobin(int quantum, int numTraceFiles)
{

}

void demoTLB()
{
    TLBElement* head = addToTLB(NULL, 10, 1);
    head = addToTLB(head, 9, 1);
    head = addToTLB(head, 8, 1);
    head = addToTLB(head, 7, 1);
    head = addToTLB(head, 6, 1);
    head = addToTLB(head, 5, 1);
    head = addToTLB(head, 4, 1);
    head = addToTLB(head, 3, 1);
    head = makeMostRecent(head, 5);
    head = addToTLB(head, 2, 1);
    head = addToTLB(head, 1, 1);
    head = makeMostRecent(head, 9);

    printTLB(head);
}

int main(int argc, char *argv[])
{

   if (argc < 8){
  	printf("tvm379 pgsize tlbentries { g | p } quantum physpages { f | l } trace1 trace2 . . . tracen\n");
  	exit(1);
   }

   int pgsize = atoi(argv[1]);
   int tbentries = atoi(argv[2]);
   char uniformity = argv[3];
   int quantum = atoi(argv[4]);
   int physpages = atoi(argv[5]);

    // Eviction Policy is now a global variable as it will influence the flow of the program
    evictionPolicy = argv[6];


   //command line arguments

   int test = pgsize;
   int test2 = tbentries;

  while (((test % 2) == 0) && test > 1)
    test /= 2;

  if ((test != 1) || (pgsize < 16) || (pgsize > 65536)){
    printf("pgsize must be a power of 2 and between the range of 16-65536");
    exit(1);        
  }

  while (((test2 % 2) == 0) && test2 > 1)
      test2 /= 2;

  if ((test2 != 1) || (tbentries < 8) || (tbentries > 256)){
    printf("tbentries must be a power of 2 and between the range of 8-256");
    exit(1);        
  }

  if (uniformity == 'g')
  {

  }
  else if (uniformity == 'p')
  {

  }
  else
  {
    printf("Usage: Please enter g (global) or p to simulate whether the TLB distinguish across processes");   
    exit(-1);
  }

  if (physpages > 1000000){
  	printf("physpages cannot be greater than 1000000\n");
  	exit(1);
  }

  if (evictionPolicy != 'f' && evictionPolicy != 'l')
  {
      printf("Usage: Please enter either f(FIFO) or l (LRU), to properly define a page eviction policy.");    
      exit(-1);
  }

  demoTLB();

  return 0;
}
