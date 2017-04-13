#include "tvm379.h"

/* This function is used to mark a particular DLL Element as either valid or invalid in a particular DLL */
void markValidity(DLLElement* DLL, unsigned int pageNum, unsigned int frameNum, unsigned int PID, unsigned int validity)
{
    DLLElement* cursor = DLL;
    while (cursor->pageNum != pageNum && cursor->PID != PID && (cursor->frameNum == NULL || cursor->frameNum != frameNum))
    {
        cursor = cursor->next;
        if (cursor == NULL)
        {
            return;
        }
    }
    cursor->valid = validity;
    return;
}

/* clearDLL is used to clear a given DLL by freeing all elements in the DLL */
void clearDLL(DLLElement* head)
{
   DLLElement* tmp;

   while (head != NULL)
    {
       tmp = head;
       head = head->next;
       free(tmp);
    }
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

    newNode->pageNum = pageNum;
    newNode->frameNum = frameNum;
    newNode->PID = PID;
    newNode->valid = 1;

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

    free(front);

    return head;
}

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
        free(tmp);
    }
    return head;
}

/* VMMakeMostRecent will use the frame array to get a element by corresponding frame 
number and modify it to be at the beginning of the VM
*/
DLLElement* VMMakeMostRecent(DLLElement* DLLVM, DLLElement** DLLVMEnd, DLLElement** frames, unsigned int frame)
{
    // printf("Check 1\n");
    // printf("frame: %u\n", frame);
    DLLElement* cursor = frames[frame];

    // printf("Check 2\n");
    if (cursor == NULL)
    {
        printf("Error.");
        return DLLVM;
    }

    // printf("Check 3\n");
    // Check if element is the head
    if (cursor->prev != NULL)
    {
        cursor->prev->next = cursor->next;
    }
    else
    {
        return DLLVM;
    }

    // printf("Check 4\n");
    // If it is the last element
    if (cursor->next != NULL)
    {
        cursor->next->prev = cursor->prev;
    }
    else // Must be the last element
    {
        cursor->prev->next = NULL;
    }

    // printf("Check 5\n");
    if (cursor == *DLLVMEnd)
    {
        *DLLVMEnd = cursor->prev;
    }

    // printf("Check 6\n");
    DLLVM->prev = cursor;
    cursor->next = DLLVM;
    cursor->prev = NULL;

    return cursor;
}

/* makeMostRecentGiven element, will make a given element the most recent when given the
DLL that it is part of
*/
DLLElement* makeMostRecentGivenElement(DLLElement* head, DLLElement** end, DLLElement* element)
{
    // Determine if the current element is already the head of the DLL
    if (element->prev != NULL)
    {
        element->prev->next = element->next;
    }
    else // Must be the first element
    {
        return head;
    }

    // Determine if this is the last element
    if (element->next != NULL)
    {
        element->next->prev = element->prev;
    }
    else // Must be the last element
    {
        element->prev->next = NULL;
    }

    if (element == *end)
    {
        *end = element->prev;
    }

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
        if (cursor->pageNum == pageNum && cursor->PID == PID)
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

    return cursor;
}

/* pageTableHitOrMiss is used to check whether the DLLElement belongs to the 
corresponding pageTable.
*/
DLLElement* pageTableHitOrMiss(unsigned int pageNum, unsigned int PID, DLLElement* pageTable)
{
    DLLElement* cursor = pageTable;
    while(cursor != NULL)
    {
        if (cursor->pageNum == pageNum)
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
    while(cursor != NULL)
    {
        if (cursor->pageNum == pageNum && cursor->PID == PID)
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

void processQuantum(unsigned char* buffer, int quantum, unsigned int PID, DLLElement** frames)
{
    // Variables
    unsigned char* currentLocation = buffer;
    unsigned char* miniBuffer = calloc(5, sizeof(char));
    
    unsigned int pageNum;
    unsigned int frameNum;

    // This is just for testing.
    unsigned int hits = 0;
    unsigned int checks = 0;

    unsigned int pageFaultsForQuantum = 0;
    unsigned int pageOutsForQuantum[numTraceFiles];
    int b = 0;
    for(; b < numTraceFiles; ++b)
    {
        pageOutsForQuantum[b] = 0;
    }
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

        pageNum = pageNum >> offset;
        DLLElement* TLBHit = TLBHitOrMiss(pageNum, PID, TLB);
        checks++;

        // CHECK FOR TLB HIT
        /* If there is a TLB hit then continue, although before continuing:
            - If the eviction policy is LRU --> make the DLLElement the most recent one, in all tables
        */
        if (TLBHit != NULL)
        {
            hits++;
            if (*evictionPolicy == 'l')
            {
                TLB = makeMostRecentGivenElement(TLB, &TLBEnd,TLBHit);
                VM = VMMakeMostRecent(VM, &VMEnd, frames, TLBHit->frameNum);
            }

            if (TLBHit->valid == 1)
            {
                continue;
            }
        }

        int pageTableHit = -1;
        pageTableHit = pageTablesArray[PID][pageNum];
        if (pageTableHit != -1)
        {
            addToPT = 0;
            addToVM = 0;

            /* This is if the element is valid and we don't have to substitute an
            element from the VM
            */
            if (*evictionPolicy == 'l')
            {
                VM = VMMakeMostRecent(VM, &VMEnd, frames, pageTableHit);
            }
        }

        // Here we will be adding to the VM
        if (addToVM == 1)
        {
            // Check if there is space in VM
            if (currentVMSize < maxVMSize)
            {
                frameNum = currentVMSize;
            }
            else
            {
                /* Grab the frame number of the DLL element that's being 
                evicted, then evict
                */
                frameNum = VMEnd->frameNum;  
                currentVMDistribution[VMEnd->PID] -= 1;

                // printf("VMEnd->pageNum: %u, VMEnd->PID: %u, VMEnd->frameNum: %u\n", VMEnd->pageNum, VMEnd->PID, VMEnd->frameNum);
                // Mark pageout for process
                pageOutsForQuantum[VMEnd->PID] += 1;

                // Set the pageTable element to NULL representing an invalid bit
                pageTablesArray[VMEnd->PID][VMEnd->pageNum] = -1;

                // Check if the element being removed is in the TLB
                DLLElement* EndElementTLBHit = TLBHitOrMiss(VMEnd->pageNum, VMEnd->PID, TLB);
                if (EndElementTLBHit != NULL)
                {
                    markValidity(TLB, pageNum, frameNum, PID, 0);
                }
            }

            // Only when we add to the VM do we set the frames[frameNum] pointer
            VM = addToDLL(VM, &VMEnd, pageNum, frameNum, PID, &currentVMSize, maxVMSize);
            pageFaultsForQuantum += 1;
            currentVMDistribution[PID] += 1;

            // Only when we change an element in the VM do we update the frame
            frames[frameNum] = VM;
        }

        // Grab the frame number from the pageTable, if it is already in the VM
        if (addToVM != 1)
        {
            frameNum = pageTablesArray[PID][pageNum];
        }

        // // Add to page Table
        if (addToPT == 1)
        {
            pageTablesArray[PID][pageNum] = frameNum;
        }
        // Last step is always adding it to the VM
        TLB = addToDLL(TLB, &TLBEnd, pageNum, frameNum, PID, &currentTLBSize, maxTLBSize);
    }

    int p = 0;
    for (; p < numTraceFiles; p++)
    {
        // printf("PID: %u, pageOuts[%d]: %u, pageOutsForQuantum[%d]: %u\n", p, p, pageOuts[p], p, pageOutsForQuantum[p]);
        pageOuts[p] += pageOutsForQuantum[p];
    }
    pageFaults[PID] += pageFaultsForQuantum;
    pageHits[PID] += hits;
    return;
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
    numTraceFiles = argc - 7;

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
    int test = pgsize;
    int test2 = maxTLBSize;

    // Ensure the pgsize is a power of 2
    while (((test % 2) == 0) && test > 1)
    {
        test /= 2;
        ++offset;
    }

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


    pageFaults = malloc(numTraceFiles*sizeof(unsigned int));
    pageOuts = malloc(numTraceFiles*sizeof(unsigned int));
    pageHits = malloc(numTraceFiles*sizeof(unsigned int));
    currentVMDistribution = malloc(numTraceFiles*sizeof(unsigned int));
    currentAverages = malloc(numTraceFiles*sizeof(unsigned int));

    FILE** fp[numTraceFiles];
    unsigned int filesize[numTraceFiles];
    char* fileName;

    int a = 0;
    unsigned int TLBHits[numTraceFiles];
    for (a = 0; a < numTraceFiles; ++a)
    {
        fp[a] = NULL;
        fileName = argv[a + 7];
        fp[a] = fopen(fileName, "rb");

        TLBHits[a] = 0;

        pageFaults[a] = 0;
        pageOuts[a] = 0;
        pageHits[a] = 0;

        currentVMDistribution[a] = 0;
        currentAverages[a] = 0;
    }

    // Initialize the TLB
    TLB = NULL;
    TLBEnd = (DLLElement*)malloc(sizeof(DLLElement));
    TLBEnd->pageNum = NULL;

    // Initialize the Virtual Memory
    VM = NULL;
    VMEnd = (DLLElement*)malloc(sizeof(DLLElement));
    VMEnd->pageNum = NULL;

    int j = 0;
    // Initialize the Page Tables for all processes
    pageTablesArray = (int **) malloc(numTraceFiles*sizeof(unsigned int *));
    for(i = 0;i < numTraceFiles; i++)
    {
        pageTablesArray[i]=(int *) malloc(pow(2, 32-test)*sizeof(unsigned int));
        for (j = 0; j < pgsize; ++j)
        {
            pageTablesArray[i][j] = -1;
        }
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
    long double tillEndCount[numTraceFiles];
    for (i = 0; i < numTraceFiles; ++i)
    {
        tillEndCount[i] = 0;
    }
    long double totalEndCount = 0;
    while(1)
    {
        readReturn = fread(buffer,1,quantum*4,fp[index]);

        if (readReturn != 0)// && readAmount != 0)
        {
            allEmpty = 0;
            /* In this assignment the process is simulated by the tracefile,
            hence the processId can be simulated by the index of the trace file.
            */
            processQuantum(buffer, quantum, index, &frames);

            if (*uniformity == 'p' && TLB != NULL)
            {
                clearDLL(TLB);
                TLBEnd = NULL;
                TLB = NULL;
                currentTLBSize = 0;
            }
            tillEndCount[index] += 1;
            totalEndCount += 1;
            int k = 0;
            for (k = 0; k < numTraceFiles; ++k)
            {
                currentAverages[k] += currentVMDistribution[k];
            }
        }

        bzero(buffer, quantum*4);
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
    long double average = 0;
    long double difference;
    // Print the stats of the simulation
    int m = 0;
    for (; m < numTraceFiles; m++)
    {
        difference = pageFaults[m] - pageOuts[m];
        average = currentAverages[m] / totalEndCount;
        // average = average / totalQuantumCount;
        printf("%u %u %u %LF\n", pageHits[m], pageFaults[m], pageOuts[m], average);
    }

    return 0;
}
