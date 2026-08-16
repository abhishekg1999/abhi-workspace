#if 0
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
#include <linux/mutex.h>
#include <linux/err.h>
#endif

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to_user()/copy_from_user()
#include <linux/kthread.h>             //kernel threads
#include <linux/sched.h>               //task_struct
#include <linux/delay.h>
#include <linux/err.h>

DEFINE_MUTEX(lock);
unsigned long global_var=0;

static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);

static struct task_struct *thread1_ptr;
static struct task_struct *thread2_ptr;

int thread_function1(void *pv);
int thread_function2(void *pv);
/*
** Thread function 1
*/
int thread_function1(void *pv)
{

    while(!kthread_should_stop()) {
        mutex_lock(&lock);  /* mutex aquired */
        global_var++;
        pr_info("In Thread Function1 %lu\n",global_var);
        mutex_unlock(&lock); /* mutex released */
        msleep(1000);
    }
    return 0;
}
/*
** Thread function 2
*/
int thread_function2(void *pv)
{
    while(!kthread_should_stop()) {
        mutex_lock(&lock);  /* mutex aquired */
        global_var++;
        pr_info("In Thread Function2 %lu\n",global_var);
        mutex_unlock(&lock); /* mutex released */
        msleep(1000);
    }
    return 0;
}

static int __init etx_driver_init(void)
{
        /* Creating Thread 1 */
        thread1_ptr = kthread_run(thread_function1,NULL,"eTx Thread1");
        if(thread1_ptr)
            pr_err("Kthread1 Created Successfully...\n");
        else
            pr_err("Cannot create kthread1\n");

         /* Creating Thread 2 */
        thread2_ptr = kthread_run(thread_function2,NULL,"eTx Thread2");
        if(thread2_ptr)
            pr_err("Kthread2 Created Successfully...\n");
        else
            pr_err("Cannot create kthread2\n");

        pr_info("Device Driver Insert...Done!!!\n");
        return 0;
}

static void __exit etx_driver_exit(void)
{
        kthread_stop(thread1_ptr);
        kthread_stop(thread2_ptr);
        pr_info("Device Driver Remove...Done!!\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abhishek <abhishekece1234@gmail.com>");
MODULE_DESCRIPTION("A simple device driver - Mutex");
MODULE_VERSION("1.17");
