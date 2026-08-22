#compile using threads
$cc creat-threads.c -pthread -o p1  #create p1 executalbe after linking with pthread 


## difference between mutex and spinlock:
The main difference is how they wait for a locked resource. A mutex puts a thread to sleep, while a spinlock keeps the thread awake and spinning in a loop until the lock opens.

FeatureMutex 		(Mutual Exclusion)		Spinlock
Waiting Behavior	Sleeps (yields the CPU)		Spins (stays active on the CPU)
CPU Usage		Low while waiting		High(100%) while waiting
Best 			ForLong waiting times		Very short waiting times
Overhead		High (takes time to wake up)	Low (reacts instantly)
Context Switch		Yes (swaps threads)		No (keeps the same thread)



