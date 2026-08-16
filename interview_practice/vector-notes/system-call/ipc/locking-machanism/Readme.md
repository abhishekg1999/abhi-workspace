## Easy way to remember
                THREADS

Mutex
Spinlock
Semaphore
                PROCESSES

Semaphore
Record Lock
Mutex (Shared Mutex)

Mutex → Mainly thread synchronization (can also synchronize processes if process-shared).
Spinlock → Thread synchronization (especially in kernel code).
Semaphore → Both thread and process synchronization.
Record lock → Process synchronization for access to shared files.
