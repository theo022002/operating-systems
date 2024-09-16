# Operating system overview: 
This project involves developing a system of N+1 processes (1 parent and N children) 
with the following behavior:  

## Basic Process (Parent) Responsibilities:
1.	The parent process starts by receiving a large text file (>1000 lines) as input.
2.	It also receives a parameter defining the partition size (e.g., 100 lines per partition).
3.	The parent is responsible for managing the file partitioning and handling requests from child processes to load file sections into memory.
4.	File sections are loaded into shared memory and are replaced using a FIFO strategy when memory is full, ensuring efficient access and replacement of sections when needed.
5.	The parent tracks the entry and exit times of file sections in memory and handles semaphore operations to coordinate between processes.
   
## Child Process Responsibilities:  
1.	Each child process randomly selects a line from a specific partition of the file and requests it from the parent process (e.g., selecting line 34 from partition 7).
2.	If the requested partition is in memory, the child accesses the line without delay; if not, the request waits until the partition is loaded into memory.
3.	Multiple child processes can read from the same partition simultaneously (multiple readers, no mutual exclusion), but they must wait if the partition is not available.
4.	Once a partition is no longer needed, and there are pending requests for other partitions, the current partition can be replaced in memory by the requested one.
5.	After completing each request, the child can submit a new one, with a 70% probability of requesting the same partition as the previous request, and a 30% probability of selecting a new partition.
6.	Each child process handles a predefined number of requests (e.g., 1000).
7.	After receiving each line, the child waits for a short processing time (e.g., 20 ms)
to simulate processing, and logs the request details (request time, response time, file partition and line) in a log file.

## Memory Management:  
•	The file is divided into partitions, and the parent process manages loading and replacing partitions in shared memory.  
•	The parent process follows a FIFO replacement strategy to manage the memory and handle partition replacements based on the order of requests.  

## Communication & Synchronization:
•	Requests and memory operations are coordinated using semaphores to ensure orderly access and prevent conflicts.  
•	The child processes log their actions, and the parent logs memory management events (such as when a partition is loaded or removed from memory).  

This system efficiently handles large file access by multiple processes using shared memory, 
ensuring synchronization and optimized access through partitioning and memory management.  

## Execution:  
For the execution of the program, the arguments are:  
program <# of processes> <# of requests> <degree_of_partitioning> <filename>  
#### Example:
./main 10 5 100 ./test.txt 
