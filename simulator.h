#include <stdio.h>
#include <stdlib.h>

int maxTLBSize;
int currentTLBSize = 0;
char* evictionPolicy;

typedef struct TLBElement
{
    // pageNum | frameNum | PID | V/I
    int * pageNum;
    int * frameNum;
    int * PID;
    int * valid;
    struct TLBElement* prev;
    struct TLBElement* next;
} TLBElement;

TLBElement* TLB;
