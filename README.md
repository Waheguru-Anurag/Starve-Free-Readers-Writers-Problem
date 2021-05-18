# Starve Free Readers Writers Problem
This is a classical problem in Operating Systems. This is also known as the Third Readers-Writers Problem. The problem is aimed at managing access to shared resources so that there is synchronization in the values read by the reader process with the values written by the writer process.

This problem is able to handle the `Starvation` Issue by processing the requests in `FIFO` (First In First Out) manner. 

## Problem Parameters:
1. One set of Data/Resources is shared among a number of processes
2. Once a writer is ready, it performs its write. Only one writer may write at a time.
3. If a process is writing no other process can read.
4. If at least one reader is reading, no other process can write.
5. Many readers can read the data at the same time.

## Semaphores Used
1. `queue`: This semaphore maintains the order of arrival of processes/requests. The semaphore is taken up by an entity that requests access to the resource and released as soon as the entity gains access to the resource.
2. `mutex`: This semaphore is used for synchronizing the changes to `readcnt` when any reader enters or leaves from the critical section.
3. `wrt`: This semaphore controls the write access to the shared resources, by allowing only one writer to write at a time and preventing any process to write while there are any process reading the shared resources.

**All the semaphores are initialized to 1**

## Readers Process
* Code:
```
do {
    // ENTRY SECTION
    wait(queue);        // wait for the request to be lined for processing
    wait(mutex);        // reader requests read access for the resource
    readcnt++;          // increment the number of active readers by 1
    if(readcnt == 1){
        wait(wrt);      // if the reader is the first reader than lock the resource for writers
    }
    signal(queue);      // release the queue semaphore to allow next request to be serviced
    signal(mutex);      // release the mutex semaphore so that other readers can also enter to read the resource

    // CRITICAL SECTION 
    /* perform the reading operation */

    // EXIT SECTION
    wait(mutex);        // the reader wants to leave
    readcnt--;          // decrement the number of active readers by 1
    if(readcnt == 0){   
        signal(wrt);    // if there is no active reader than allow writers to access the resource
    }
    signal(mutex);      // the reader leaves
    
} while(true);
```
* Explanation:
  * Entry Section
    * Wait for the request to get lined up. After the request gets initiated the reader acquires the `queue` semaphore.
    * Reader requests access to the resource and acquires the `mutex` semaphore on getting access.
    * `readcnt` variable is incremented to accomodate the new reader of the shared resource.
    * If the reader is the first reader than the shared resources are locked for preventing the writers to have access to them and therefore the reader acquires the `wrt` semaphore. 
    * `queue` semaphore is released by the reader for allowing the next request to be processed.
    * `mutex` semaphore is released by the reader for allowing other readers to access the resource
  * Critical Section
    * Reader reads from the shared resources.
  * Exit Section
    * Reader acquires the `mutex` semaphore for changing the `readcnt` variable while leaving.
    * If the reader which is leaving is the last active reader than the `wrt` semaphore is released by the reader allowing other writers to access the resource.
    * Reader releases the `mutex` semaphore after modifying the `readcnt` at the end.

## Writers Process
* Code:
```
do{
    // ENTRY SECTION
    wait(queue);        // wait for the request to be lined for processing
    wait(wrt);          // writer requests write access for the resource
    signal(queue);      // release the queue semaphore to allow next request to be serviced

    // CRITICAL SECTION
    /* perform the writing operation */
    
    // EXIT SECTION
    signal(wrt);        // writer leaves giving access of the resource to other readers/writers

} while(true);
```

* Explanation:
  * Entry Section
    * Wait for the request to get lined up. After the request gets initiated the writer acquires the `queue` semaphore.
    * Writer requests access to the resource and acquires the `wrt` semaphore on getting access.
    * `queue` semaphore is released by the reader for allowing the next request to be processed.
  * Critical Section
    * Writer writes to the shared resources.
  * Exit Section
    * Writer releases the `wrt` semaphore at the end.