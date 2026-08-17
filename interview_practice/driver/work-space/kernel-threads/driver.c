/* simple kernel thread driver */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kthread.h>             //kernel threads
#include <linux/sched.h>               //task_struct 
#include <linux/delay.h>
#include <linux/err.h>

/* init exit function prototype */
static int __init my_init(void);
static void __exit my_exit(void);

static struct task_struct *thread_ptr; /* thread pointer define */
int thread_function(void *pv);   /* thread function prototype */

int thread_function(void *pv)
{
    int i=0;
    while(!kthread_should_stop()) {
        pr_info("In Thread Function %d\n", i++);
        msleep(1000);
    }
    return 0;
}

/* driver init function */
static int __init my_init(void)
{
	
        /* You can use this method also to create and run the thread */
        thread_ptr = kthread_run(thread_function, NULL, "hello");
        if(thread_ptr == 0)
            pr_err("Cannot create kthread\n");

	pr_info("Kthread Created Successfully...\n");      
        pr_info("Device Driver Inserted...!!!\n");
        return 0;
}

/* driver exit function */
static void __exit my_exit(void)
{
	kthread_stop(thread_ptr);
	pr_info("Device Driver Removed..!!\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abhishek <abhishekece1234@gmail.com>");
MODULE_DESCRIPTION("A simple device driver - Kernel Thread");
MODULE_VERSION("1.14");
