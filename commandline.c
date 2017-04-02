#include "simulator.h"


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

TLBElement* addToTLB(int pageNum, int frameNum, TLBElement* TLB)
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
   char eviction = argv[6];


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

  if (eviction == 'f')
  {

  }
  else if (eviction == 'l')
  {

  }
  else
  {
      printf("Usage: Please enter either f(FIFO) or l (LRU), to properly define a page eviction policy.");    
      exit(-1);
  }

  return 0;
}
