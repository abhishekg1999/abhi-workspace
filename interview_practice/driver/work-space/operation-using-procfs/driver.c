#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include<linux/uaccess.h>              /* copy_to/from_user() */
#include<linux/proc_fs.h>     /* proc_mkdir() & proc_create() */ 
#include <linux/err.h>
#define LINUX_KERNEL_VERSION  504
 
char etx_array[20]="try_proc_array";  /* kernel buffer */ 
dev_t dev = 0;
static struct proc_dir_entry *parent;
/*
** Function Prototypes
*/
static int      __init etx_driver_init(void);
static void     __exit etx_driver_exit(void);
/***************** Procfs Functions *******************/
static int      open_proc(struct inode *inode, struct file *file);
static int      release_proc(struct inode *inode, struct file *file);
static ssize_t  read_proc(struct file *filp, char __user *buffer, size_t length,loff_t * offset);
static ssize_t  write_proc(struct file *filp, const char *buff, size_t len, loff_t * off);

#if ( LINUX_KERNEL_VERSION > 505 )
/*
** procfs operation sturcture
*/
static struct proc_ops proc_fops = {
        .proc_open = open_proc,
        .proc_read = read_proc,
        .proc_write = write_proc,
        .proc_release = release_proc
};

#else //LINUX_KERNEL_VERSION > 505
/*
** procfs operation sturcture
*/
static struct file_operations proc_fops = {
        .open = open_proc,
        .read = read_proc,
        .write = write_proc,
        .release = release_proc
};

#endif //LINUX_KERNEL_VERSION > 505
/*
** This function will be called when we open the procfs file
*/
static int open_proc(struct inode *inode, struct file *file)
{
    pr_info("proc file opend.....\t");
    return 0;
}

/*
** This function will be called when we close the procfs file
*/
static int release_proc(struct inode *inode, struct file *file)
{
    pr_info("proc file released.....\n");
    return 0;
}

/*
** This function will be called when we read the procfs file
*/
static ssize_t read_proc(struct file *filp, char __user *buffer, size_t length,loff_t * offset)
{

    	if( copy_to_user(buffer,etx_array,20) ) /* copy to buffer(app) from etx_array(kernel) */
    	{
        	pr_err("Data Send : Err!\n");
    	}
    	pr_info("proc file reading is done ..\n");
    	return length;
}

/*
** This function will be called when we write the procfs file
*/
static ssize_t write_proc(struct file *filp, const char *buff, size_t len, loff_t * off)
{   
	if( copy_from_user(etx_array,buff,len) ) /* copy from etx_array(kernel) to buff(app) */
    	{
        	pr_err("Data Write : Err!\n");
    	}
     	pr_info("proc file writing is done..\n");
   	return len;
}

/*
** Module Init function
*/
static int __init etx_driver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
                pr_info("Cannot allocate major number\n");
                return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
        
        /*Create proc directory. It will create a directory under "/proc" */
        parent = proc_mkdir("etx",NULL);
  
        
        /*Creating Proc entry under "/proc/etx/" */
        proc_create("etx_proc", 0666, parent, &proc_fops);
 
        pr_info("Device Driver Insert...Done!!!\n");
        return 0;
}
 
/*
** Module exit function
*/
static void __exit etx_driver_exit(void)
{
        /* Removes single proc entry */
        //remove_proc_entry("etx/etx_proc", parent);
        
        /* remove complete /proc/etx */
        proc_remove(parent);
        unregister_chrdev_region(dev, 1);
        pr_info("Device Driver Remove...Done!!!\n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("abhishek <abhishekece1234@gmail.com>");
MODULE_DESCRIPTION("Simple Linux device driver (procfs)");
MODULE_VERSION("1.6");
