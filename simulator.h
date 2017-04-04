#include <stdio.h>
#include <stdlib.h>

int maxTLBSize;
int currentTLBSize = 0;

int maxVMSize;
int currentVMSize = 0;

char* evictionPolicy;

typedef struct DLLElement
{
    // pageNum | frameNum | PID | V/I
    int * pageNum;
    int * frameNum;
    int * PID;
    int * valid;
    struct DLLElement* prev;
    struct DLLElement* next;
} DLLElement;

DLLElement* TLB;
DLLElement* VM;

