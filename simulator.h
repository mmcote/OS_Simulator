#include <stdio.h>
#include <stdlib.h>

int maxTLBSize;
int currentTLBSize = 0;
char evictionPolicy;

typedef struct
{
    int * pageNum;
    int * frameNum;
    struct TLBElement* next;
} TLBElement;