struct Customer {
    float arrivalTime;
    float serviceTime;
    int priority;

    Customer(float,float,int);
    Customer();
};

struct Teller {
    bool busy = false;
    float serviceEndTime = 0;
    int customersServed = 0;
    float idleTime = 0;
};


//Decalring main functions
void processArrival(Customer);
void processServiceCompletion(Teller&);
void enqueueCustomer(Customer);
float dequeueCustomer();
void readFile();
bool isTellerAvailable();
bool customersBeingServed();
void sortTellers();
bool isServiceEnding(float);
bool queueEmpty();
void clearQueue();
void runSimulation();
void printSimulationStatistics();
void checkEndingServices(float);


//Heap functions
void siftDown(int);
void siftUp(int);
void swap(Customer*, Customer*);
void deleteRoot();



