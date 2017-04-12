#include "simulator.h"

/* reverse bit order of unsigned int
http://stackoverflow.com/questions/9144800/c-reverse-bits-in-unsigned-integer
*/
unsigned int reverse(unsigned int x)
{
    x = ((x >> 1) & 0x55555555u) | ((x & 0x55555555u) << 1);
    x = ((x >> 2) & 0x33333333u) | ((x & 0x33333333u) << 2);
    x = ((x >> 4) & 0x0f0f0f0fu) | ((x & 0x0f0f0f0fu) << 4);
    x = ((x >> 8) & 0x00ff00ffu) | ((x & 0x00ff00ffu) << 8);
    x = ((x >> 16) & 0xffffu) | ((x & 0xffffu) << 16);
    return x;
}

void printBinary(unsigned int num)
{
    unsigned int copy;
    unsigned int shiftedVal;
    int i = 0;
    for (; i < 32; ++i)
    {
        copy = num;
        // Left shift
        copy = copy << i;
        // Right shift
        copy = copy >> 31;
        
        if (copy == 1)
        {
            printf("1");
        }
        else
        {
            printf("0");
        }
    }
    printf("\n");
}

// Print the Doubly Linked List from the start
void printDLL(DLLElement* head)
{
    DLLElement* cursor = head;
    while(cursor != NULL)
    {
        printf("Page Number: %u, Frame Number: %u\n", *cursor->pageNum, *cursor->frameNum);
        cursor = cursor->next;
    }
}

printDLLSize(DLLElement* head)
{
    DLLElement* cursor = head;
    unsigned int i = 0;
    while (cursor != NULL)
    {
        ++i;
        cursor = cursor->next;
    }
    printf("Size of the DLL: %u\n", i);
}

// Print the Doubly Linked List from the end up
void printDLLBackwards(DLLElement* head)
{
    DLLElement* cursor = head;
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

void markValidity(DLLElement* DLL, unsigned int pageNum, unsigned int frameNum, unsigned int PID, unsigned int validity)
{
    DLLElement* cursor = DLL;
    while (*cursor->pageNum != pageNum && *cursor->PID != PID && (cursor->frameNum == NULL || *cursor->frameNum != frameNum))
    {
        cursor = cursor->next;
    }
    cursor->valid = validity;
    return;
}

// Used to create the head node
DLLElement* create(unsigned int pageNum, unsigned int frameNum, unsigned int PID, DLLElement* prev, DLLElement* next)
{
    DLLElement* newNode = (DLLElement*)malloc(sizeof(DLLElement));
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

    if (next != NULL)
    {
        next->prev = newNode;
    }

    if (prev != NULL)
    {
        prev->next = newNode;
    }
    return newNode;
}

DLLElement* push(DLLElement* head, unsigned int pageNum, unsigned int frameNum, unsigned int PID)
{
    if(head == NULL)
    {        
        return NULL;
    }

    /* create a new DLLElement */
    DLLElement* newNode =  create(pageNum, frameNum, PID, NULL, head);

    return newNode;
}

/* Append will be used for the TLB to add a newly loaded entry into the TLB
As for LRU, if this is just used once it will be the least
recently used.
As for FIFO, we can just grab from the head.
*/
// TODO: Change to append to the end node of the DLL
DLLElement* append(DLLElement* head, unsigned int pageNum, unsigned int frameNum, unsigned int PID)
{
    if(head == NULL)
    {        
        return NULL;
    }

    /* go to the last DLLElement */
    DLLElement *cursor = head;
    while(cursor->next != NULL)
    {
        cursor = cursor->next;
    }

    /* create a new DLLElement */
    DLLElement* newNode =  create(pageNum, frameNum, PID, cursor, NULL);

    return head;
}

/* removeFront will be used especially in the FIFO eviction policy as new
elements being inserted will just be added to the back and therefore the first
in will be those residing in the front.
*/
DLLElement* removeFront(DLLElement* head)
{
    // If the head is already NULL
    if(head == NULL)
    {
        return NULL;
    }

    // Check if the head is the only element
    DLLElement *front = head;
    
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


// // TODO: Remove end with pointer to end node
// DLLElement* removeBack(DLLElement* head)
// {
//     // If the head is already NULL
//     if(head == NULL)
//     {
//         return NULL;
//     }

//     // Get the last element of the linked list
//     DLLElement *cursor = head;
//     DLLElement *back = NULL;
//     while(cursor->next != NULL)
//     {
//         back = cursor;
//         cursor = cursor->next;
//     }

//     if(back != NULL)
//     {
//         back->next = NULL;
//     }

//     // if this is the last DLLElement in the list
//     if(cursor == head)
//     {
//         head = NULL;
//     }

//     free(cursor->pageNum);
//     free(cursor->frameNum);
//     free(cursor->PID);
//     free(cursor->valid);
//     free(cursor);

//     return head;
// }


// This function will return the new last element of the list
DLLElement* removeBack(DLLElement* end)
{
    // If the end is already NULL
    if(end == NULL)
    {
        return NULL;
    }

    DLLElement* freeDLLElement = end;
    if (end->prev != NULL)
    {
        end->prev->next = NULL;
        end = end->prev;
    }

    free(freeDLLElement->pageNum);
    free(freeDLLElement->frameNum);
    free(freeDLLElement->PID);
    free(freeDLLElement->valid);
    free(freeDLLElement);

    return end;
}

/* removeNode DLLElement from the anywhere in the TLB will be used especially 
in the LRU eviction policy as we will always be removing the least recently 
used which may be anywhere in the TLB.
*/
DLLElement* removeNode(DLLElement* head,DLLElement* nd)
{
    // If the DLLElement passed was NULL
    if(nd == NULL)
    {
        return NULL;
    }

    // if the DLLElement is the first DLLElement
    if(nd == head)
    {
        return removeFront(head);
    }

    // if the DLLElement is the last DLLElement
    if(nd->next == NULL)
    {
        return removeBack(head);
    }

    // if the DLLElement is in the middle
    DLLElement* cursor = head;
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
        DLLElement* tmp = cursor->next;
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

/* VMMakeMostRecent will use the frame array to get a element by corresponding frame 
number and modify it to be at the beginning of the VM
*/
DLLElement* VMMakeMostRecent(DLLElement* VM, DLLElement** frames, unsigned int frame)
{
    DLLElement* cursor = frames[frame];
    cursor->prev->next = cursor->next;
    cursor->next->prev = cursor->prev;

    cursor->prev = NULL;

    VM->prev = cursor;
    cursor->next = VM;

    return cursor;
}

/* makeMostRecentGiven element, will make a given element the most recent when given the
DLL that it is part of
*/
DLLElement* makeMostRecentGivenElement(DLLElement* head, DLLElement* element)
{
    if (element == VMEnd)
    {
        VMEnd = element->prev;
    }

    element->next->prev = element->prev;
    element->prev->next = element->next;
    element->prev = NULL;

    head->prev = element;
    element->next = head;
    element->prev = NULL;

    return element;
}

/* makeMostRecent will take a pageNum, find the node, and make it most recent.
In this case making the page the most recent involves bringing the element to 
front of the list.
*/
DLLElement* makeMostRecent(DLLElement* head, DLLElement** end, unsigned int pageNum, unsigned int PID)
{
    DLLElement* cursor = head;

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
            return cursor;
        }

        // Set the next cursor to be checked
        cursor = cursor->next;
    }

    if (cursor->prev != NULL)
    {
        cursor->prev->next = cursor->next;
        if (cursor == *end && (*end)->prev != NULL)
        {
            (*end)->prev->next = NULL;
            *end = cursor->prev;
        }
    }

    // Handle previous and next
    if (cursor->next != NULL)
    {
        cursor->next->prev = cursor->prev;
    }

    // Handle the head of the doubly linked list
    head->prev = cursor;
    cursor->next = head;
    cursor->prev = NULL;

    // If the item is being made most recent then 
    cursor->valid = 1;

    head = cursor;
}

/* pageTableHitOrMiss is used to check whether the DLLElement belongs to the 
corresponding pageTable.
*/
DLLElement* pageTableHitOrMiss(unsigned int pageNum, unsigned int PID, DLLElement* pageTable)
{
    DLLElement* cursor = pageTable;
    while(cursor != NULL)
    {
        if (*cursor->pageNum == pageNum)
        {
            return cursor;
        }
        cursor = cursor->next;
    }
    return NULL;
}

/* TLBHitOrMiss is used to check whether the DLLElement belongs to the current
TLB state.
*/
DLLElement* TLBHitOrMiss(unsigned int pageNum, unsigned int PID, DLLElement* TLB)
{
    DLLElement* cursor = TLB;
    int i = 0;
    while(1)
    {
        if (cursor == NULL || cursor->pageNum == NULL)
        {
            break;
        }

        if (*cursor->pageNum == pageNum && *cursor->PID == PID)
        {
            return cursor;
        }
        cursor = cursor->next;
    }
    return NULL;
}

/* 
    This function will try to add to the corresponding DLL, while limiting
    the size of the overall DLL
*/
DLLElement* addToDLL(DLLElement* DLL, DLLElement** DLLEnd, unsigned int pageNum,
    unsigned int frameNum, unsigned int PID, unsigned int* currentSize, unsigned int maxSize)
{  
    // If the max size has been reached then must evict
    // currentSize will be NULL when the size does not matter.
    if (currentSize != NULL && *currentSize >= maxSize)
    {
        /* Remove an element from the TLB, independent of the
        eviction policy here, that is handled elsewhere, here
        we just remove
        */
        *DLLEnd = removeBack(*DLLEnd);
        *currentSize -= 1;
    }

    if (currentSize != NULL)
    {
        *currentSize += 1;
    }

    if (DLL == NULL)
    {
        DLLElement* onlyElement = create(pageNum, frameNum, PID, NULL, NULL);
        if (DLLEnd != NULL)
        {
            *DLLEnd = onlyElement;
        }
        return onlyElement; 
    }
    return push(DLL, pageNum, frameNum, PID);
}

int processQuantum(unsigned char* buffer, int quantum, unsigned int PID, DLLElement** pageTable, DLLElement** frames)
{
    // Variables
    unsigned char* currentLocation = buffer;
    unsigned char* miniBuffer = calloc(5, sizeof(char));
    
    unsigned int pageNum;
    unsigned int frameNum;

    // This is just for testing.
    unsigned int hits = 0;
    unsigned int checks = 0;

    /* This is a check variable, that will be flagged if we intend to add to the 
    page table */
    int addToPT;

    /* This is a check variable, that will be flagged if we intend to add to the 
    page table */
    int addToVM;

    // Process whole quantum
    int i = 0;
    for (; i < quantum; ++i)
    {
        // Reset vars, 
        addToVM = 1;
        addToPT = 1;
        pageNum = 0;

        // start = clock();
       
        // // Foreach quantum read in the four next bytes
        int j = 0;
        for (; j < 4; ++j)
        {
            miniBuffer[j] = *currentLocation;
            currentLocation++;
            int k = 0;
            for (; k < 8; ++k) {
                if ( (!!((miniBuffer[j] << k) & 0x80)) == 1)
                {
                    pageNum |= 1 << ((32 - j*8 - 1) - k);
                }
            }
        }

        // reverse the order of the read in bits
        pageNum = reverse(pageNum);
        pageNum = pageNum >> 12;

        DLLElement* TLBHit = TLBHitOrMiss(pageNum, PID, TLB);
        checks++;

        /* If there is a TLB hit then continue, although before continuing:
            - If the eviction policy is LRU --> make the DLLElement the most recent one, in all tables
        */
        if (TLBHit != NULL)
        {
            hits++;

            if (evictionPolicy == 'l')
            {
                // Optimization
                TLB = makeMostRecentGivenElement(TLB, TLBHit);

                // CHANGE PAGE TABLES TO GIANT ARRAYS --> Prioritizing speed
                // pageTable = makeMostRecent(pageTable, NULL, pageNum, PID);

                // // Need to come up with a different method for updating the VM rather than traversing the whole thing
                // VM = VMMakeMostRecent(VM, frames, PID);
                // //VM = makeMostRecent(VM, pageNum, PID);
            }

            if (TLBHit->valid == 1)
            {
                continue;
            }
        }

        DLLElement* pageTableHit = pageTableHitOrMiss(pageNum, PID, *pageTable);
        if (pageTableHit != NULL)
        {
        //     addToPT = 0;

        //     /* This is if the element is valid and we don't have to substitute an
        //     element from the VM
        //     */
        //     if (pageTableHit->valid != 1)
        //     {               
        //         // if (evictionPolicy == 'l')
        //         // {
        //         //     VM = VMMakeMostRecent(VM, frames, PID);
        //         //     // VM = makeMostRecent(VM, pageNum, PID);
        //         // }
        //         // addToVM = 0;
        //     }

        //     if (evictionPolicy == 'l') 
        //     {
        //         *pageTable = makeMostRecentGivenElement(*pageTable, pageTableHit);
        //     }
        // } 

    //     // Here we will be adding to the VM
    //     if (addToVM == 1)
    //     {
    //         // Check if there is space in VM
    //         if (currentVMSize < maxVMSize)
    //         {
    //             frameNum = currentVMSize;
    //         }
    //         else
    //         {
    //             /* Grab the frame number of the DLL element that's being 
    //             evicted, then evict
    //             */
    //             frameNum = *VMEnd->frameNum;  

    //             /* If the given element being removed is in the page table
    //             or TLB then set there validity bit as invalid.
    //             */
    //             if (pageTableHit != NULL)
    //             {
    //                 markValidity(pageTable, pageNum, NULL, PID, 0);
    //             }

    //             if (TLBHit != NULL)
    //             {
    //                 markValidity(TLB, pageNum, frameNum, PID, 0);             
    //             }

    //             diff = clock() - start;
    //             int msec = diff * 1000 / CLOCKS_PER_SEC;
    //             printf("Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);
    //         }

    //         // Only when we add to the VM do we set the frames[frameNum] pointer
    //         addToDLL(&VM, &VMEnd, pageNum, frameNum, PID, &currentVMSize, maxVMSize);

    //         // Only when we change an element in the VM do we update the frame
    //         frames[frameNum] = VM;

    //         if (pageTableHit != NULL)
    //         {
    //             pageTableHit->valid = 1;

    //             if (TLBHit != NULL)
    //             {
    //                 TLBHit->valid = 1;
    //                 continue;
    //             }
    //         }
    //     }

        // // Grab the frame number from the pageTable, if it is already in the VM
        // if (addToVM != 1)
        // {
        //     frameNum = *pageTableHit->frameNum;
        // }

        // Add to page Table
        // if (addToPT == 1)
        // {
        //     *pageTable = addToDLL(*pageTable, NULL, pageNum, frameNum, PID, NULL, 0);
        // }

        // Last step is always adding it to the VM
        frameNum = 5;
        TLB = addToDLL(TLB, &TLBEnd, pageNum, frameNum, PID, &currentTLBSize, maxTLBSize);
    }
    // printDLLSize(*pageTable);
    // printf("Hit/Checks %d/%d\n", hits, checks);
    return hits;
}

void demoTLB()
{
    unsigned int current = 0;
    DLLElement* end = malloc(sizeof(DLLElement));
    DLLElement* head = NULL;
    head = addToDLL(head, &end, 10, 1, 1, &current, 10);
    head = addToDLL(head, &end, 9, 1, 1, &current, 10);
    head = addToDLL(head, &end, 8, 1, 1, &current, 10);
    head = makeMostRecent(head, &end, 10, 1);
    head = addToDLL(head, &end, 7, 1, 1, &current, 10);
    head = addToDLL(head, &end, 6, 1, 1, &current, 10);
    head = addToDLL(head, &end, 5, 1, 1, &current, 10);
    head = addToDLL(head, &end, 4, 1, 1, &current, 10);
    head = addToDLL(head, &end, 3, 1, 1, &current, 10);
    head = makeMostRecent(head, &end, 5, 1);
    head = addToDLL(head, &end, 2, 1, 1, &current, 10);
    head = makeMostRecent(head, &end, 9, 1);
    head = addToDLL(head, &end, 1, 1, 1, &current, 10);
    head = addToDLL(head, &end, 6, 1, 1, &current, 10);
    head = addToDLL(head, &end, 5, 1, 1, &current, 10);
    head = addToDLL(head, &end, 4, 1, 1, &current, 10);
    head = addToDLL(head, &end, 3, 1, 1, &current, 10);

    printDLL(head);
    printf("-------------------\n");
    printDLLBackwards(end);
}

int main(int argc, char **argv)
{
    clock_t start = clock(), diff;

    // Basic check to see if there is even enough elements to make a proper request to the program
    if (argc < 8){
        printf("tvm379 pgsize tlbentries { g | p } quantum physPages { f | l } trace1 trace2 . . . tracen\n");
        exit(1);
    }

    /* If this was inputted correctly only the first 7 are not trace files,
    the rest of the arguments are trace files
    */
    unsigned int numTraceFiles = argc - 7;

    // pgsize, after the offset we arrive at 2^20, divide this by the pgsize, and we get the size of the pageTable
    unsigned int pgsize = atoi(argv[1]);
    
    // no interpolation needed, this inputted argument is the final size of the TLB
    maxTLBSize = atoi(argv[2]);

    // uniformity is the eviction policy
    char* uniformity = argv[3];

    // amount of 32 bit addresses we will be reading from each process during the round robin
    unsigned int quantum = atoi(argv[4]);

    // this is the size of the virtual memory which will determine the number of frames we have
    unsigned int physPages = atoi(argv[5]);
    maxVMSize = physPages;

    /* Eviction Policy is now a global variable as it will influence the flow 
    of the program
    */
    evictionPolicy = calloc(2, sizeof(char));
    evictionPolicy = argv[6];

    //command line arguments, powers of 2
    {
        int test = pgsize;
        int test2 = maxTLBSize;

        // Ensure the pgsize is a power of 2
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

    if (physPages > 1000000){
        printf("physPages cannot be greater than 1000000\n");
        exit(1);
    }

    // Create an array of physpages size, that holds all the pointers to the DLLElement that is using the frameNumber
    DLLElement* frames[physPages];
    int i;
    for (i = 0; i < physPages; ++i)
    {
        frames[i] = NULL;
    } 

    if (*evictionPolicy != 'f' && *evictionPolicy != 'l')
    {
        printf("Usage: Please enter either f(FIFO) or l (LRU), to properly define a page eviction policy.");    
        exit(-1);
    }

    FILE** fp[numTraceFiles];
    char* fileName;
    for (i = 0; i < numTraceFiles; ++i)
    {
        fp[i] = NULL;
        fileName = argv[i + 7];
        fp[i] = fopen(fileName, "rb");
    }

    // Initialize the TLB
    TLB = NULL;
    TLBEnd = (DLLElement*)malloc(sizeof(DLLElement));
    TLBEnd->pageNum = NULL;

    // Initialize the Virtual Memory
    VM = NULL;
    VMEnd = (DLLElement*)malloc(sizeof(DLLElement));
    VMEnd->pageNum = NULL;

    // Initialize the Page Tables for all processes
    DLLElement* pageTables[numTraceFiles];
    for (i = 0; i < numTraceFiles; ++i)
    {
        pageTables[i] = NULL;
    }

    DLLElement* VMHash[physPages];
    for (i = 0; i < physPages; ++i)
    {
        VMHash[i] = NULL;
    }

    unsigned char* buffer;
    // allocate memory to contain the whole file:
    buffer = (unsigned char*) calloc((quantum*4) + 1, sizeof(char));
    if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

    // Round Robin, loop till all files are empty
    int allEmpty = 1;
    int index = 0;
    int readReturn = 0;
    // The only way that this loop will quit is when all the files are empty
    // int z = 0;
    while(1)
    {
        // printf("Z: %d\n", z);
        // ++z;
        // printf("Quantum\n");
        // Read in only the amount that is needed for one round of the round robin
        readReturn = fread(buffer,1,(quantum*4),fp[index]);
        if (readReturn != 0)
        {
            allEmpty = 0;
            /* In this assignment the process is simulated by the tracefile,
            hence the processId can be simulated by the index of the trace file.
            */
            processQuantum(buffer, quantum, index, &pageTables[index], frames);
        }

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
    int msec;
    diff = clock() - start;
    msec = diff * 1000 / CLOCKS_PER_SEC;
    printf("Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);

    // maxTLBSize = 8;
    // evictionPolicy = 'l';
    // demoTLB();
    return 0;
}
