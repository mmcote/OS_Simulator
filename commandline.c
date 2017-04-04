#include "simulator.h"

void printTLB(TLBElement* head)
{
    TLBElement* cursor = head;
    while(cursor != NULL)
    {
        printf("Page Number: %d\n", *cursor->pageNum);
        cursor = cursor->next;
    }
}

void printTLBBackwards(TLBElement* head)
{
    TLBElement* cursor = head;
    while(cursor != NULL)
    {
        if (cursor->next == NULL)
        {
            break;
        }
        cursor = cursor->next;
    }
    while(cursor != NULL)
    {
        printf("Page Number: %d\n", *cursor->pageNum);
        cursor = cursor->prev;
    }
}

// This is used originally to create the list
TLBElement* create(int pageNum, int frameNum, int PID, TLBElement* prev, TLBElement* next)
{
    TLBElement* newNode = (TLBElement*)malloc(sizeof(TLBElement));
    if(newNode == NULL)
    {
        printf("Error creating a new node.\n");
        exit(0);
    }

    newNode->pageNum = calloc(1, sizeof(int));
    newNode->frameNum = calloc(1, sizeof(int));
    newNode->PID = calloc(1, sizeof(int));
    newNode->valid = calloc(1, sizeof(int));

    *newNode->pageNum = pageNum;
    *newNode->frameNum = frameNum;
    *newNode->PID = PID;
    *newNode->valid = 1;

    newNode->prev = prev;
    newNode->next = next;
 
    return newNode;
}

/* Append will be used for the TLB to add a newly loaded entry into the TLB
As for LRU, if this is just used once it will be the least
recently used.
As for FIFO, we can just grab from the head.
*/
TLBElement* append(TLBElement* head, int pageNum, int frameNum, int PID)
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
    TLBElement* newNode =  create(pageNum, frameNum, PID, cursor, NULL);
    cursor->next = newNode;

    return head;
}

/* removeFront will be used especially in the FIFO eviction policy as new
elements being inserted will just be added to the back and therefore the first
in will be those residing in the front.
*/
TLBElement* removeFront(TLBElement* head)
{
    // If the head is already NULL
    if(head == NULL)
    {
        return NULL;
    }

    // Check if the head is the only element
    TLBElement *front = head;
    
    // Change the new head's previous to NULL
    head = head->next;
    if (head != NULL)
    {
        head->prev = NULL;
    }
    front->next = NULL;
    
    if(front == head)
    {
        head = NULL;
    }

    free(front->pageNum);
    free(front->frameNum);
    free(front->PID);
    free(front->valid);
    free(front);
    return head;
}

TLBElement* removeBack(TLBElement* head)
{
    // If the head is already NULL
    if(head == NULL)
    {
        return NULL;
    }

    // Get the last element of the linked list
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

    // if this is the last TLBElement in the list
    if(cursor == head)
    {
        head = NULL;
    }

    free(cursor->pageNum);
    free(cursor->frameNum);
    free(cursor->PID);
    free(cursor->valid);
    free(cursor);

    return head;
}

/* removeNode TLBElement from the anywhere in the TLB will be used especially 
in the LRU eviction policy as we will always be removing the least recently 
used which may be anywhere in the TLB.
*/
TLBElement* removeNode(TLBElement* head,TLBElement* nd)
{
    // If the TLBElement passed was NULL
    if(nd == NULL)
    {
        return NULL;
    }

    // if the TLBElement is the first TLBElement
    if(nd == head)
    {
        return removeFront(head);
    }

    // if the TLBElement is the last TLBElement
    if(nd->next == NULL)
    {
        return removeBack(head);
    }

    // if the TLBElement is in the middle
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
        cursor->prev = tmp->prev;
        tmp->prev = NULL;
        tmp->next = NULL;
        free(tmp->pageNum);
        free(tmp->frameNum);
        free(tmp->PID);
        free(tmp->valid);
        free(tmp);
    }
    return head;
}

/* makeMostRecent will take a pageNum, find the node, and make it most recent.
In this case making the page the most recent involves bringing the element to 
back of the list.
*/
TLBElement* makeMostRecent(TLBElement* head, int pageNum, int PID)
{
    TLBElement* priorCursor = NULL;
    TLBElement* cursor = head;

    /* find the pageNum in the TLB */
    while (1)
    {
        if (*cursor->pageNum == pageNum && *cursor->PID == PID)
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
    if (priorCursor != NULL)
    {
        priorCursor->next = cursor->next;
        cursor->next->prev = priorCursor;
    }
    else 
    {
        // The head must be the node being made most recent
        head = cursor->next;
    }

    TLBElement* lastCursor = cursor->next;

    // Get the last element of the current list
    while(1)
    {
        if (lastCursor->next == NULL)
        {
            break;
        }
        lastCursor = lastCursor->next;
    }

    // Set the current element next to NULL
    cursor->next = NULL;

    // Set the last element to the given pageNum element
    lastCursor->next = cursor;
    cursor->prev = lastCursor;
    return head;
}

TLBElement* lookupTLB(int pageNum, int PID, TLBElement* TLB)
{
    TLBElement* cursor = TLB;
    while(cursor != NULL)
    {
        if (*cursor->pageNum == pageNum && *cursor->PID == PID)
        {
            return cursor;
        }
        cursor = cursor->next;
    }
    return NULL;
}

TLBElement* TLBHitOrMiss(TLBElement* TLB, int pageNum, int PID)
{
    TLBElement* previouslyMade = lookupTLB(pageNum, PID, TLB);
    if (previouslyMade != NULL)
    {
        return TLB;
    }
    return NULL;
}

TLBElement* addToTLB(TLBElement* TLB, int pageNum, int frameNum, int PID)
{
    // First check if the current element is in the TLB
    TLBElement* previouslyMade = lookupTLB(pageNum, PID, TLB);
    if (previouslyMade != NULL)
    {
        return TLB;
    }

    // If the max size has been reached then must evict
    if (currentTLBSize == maxTLBSize)
    {
        // Remove an element from the TLB
        if (evictionPolicy == 'f')
        {
            
        }
        else
        {
            TLB = removeFront(TLB);
            currentTLBSize--;
        }
    }

    currentTLBSize++;
    if (TLB == NULL)
    {
        return create(pageNum, frameNum, PID, NULL, NULL);
    }
    return append(TLB, pageNum, frameNum, PID);
}

void processQuantum(unsigned char* buffer, int quantum, int PID)
{
    unsigned char* currentLocation = buffer;
    unsigned char* miniBuffer = calloc(5, sizeof(char));
    
    int pageNum;

    int hits = 0;
    int checks = 0;
    // Process whole quantum
    // TODO: change limit back to quantum
    // for (int i = 0; i < quantum; ++i)
    for (int i = 0; i < quantum; ++i)
    {
        pageNum = 0;
        for (int j = 0; j < 4; ++j)
        {
            miniBuffer[j] = *currentLocation;
            currentLocation++;

            for (int k = 0; k < 8; ++k) {
                // printf("%d", !!((miniBuffer[j] << k) & 0x80));
                if ( (!!((miniBuffer[j] << k) & 0x80)) == 1)
                {
                    pageNum |= 1 << ((32 - j*8 - 1) - k);
                }
            }

            // for (int k = 0; k < 8; ++k) {
            //     printf("%d", !!((miniBuffer[j] << k) & 0x80));
            // }
        }
        
        // printf("pageNum: %d\n", pageNum);
        // printf("\n");
        // printf("pageNum: %d\n", pageNum);

        TLBElement* hit = TLBHitOrMiss(TLB, pageNum, PID);
        checks++;
        /* If there is a TLB hit then continue, although before continuing:
            - If the eviction policy is LRU --> make the TLBElement the most recent one
        */
        if (hit != NULL)
        {
            hits++;
            // printf("Hit for %d\n", pageNum);
            // printTLB(TLB);
            TLB = makeMostRecent(TLB, pageNum, PID);
            continue;
        }
        else
        {
            TLB = addToTLB(TLB, pageNum, 1, PID);
        }
    }
    printf("Hit/Checks %d/%d\n", hits, checks);
}

void demoTLB()
{
    TLBElement* head = addToTLB(NULL, 10, 1, 1);
    head = addToTLB(head, 9, 1, 1);
    head = addToTLB(head, 8, 1, 1);
    head = addToTLB(head, 7, 1, 1);
    head = addToTLB(head, 6, 1, 1);
    head = addToTLB(head, 5, 1, 1);
    head = addToTLB(head, 4, 1, 1);
    head = addToTLB(head, 3, 1, 1);
    head = makeMostRecent(head, 5, 1);
    head = addToTLB(head, 2, 1, 1);
    head = makeMostRecent(head, 9, 1);
    head = addToTLB(head, 1, 1, 1);

    printTLB(head);
    printf("-------------------\n");
    printTLBBackwards(head);
}

int main(int argc, char **argv)
{
    // #Checks
    {
        if (argc < 8){
            printf("tvm379 pgsize tlbentries { g | p } quantum physpages { f | l } trace1 trace2 . . . tracen\n");
            exit(1);
        }

        /* If this was inputted correctly only the first 7 are not trace files,
        the rest of the arguments are trace files
        */
        int numTraceFiles = argc - 7;
        int pgsize = atoi(argv[1]);
        maxTLBSize = atoi(argv[2]);
        char* uniformity = argv[3];
        int quantum = atoi(argv[4]);
        int physpages = atoi(argv[5]);

        /* Eviction Policy is now a global variable as it will influence the flow 
        of the program
        */
        evictionPolicy = calloc(2, sizeof(char));
        evictionPolicy = argv[6];

        //command line arguments, powers of 2
        int test = pgsize;
        int test2 = maxTLBSize;

        while (((test % 2) == 0) && test > 1)
            test /= 2;

        if ((test != 1) || (pgsize < 16) || (pgsize > 65536))
        {
            printf("pgsize must be a power of 2 and between the range of 16-65536");
            exit(1);        
        }

        while (((test2 % 2) == 0) && test2 > 1)
            test2 /= 2;

        if ((test2 != 1) || (maxTLBSize < 8) || (maxTLBSize > 256))
        {
            printf("tlbentries must be a power of 2 and between the range of 8-256");
            exit(1);        
        }

        if (*uniformity != 'g' && *uniformity != 'p')
        {
            printf("Usage: Please enter g (global) or p to simulate whether the TLB distinguish across processes");   
            exit(-1);
        }

        if (quantum <= 0)
        {
            printf("Usage: Quantum must be greater than zero.");		
            exit(-1);
        }

        if (physpages > 1000000){
            printf("physpages cannot be greater than 1000000\n");
            exit(1);
        }

        if (*evictionPolicy != 'f' && *evictionPolicy != 'l')
        {
            printf("Usage: Please enter either f(FIFO) or l (LRU), to properly define a page eviction policy.");    
            exit(-1);
        }

        FILE* fp[numTraceFiles];
        off_t offset[numTraceFiles];
        char* fileName;
        for (int i = 0; i < numTraceFiles; ++i)
        {
            fp[i] = NULL;
            fileName = argv[i + 7];
            fp[i] = fopen(fileName, "rb");
        }
    }

    // Initialize the TLB
    TLB = NULL;

    // Initialize the Virtual Memory
    VM = NULL;

    // Initialize the Page Tables for all processes

    unsigned char* buffer;
    // allocate memory to contain the whole file:
    buffer = (unsigned char*) calloc((quantum*4) + 1, sizeof(char));
    if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

    // Round Robin, loop till all files are empty
    int allEmpty = 1;
    int index = 0;
    int readReturn = 0;
    while(1)
    {
        readReturn = fread(buffer,1,(quantum*4),fp[index]);
        // readReturn = read(fp[index], buffer, (quantum*4)); 
        if (readReturn != 0)
        {
            allEmpty = 0;
            /* In this assignment the process is simulated by the tracefile,
            hence the processId can be simulated by the index of the trace file.
            */
            processQuantum(buffer, quantum, index);
        }

        // printf("Buffer: %s\n", buffer);
        // bzero(buffer, (quantum*4));

        if (index == (numTraceFiles - 1) && allEmpty == 1)
        {
            break;
        }

        if (index == (numTraceFiles - 1))
        {
            allEmpty = 1;
            index = 0;
            continue;
        }
        index++;
    }

    // maxTLBSize = 8;
    // evictionPolicy = 'l';
    // demoTLB();

    return 0;
}
