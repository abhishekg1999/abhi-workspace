#include<linux/kernel.h>
#include<linux/init.h>	/* for __init & __exit macro */
#include<linux/module.h>
#include<linux/kdev_t.h> /* for dev_t */
#include<linux/fs.h>
#include <linux/device.h>  /* for device_create */
#include <linux/kdev_t.h>  /* for class_create */
#include <linux/cdev.h>	   /* for cdev_add & cdev_init */
#include <linux/ioctl.h>   /* for ioctl */
#include<linux/uaccess.h>  /* copy_to/from_user() */

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

dev_t dev;
static struct cdev cdev_var;
static struct class *class_ptr;
static struct device *device_ptr;
int32_t value;

static int __init my_init(void);
static void __exit my_exit(void);
static int      my_open(struct inode *inode, struct file *file);
static int      my_release(struct inode *inode, struct file *file);
static ssize_t  my_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  my_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static long my_ioctl(struct file *file,unsigned int cmd,unsigned long buf);

static struct file_operations fops=
{
	.owner=THIS_MODULE,
	.read=my_read,
	.write=my_write,
	.open=my_open,
	.unlocked_ioctl=my_ioctl,
	.release=my_release
};

/*
** This function will be called when we open the Device file
*/
static int my_open(struct inode *inode,struct file *file)
{
	pr_info("driver open called\n");
	return 0;
}

/*
** This function will be called when we close the Device file
*/
static int my_release(struct inode *inode,struct file *file)
{
	pr_info("driver release called \n");
	return 0;
}

/*
** This function will be called when we read the Device file
*/
static ssize_t my_read(struct file *filp,char __user *buf,size_t len,loff_t *off)
{
	pr_info("driver read called \n");
	return 0;
}

/*
** This function will be called when we write the Device file
*/
static ssize_t my_write(struct file *filp,const char __user *buf,size_t len,loff_t *off)
{
	pr_info("driver write function called \n");
	return len;
}

static long my_ioctl(struct file *file,unsigned int cmd,unsigned long buf)
{
	// value => kernel space variable
	// buf => user space variable which constains adds of variable pass from application
	switch(cmd){
		case WR_VALUE:
			if(copy_from_user(&value,(int32_t*)buf,sizeof(value))) /* copying from buf(user) to value(kernel) */
			{
				pr_err("Data Write : Err!\n");
			}
				pr_info("write done \n");
				break;
		case RD_VALUE:
			if(copy_to_user((int32_t*)buf,&value,sizeof(value))) /* copying from value(kernel) to buf(user) */
			{
				pr_err("Data Read : Err!\n");
			}
				pr_info("read done \n");
				break;
		default:
				pr_info("Default\n");
				break;
		}
		return 0;
}

static int __init my_init(void)
{
	/*Allocating Major number*/
	if((alloc_chrdev_region(&dev,0,1,"my_device"))<0){  /* entry in /proc/devices/ */
		pr_info("Cannot allocate major number for driver 1\n");
                return -1;
	} 
	
	/*Creating cdev structure*/
	cdev_init(&cdev_var,&fops);

	/*Adding character device to the system*/
	if((cdev_add(&cdev_var,dev,1))<0){
		pr_err("connot add device to system \n");
		goto r_class;
	}

	/*Creating struct class*/
	class_ptr=class_create("my_class");  /* entry in /sys/class/auto_class */
	if(IS_ERR(class_ptr)){
		pr_err("connot create the struct class for the device\n");
		goto r_class;
	}

	/*Creating device*/
	device_ptr = device_create(class_ptr,NULL,dev,NULL,"my_device");  /* entry in /dev/auto_device */
	if(IS_ERR(device_ptr)){
		pr_err("Cannot create the Device\n");
            	goto r_device;
	}

	pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
        pr_info("Kernel Module Inserted Successfully...\n");
	return 0;

r_device:
        class_destroy(class_ptr);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}

static void __exit my_exit(void)
{
	device_destroy(class_ptr,dev); /* delete device file/destroy created device */
	class_destroy(class_ptr);  	/* delete struct class/destroy created class */
	cdev_del(&cdev_var); 		/* delete cdev struct instance/variable  */ 
	unregister_chrdev_region(dev,1);  /* release major&minor number */
	pr_info("Kernel Module Removed Successfully...\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("abhishek <abhishekece1234@gmail.com>");
MODULE_DESCRIPTION("Simple linux driver (Dynamically allocating the Major and Minor number)");
MODULE_VERSION("1.1");
