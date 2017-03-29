#include <stdio.h>

int main(int argc, char *argv[]){

   if (argc < 8){
	printf("tvm379 pgsize tlbentries { g | p } quantum physpages { f | l } trace1 trace2 . . .
tracen\n");
	exit(1);
   }

   int pgsize = argv[1];
   int tbentries = argv[2];
   char uniformity = argv[3];
   int quantum = argv[4];
   int physpages = argv[5];
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

    if (physpages > 1000000){
	printf("physpages cannot be greater than 1000000\n");
	exit(1);

    if ((argv[3] != 'g') && (argv[3] != 'p')){
	printf("tvm379 pgsize tlbentries { g | p } quantum physpages { f | l } trace1 trace2 . . .
tracen\n");
    }

    if ((argv[6] != 'f') && (argv[6] != 'l')){
	printf("tvm379 pgsize tlbentries { g | p } quantum physpages { f | l } trace1 trace2 . . .
tracen\n");
    }

    return 0;
