/* simple driver for spinlock */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/kthread.h>             //kernel threads
#include <linux/sched.h>               //task_struct 
#include <linux/delay.h>
#include <linux/err.h>

/* prototype of driver init and exit functions */
static int __init my_init(void);
static void __exit my_exit(void);

/* protype of thread functions */
int thread_fn1(void *p1);
int thread_fn2(void *p2);

DEFINE_SPINLOCK(lock);  /* define the spinlock */
static struct task_struct *thread1_ptr,*thread2_ptr;  /* kernel threads pointers */ 
unsigned long count = 0;

int thread_fn1(void *p1) 
{
	while(!kthread_should_stop()) {
        spin_lock(&lock); /* spinlock aquired */
        count++;
        pr_info("In Thread Function1 %lu\n", count);
        spin_unlock(&lock);  /* spinlock released */
        msleep(1000);
    	}
    	
	return 0;
}

int  thread_fn2(void *p2) 
{
	while(!kthread_should_stop()) {
        spin_lock(&lock); /* spinlock aquired */
        count++;
        pr_info("In Thread Function2 %lu\n", count);
        spin_unlock(&lock); /* spinlock released */
        msleep(1000);
    	}
    	
	return 0;
}

static int __init my_init(void)
{        
        /* Creating Thread 1 */
        thread1_ptr = kthread_run(thread_fn1,NULL,"Thread1");
        if(thread1_ptr == 0)
            pr_err("Cannot create kthread1\n");
 
         /* Creating Thread 2 */
        thread2_ptr = kthread_run(thread_fn2,NULL,"Thread2");
        if(thread2_ptr == 0)
            pr_err("Cannot create kthread2\n");
        
        pr_info("Device Driver Insert...Done!!!\n");
        pr_err("Kthread1 Created Successfully...\n");
        pr_err("Kthread2 Created Successfully...\n");
        return 0;
}

static void __exit my_exit(void)
{
        kthread_stop(thread1_ptr);
        kthread_stop(thread2_ptr);
        pr_info("Device Driver Remove...Done!!\n");
}
 
module_init(my_init);
module_exit(my_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abhishek <abhishekece1234@gmail.com>");
MODULE_DESCRIPTION("A simple device driver - Spinlock");
MODULE_VERSION("1.18");
