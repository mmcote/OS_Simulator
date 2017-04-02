#include <stdio.h>
#include <stdlib.h>

char evictionPolicy;

typedef struct
{
    int * pageNum;
    int * frameNum;
    struct TLBElement* next;
} TLBElement;