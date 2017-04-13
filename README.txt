Michael Cote | mmcote
Brad Hanasyk | hanasyk1

Operating System MMU Simulator:

This project mainly consists of three fundamental components: TLB (Translation lookaside buffer), individaul page tables, and a virtual memory.
With these three components we attempt to simulate a basic MMU.

Benchmarks:
Standard Parameters used for all tests:
  - Page Size: 1024
  - TLB Entries: 128
  - Global or Process Specific: g
  - Quantum: 100
  - Physical Pages: 800
  
  Fifo or LRU | Number of Files | Filesize | Time
  ------------------------------------------------------
  FIFO        | 1               | ~150 MB  | ~ 11.8 secs
  LRU         | 1               | ~150 MB  | ~ 5.9 secs
  FIFO        | 2               | ~150 MB  | ~ 28.1 secs
              |                 | each     |
  LRU         | 2               | ~150 MB  | ~ 14.1 secs
              |                 | each     |
              
This application correctly handles both eviction policies, LRU and FIFO, these can be seen to be properly implemented in the function process quantum.
The overall program just takes off of the bottom of the doubly linked list. Because this is a doubly linked list this is done in constant time as we always have a pointer pointing to the end of the doubly linked list.
It can be seen that the process and global specific parameters are handles in the main where the round robin is handled. This application did make a tradeoff in two areas to take up more memory but have a faster implementation.
This was done by using a hash table (array indexed by frame number to pointer of element currently using the frame number). The second location was using a two dimensional array
to quickly access elements of the page table (eg. pageTableArray[PID][pageNum] --> frameNum) but this also saved a tiny bit of memory considering we no longer needed a validity element as this would be set to a flag (-1) if not valid.

