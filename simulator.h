#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
unsigned int numTraceFiles = 0;
unsigned int onetest = 1;
unsigned int maxTLBSize;
unsigned int currentTLBSize = 0;

unsigned int maxVMSize;
unsigned int currentVMSize = 0;

unsigned int offset;

int** pageTablesArray;
unsigned int* pageFaults;
unsigned int* pageOuts;
unsigned int* pageHits;

char* evictionPolicy;

typedef struct DLLElement
{
    // pageNum | frameNum | PID | V/I
    unsigned int pageNum;
    unsigned int frameNum;
    unsigned int PID;
    unsigned int valid;
    struct DLLElement* prev;
    struct DLLElement* next;
} DLLElement;

DLLElement* TLB;
DLLElement* TLBEnd;

DLLElement* VM;
DLLElement* VMEnd;