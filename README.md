# PageReplacementSimulator

Mohannad Shihadeh, Daniel Shultzl Gopal Krishna Shukla 

The following files are necessary to compile the program – 

memsim.cpp – Main interface of the simulator. It takes 2 forms of input arguments – with and without the partition value ranging from 0 – 100.  

fifo.cpp – Implements the First in First Out page replacement policy.  

lru.cpp – Implements the Least Recently Used page replacement policy.  

vms.cpp – Implements the Segmented First in First Out page replacement policy.  

policies.hpp – Header file for all the three page replacement policies.  

bzip.trace – Trace file.  

sixpack.trace – Trace file.  


To compile the program, use this statement – 
make

Alternatively, you can also use - 
g++ -std=c++11 -Wall memsim.cpp -o memsim

To run the page replacement policies, use the following statements –   


FIFO – ./memsim <file_name> <nFrames> <fifo> <system_mode>. 
Ex – ./memsim bzip.trace 64 fifo quiet. 
  
LRU – ./memsim <file_name> <nFrames> <lru> <system_mode>. 
Ex – ./memsim bzip.trace 64 lru quiet. 
  
VMS/SFIFO – ./memsim <file_name> <nFrames> <vms> <partition_size> <system_mode>. 
Ex – ./memsim bzip.trace 64 vms 50 quiet. 
  
Because of the way the code has been written, if the input is any filename other than “bzip.trace” or “sixpack.trace”, it will throw an error. To fix this, we can simply add another else if statement on line 23 in vms.cpp and fifo.cpp and line 93 in lru.cpp with the name of the file.
