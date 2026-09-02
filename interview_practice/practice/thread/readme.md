## Quick Comparison 
Mutex:
1. Mutual Exclusion (Locking access to a single shared resource).		
2. Binary state (0 or 1).		
3. Ownership requirement: The exact same thread that locks it must unlock it.

Semaphore:
1. Resource Counting or signaling across different threads.		
2. Integer counter (0 to N).	
3. No Ownership: Any thread can signal (increment) or wait (decrement.

Condition Variable:
1. State Signaling (Putting threads to sleep until a specific condition becomes true).
2. Event queue triggered by a predicate.
3. Requires a Mutex companion to protect the condition check itself.

## Deep Dive: When to use a Mutex instead of a Semaphore?
- Use a Mutex when your main goal is to protect a shared data structure, variable, or hardware peripheral from corruption caused by multiple threads accessing it at the exact same time (Mutual Exclusion).

- Use a Semaphore when you are counting available spaces/items or when Thread A needs to wake up Thread B.

## Why you shouldn't use a Semaphore as a Mutex (even if you set its count to 1):
1. Ownership Safety: A Mutex has a strict concept of an owner. If Thread 1 locks a mutex, and Thread 2 attempts to maliciously or accidentally call unlock on it, the OS will block the action or throw an error. With a Binary Semaphore, any thread can unlock it at any time. If a bug in Thread 2 calls sem_post(), it breaks the critical section protection for Thread 1 completely.

2. Priority Inversion Protection: Modern operating systems equip Mutexes with Priority Inheritance Protocols. If a low-priority thread locks a mutex, and a high-priority thread tries to grab it, the OS temporarily boosts the low-priority thread's speed so it can finish up and hand over the lock. Semaphores lack ownership context, meaning they cannot support priority inheritance, which frequently leads to frozen real-time operating systems (RTOS).
 
## Use a Mutex when:
1. You want to safely increment a single global counter variable.
2. You need to update a shared linked list or hash map across threads.
3. You are writing to an external UART or SPI bus in an embedded device.

## Use a Semaphore when:
1. You want to limit access to a pool of resources (e.g., managing access to 3 identical printers).
2. You are writing an Interrupt Service Routine (ISR). An ISR cannot hold a lock (Mutex), but it can fire a semaphore (sem_post) to wake up a background processing thread.

## Use a Condition Variable when:
1. A thread needs to pause until a highly specific logical state occurs (e.g., "Wait until buffer_count == 5 AND system_status == READY").


## If multiple threads access a shared variable, can I simply declare it volatile?

No , Because volatile does not lock the variable, two threads can execute these steps at the exact same time, overlapping each other.

we should use mutex here :
#include <pthread.h>

int counter = 0;
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

// Thread-safe increment
pthread_mutex_lock(&counter_mutex);
counter++;
pthread_mutex_unlock(&counter_mutex);


