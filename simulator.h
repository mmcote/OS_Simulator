#include <stdio.h>
#include <stdlib.h>
#include <time.h>


unsigned int maxTLBSize;
unsigned int currentTLBSize = 0;

unsigned int maxVMSize;
unsigned int currentVMSize = 0;

char* evictionPolicy;

typedef struct DLLElement
{
    // pageNum | frameNum | PID | V/I
    unsigned int * pageNum;
    unsigned int * frameNum;
    unsigned int * PID;
    unsigned int * valid;
    struct DLLElement* prev;
    struct DLLElement* next;
} DLLElement;

DLLElement* TLB;
DLLElement* TLBEnd;

DLLElement* VM;
DLLElement* VMEnd;

