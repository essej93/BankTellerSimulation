#include <iostream>
#include <fstream>
#include <iomanip> // for cout formatting
#include "library.h"

using namespace std;

// tracking variables
float currentTime = 0.0f; // tracks time during simulation

int NO_OF_TELLERS;
Teller tellers[4];
int availTellerIndex; // used in isTelleravailable function
int endingServiceIndex; // used in isServiceEnding and customersBeingServed functions

// heap variables
Customer customerQueueHeap[200]; // used for customer queue
int queueSize = 0; // keeps track of queue size

// statistic variables for printSimulationStatistics
int totalCustomers = 0;
int maxQueueLength = 0;
float totalTime = 0.0f;
float totalTimeInQueue = 0.0f;
float totalServiceTime = 0.0f;
float totalTellerIdleTime = 0.0f;


// main function just includes runSimulation and
// printSimulationStatistics function calls
int main(){

    runSimulation();
    printSimulationStatistics();

}


// runSimulation gets user input for filename and number of tellers and closes if the file doesn't open
// or invalid numberr of tellers is input
// collects the customer details line by line and send to process arrival function
void runSimulation(){

    cout<<"Please input number of tellers (1-4): ";
    cin>>NO_OF_TELLERS;

    if(NO_OF_TELLERS < 1 || NO_OF_TELLERS > 4){
        cerr<<"Invalid number of tellers, terminating program"<<endl;
        exit(1);
    }

    char filename[20];
    ifstream filein;
    
    cout<<"Please input the file name: ";
    cin>>filename;

    filein.open(filename);

    // if statement checks if file opened
    if(!filein){
        cerr<<"Error opening file: " << filename <<endl;
        exit(1);
    }


    float arrivalTime, serviceTime;
    int priority;

    // while loop runs until the end of the file
    while(filein >> arrivalTime >> serviceTime >> priority){

        // arrival time of next customer is sent to checkEndingServices
        // to check if some of the services are ending before the next customer arrives.
        checkEndingServices(arrivalTime);

        // sends customer info to processArrival function
        processArrival(Customer(arrivalTime,serviceTime,priority));

    }

    filein.close();

    clearQueue(); // calls clear queue to process remaining customers in queue
}


// check ending services checks whether a service should end before
// the new customer arrives.
void checkEndingServices(float arrivalTime){

    // while loop runs before next customer arrival
    // uses isServiceEnding to check the next customer arrival time
    // against the end service times of the tellers to determine
    // which needs to be processed first
    while(isServiceEnding(arrivalTime)){
        // process ending service if the next customer arrival time is after
        // the end service time of a teller
        processServiceCompletion(tellers[endingServiceIndex]);
    }


}

// process arrival checks if a teller is available
// if one is available assigns customer to teller otherwise, sends customer to the queue.
void processArrival(Customer newCustomer){

    totalServiceTime += newCustomer.serviceTime;    

    // checks if a teller is available by using isTellerAvailable function
    if(isTellerAvailable()){
        currentTime = newCustomer.arrivalTime;
        tellers[availTellerIndex].idleTime += currentTime - tellers[availTellerIndex].serviceEndTime; // collects individual teller idle time
        tellers[availTellerIndex].serviceEndTime = newCustomer.serviceTime + currentTime;
        tellers[availTellerIndex].busy = true;
    }
    else{
        enqueueCustomer(newCustomer); // enqueue's customer if no teller available
    }

    sortTellers(); // sorts tellers after every arrival
}

// checks if there is a teller availble by
// checking the busy boolean for each teller
// as soon as it finds busy == false it will set the availTellerIndex then return
bool isTellerAvailable(){

    bool available = false;

        for(int i = 0; i < NO_OF_TELLERS; i++){
        if(tellers[i].busy == false){
            availTellerIndex = i;
            return true;
        } 
    }

    return available;
}

// enqueue customer adds customer to the end of the heap
// increments the queueSize then sifts the customer up the heap
void enqueueCustomer(Customer newCustomer){
    
    customerQueueHeap[queueSize++] = newCustomer;
    siftUp(queueSize-1);

    // if statement used to track the maximum size of the queue
    if(queueSize > maxQueueLength) maxQueueLength = queueSize;
}

// siftUp function to sift a newly added customer up the heap to the correct spot
void siftUp(int i){

    // if statements to stop recursion/process if queuesize is 1 or
    // the current node is the root
    if(queueSize == 1) return;
    if(i == 0) return;
    int p = i / 2; // gets parent node index

    // if statement checks if parent priority is higher than the child, returns if it is.
    // otherwise checks if both child and parent nodes priority equal each other then
    // checks if parents arrival time is before childs arrival time.
    if(customerQueueHeap[p].priority > customerQueueHeap[i].priority) return;
    else if(customerQueueHeap[p].priority == customerQueueHeap[i].priority) {
        if (customerQueueHeap[p].arrivalTime < customerQueueHeap[i].arrivalTime) return;
    }  

    // swaps child and parent if function doesn't return from previous if statements.
    swap(customerQueueHeap[i], customerQueueHeap[p]);
    siftUp(p); // calls siftUp process again with swapped node to check if it needs to be moved further

}

// function used to check if a service is ending by comparing next customer arrival time
// with end service time of tellers.
bool isServiceEnding(float time){
    bool serviceEnding = false;

    // runs through each teller, as soon as one teller meets requirements of if statement
    // gets the index of that teller (which is used to send Teller to processServiceCompletion) then returns.
    for(int i = 0; i < NO_OF_TELLERS; i++){
        if(tellers[i].serviceEndTime < time && tellers[i].busy == true){
            endingServiceIndex = i;
            return true;
        }
    }

    return serviceEnding;
}

// function for processing service completion.
void processServiceCompletion(Teller& teller){
    
    // sets current time as service end time
    currentTime = teller.serviceEndTime;
    
    // checks if the queue is empty
    // if queue is empty, sets the teller to idle/not busy
    // otherwise dequeue's a customer and sets a new service end time for teller
    if(queueEmpty()){
        teller.busy = false;
    }
    else{
        teller.serviceEndTime = currentTime + dequeueCustomer();
    }

    sortTellers(); // sorts tellers after each
    teller.customersServed++; // increments customersServed after teller processes a completion
}

// function to dequeue customer which gets service time of root node
// deletes the root(removes customer from queue) then returns the service time
float dequeueCustomer(){

    float serviceTime = 0.0f;
    serviceTime = customerQueueHeap[0].serviceTime; // gets service time of root node of heap

    totalTimeInQueue += currentTime - customerQueueHeap[0].arrivalTime; // used to track queue time

    deleteRoot(); // calls delete root function

    return serviceTime;
}

// delete root function to remove customer from queue/root node
void deleteRoot(){

    // if queueSize is 1 then just reduces queue size to 0
    if(queueSize == 1){
        queueSize--;
        return;
    }

    customerQueueHeap[0] = customerQueueHeap[queueSize-1]; // assigns last node to the root node
    queueSize -= 1; // reduces the heap by 1

    siftDown(0); // sifts the root node down to the correct spot
}

// siftDown function which moves the root node down to the correct position
void siftDown(int i){

    int siftNode = i; // keeps track of node we need to sift root to
    int child = 2 * i + 1; // gets child node

    // if statement checks if child is out of bounds of heap
    if(child >= queueSize) return;
    
    // checks if child node priority is higher than the siftNode/current node
    // otherwise checks if their priorities are the same then checks which ones arrival time is smaller
    if(siftNode < queueSize && customerQueueHeap[child].priority > customerQueueHeap[siftNode].priority){
        siftNode = child;
    }
    else if(siftNode < queueSize && customerQueueHeap[child].priority == customerQueueHeap[siftNode].priority){
         if(siftNode < queueSize && customerQueueHeap[child].arrivalTime < customerQueueHeap[siftNode].arrivalTime){
            siftNode = child;
         }
    }

    // if the node that we are sifting the indexed node to is the same then we will not swap them
    // as it's the same node otherwise it swaps the nodes then uses recursion to siftDown the swapped node.
    if(siftNode != i){
        swap(customerQueueHeap[i], customerQueueHeap[siftNode]);

        siftDown(siftNode);
    }


    return;
}


// sortTellers to sort tellers in ascending order of service end times
// to ensure that they are processed in chronological order
// uses insert sort
void sortTellers(){

    if (NO_OF_TELLERS == 1) return; // if only 1 teller then returns

    Teller temp;
    int x;

    // for loop and while loop uses insertion sort to sort in ascending order
    for(int i = 0; i < NO_OF_TELLERS; i++){
        temp = tellers[i];
        x = i-1;
        while((tellers[x].serviceEndTime > temp.serviceEndTime) && x >=0 ){
            tellers[x+1] = tellers[x];
            x = x - 1;
        }
        tellers[x+1] = temp;
    }
}


// clear queue checks if customers are still being served
// if customers being served then it processes the completion of the service using
// the endingServiceIndex which is assigned in customersBeingServed
void clearQueue(){

    while(customersBeingServed()){
        processServiceCompletion(tellers[endingServiceIndex]);
    }
}

// function to check if customers are still being served
bool customersBeingServed(){

    bool stillBeingServed = false;

    // for loops checks for first teller that is still serving customer
    // sets endingServiceIndex tot that teller then returns true
    for(int i = 0; i<NO_OF_TELLERS; i++){
        if(tellers[i].busy == true){
            endingServiceIndex = i;
            return true;
        }
    }

    return stillBeingServed;
}

// function to just check whether the queue is empty
bool queueEmpty(){

    bool isEmpty = false;

    if(queueSize == 0) return true;
    return isEmpty;
}


// swap function just to swap child/parent nodes
void swap(Customer *c, Customer *p){
    
    Customer temp = *c;
    *c = *p;
    *p = temp;
}

// function used to print the information of the simulation
void printSimulationStatistics(){

    float avgServiceTime, avgWaitingTime, avgLengthOfQueue, idleRate;
    
    totalTime = currentTime;

    cout<<"------------=============Simulation Statistics=============------------"<<endl;
    cout<<"Number of Tellers: "<<NO_OF_TELLERS<<endl;
    cout<<"Teller statistics (no particular order): "<<endl;
    cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
    // for loop runs through tellers to print statistics and finish adding
    // final idle time and total idle time.
    for(int i = 0; i<NO_OF_TELLERS; i++){

        tellers[i].idleTime += totalTime - tellers[i].serviceEndTime; // calculates final idle time
        totalTellerIdleTime += tellers[i].idleTime; // gets total idle time of all tellers.
        cout<<"Teller "<<i<<" customers served: "<<tellers[i].customersServed<<endl;
        cout<<"Teller "<<i<<" total idle time: "<<tellers[i].idleTime<<endl;
        cout<<"Teller "<<i<<" idle rate: "<<(tellers[i].idleTime/totalTime)<<endl;
        totalCustomers += tellers[i].customersServed;
        cout<<"-----"<<endl;
    }

    // total idle rate of each teller
    idleRate = totalTellerIdleTime / totalTime;

    cout<<"Total idle time of each teller: "<<totalTellerIdleTime<<endl;
    cout<<"Total idle rate of each teller: "<<idleRate<<endl;

    cout<<"----------------------------------------------------------------"<<endl;
    cout<<"Total number of Customers: "<<totalCustomers<<endl;
    cout<<"Total simulation time: "<<totalTime<<endl; 
    cout<<"----------------------------------------------------------------"<<endl;

    // calculates statistics of simulation
    avgServiceTime = totalServiceTime / totalCustomers;
    avgWaitingTime = totalTimeInQueue / totalCustomers;
    avgLengthOfQueue = totalTimeInQueue / totalTime;

    cout<<"Average service time per customer: "<<avgServiceTime<<endl;
    cout<<"Average waiting time per customer: "<<avgWaitingTime<<endl;
    cout<<"Maximum length of the queue: "<<maxQueueLength<<endl;
    cout<<"Average Length of the queue: "<<avgLengthOfQueue<<endl;
    
}




// Customer functions:

//Customer constructor
Customer::Customer(float aTime, float sTime, int p){
    arrivalTime = aTime;
    serviceTime = sTime;
    priority = p;
}

//Customer default constructor
Customer::Customer(){
    arrivalTime = 0;
    serviceTime = 0;
    priority = 0;
}
