#compile using threads
$cc creat-threads.c -pthread -o p1  #create p1 executalbe after linking with pthread 


## difference between mutex and spinlock:
The main difference is how they wait for a locked resource. A mutex puts another thread to sleep, while a spinlock keeps the another thread awake and spinning in a loop until the lock opens.

FeatureMutex 		(Mutual Exclusion)		Spinlock
Waiting Behavior	Sleeps (yields the CPU)		Spins (stays active on the CPU)
CPU Usage		Low while waiting		High(100%) while waiting
Best 			ForLong waiting times		Very short waiting times
Overhead		High (takes time to wake up)	Low (reacts instantly)
Context Switch		Yes (swaps threads)		No (keeps the same thread)


## types of mutex 

1. static (No init need to call init and destroy function)

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_lock(&lock);
pthread_mutex_unlock(&lock);


2. dynamic (must call init and destroy function)

pthread_mutex_t lock;
pthread_mutex_init(&lock, 0);

pthread_mutex_lock(&lock);
pthread_mutex_unlock(&lock);

pthread_mutex_destroy(&lock);


## types spin lock

it only support dynamic , No static support  

pthread_spin_t lock;
pthread_spin_init(&lock, 0);

pthread_spin_lock(&lock);
pthread_spin_unlock(&lock);

pthread_spin_destroy(&lock);


