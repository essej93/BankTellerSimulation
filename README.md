# Bank Teller Simulation
## Description
This was developed to better understand data types, queuing and big-O. Strictly we had to use primitive data types as well as c-string/char arrays; which means no third party libraries of data structures and algorithms that we needed.

The program is strictly a multiple teller - single queue simulation.

This program takes a .txt file where each line represents a Customer interaction and has <ArrivalTime> <ServiceTime> <Priority>. You also select how many "Tellers" there are to process each Customer. (A sample file has been provided)

After the last customer is processed it will then print out stats of each teller in relation to idle time/rate.

I’ve used a heap structure to keep track of the customer queue throughout the program. The 
heap provides a quick way of sorting newly arrived customers to the correct position in the queue 
but mainly to ensure the correct customer is at the top of the queue. It also allows quick access to 
the first customer in the queue and to easily re-sort the heap after a customer has been removed 
from the queue.
