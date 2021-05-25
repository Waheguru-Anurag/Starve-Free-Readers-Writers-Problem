#include <iostream>
#include <pthread.h>

using namespace std;

// Shared Data
struct Data{
    int data1;
    int data2;
};


// Semaphore
struct Semaphore{
    private:
    int mutex;

    public:
    Semaphore(){
        // initialize semaphore
        mutex = 1;
    }

    void wait(){
        while(mutex<=0);
        mutex--;
    }

    void signal(){
        mutex++;
    }
};

// argument struct for read, write methods
struct args {
    int processId;
    int val;
};

// define semaphores, readcnt and sharedData
struct Semaphore queue;
struct Semaphore mutex;
struct Semaphore wrt; 
unsigned int readcnt;
struct Data sharedData;

// read function on sharedData
void read(void *args){
    struct args* arg = (struct args *)args;
    printf("Read Process: %d, Data: %d %d \n",arg->processId, sharedData.data1,sharedData.data2);
}

// write function on sharedData
void write(void *args){
    struct args* arg = (struct args *)args;
    sharedData.data1 = sharedData.data1 + arg->val;
    sharedData.data2 = sharedData.data2 - arg->val;

    printf("Write Process: %d, Data: %d %d \n",arg->processId, sharedData.data1,sharedData.data2);
}

// reader process
void *reader(void *args){
    // Entry Section
    queue.wait();
    mutex.wait();
    readcnt++;
    if(readcnt == 1){
        wrt.wait();
    }
    queue.signal();
    mutex.signal();

    // Critical Section
    read(args);

    // Exit Section 
    mutex.wait();
    readcnt--;
    if(readcnt == 0){   
        wrt.signal();
    }
    mutex.signal();
    return 0;
}

// writers process
void *writer(void *args){
    // Entry Section
    queue.wait();
    wrt.wait();
    queue.signal();

    // Critical Section
    write(args);

    // Exit Section 
    wrt.signal();
    return 0;
}

int main(){
    int cnt_readers = 6; // number of read processes
    int cnt_writers = 4; // number of write processes
    
    readcnt = 0; // readcnt maintaining count of number of readers initialized to 0

    // intialize the sharedData
    sharedData.data1 = 1;
    sharedData.data2 = 2;

    // threads for executing the processes
    pthread_t read_thread[cnt_readers], write_thread[cnt_writers];

    // create the reader process threads
    for(int i=0; i<cnt_readers; i++){
        // define the argumnents for the process
        struct args *arg = (struct args *)malloc(sizeof(struct args));
        arg->processId = i+1;
        arg->val = -1;

        pthread_create(&read_thread[i], NULL, reader, (void *)(arg));
    }

    // create the writer process threads
    for(int i=0; i<cnt_writers; i++){
        // define the argumnents for the process
        struct args *arg = (struct args *)malloc(sizeof(struct args));
        arg->processId = i+1;
        arg->val = i+2;
        pthread_create(&write_thread[i], NULL, writer, (void *)(arg));
    }

    // join the threads and execute them

    for(int i=0; i<cnt_readers; i++){
        pthread_join(read_thread[i], NULL);
    }

    for(int i=0; i<cnt_writers; i++){
        pthread_join(write_thread[i], NULL);
    }

    return 0;
}
