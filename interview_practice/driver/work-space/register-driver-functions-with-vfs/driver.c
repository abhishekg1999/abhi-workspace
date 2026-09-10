/* simple driver for registering file operation to driver operation */
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include <linux/device.h>  /* for device_create */
#include <linux/kdev_t.h>  /* for class_create */
#include <linux/cdev.h>	   /* for cdev_add & cdev_init */


/*
1. create major and minor number
2. define driver functions and register with file operation structure
3. initialize char device structure
4. register char device with kernel
5. create device class
7. create device file/node

8. destroy device file 
9. destroy device class
10. remove device from kernel
*/

dev_t dev;	/* device Id */
static struct cdev cdev_var;	/* for cdev_init(),cdev_add() */
static struct class *class_ptr; /* for class_create() */
static struct device *dev_ptr;  /* for device_create() */

static int __init my_init(void);
static void __exit my_exit(void);

static int      my_open(struct inode *inode, struct file *file);
static int      my_release(struct inode *inode, struct file *file);
static ssize_t  my_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  my_write(struct file *filp, const char *buf, size_t len, loff_t * off);

/* register All created functions with file operations structure */
static struct file_operations fops=
{
	.owner=THIS_MODULE,
	.read=my_read,
	.write=my_write,
	.open=my_open,
	.release=my_release
};

/*
** This function will be called when we open the Device file
open() --> sys_open() --> my_open()
*/
static int my_open(struct inode *inode,struct file *file)
{
	pr_info("driver open called\n");
	return 0;
}

/*
** This function will be called when we close the Device file
close() --> sys_close() --> my_release()
*/
static int my_release(struct inode *inode,struct file *file)
{
	pr_info("driver release called \n");
	return 0;
}

/*
** This function will be called when we read the Device file
read() --> sys_read() --> my_read()
*/
static ssize_t my_read(struct file *filp,char __user *buf,size_t len,loff_t *off)
{
	pr_info("driver read called \n");
	return 0;
}

/*
** This function will be called when we write the Device file
write() --> sys_write()  --> my_write()
*/
static ssize_t my_write(struct file *filp,const char __user *buf,size_t len,loff_t *off)
{
	pr_info("driver write function called \n");
	return len;
}

static int __init my_init(void)
{
	/*Allocating Major number*/
	if(alloc_chrdev_region(&dev,0,1,"my_device") <0){	/* entry in /proc/devices/ */
		pr_info("Cannot allocate major number for driver 1\n");
                return -1;
	} 

	/*create cdev instance and fill with driver supported fuctions*/
	cdev_init(&cdev_var,&fops);

	/*Adding character device to the system*/
	if(cdev_add(&cdev_var,dev,1) <0){
		pr_err("connot add device to system \n");
		goto r_class;
	}

	/*Creating struct class*/
	class_ptr=class_create("my_class");  /* entry in /sys/class/my_class */
	if(IS_ERR(class_ptr)){
		pr_err("can not create the struct class for the device\n");
		goto r_class;
	}

	/*Creating device*/
	dev_ptr = device_create(class_ptr,NULL,dev,NULL,"my_device");
	if(IS_ERR(dev_ptr)){  /* entry in /dev/my_device */
		pr_err("Can not create the Device file for device\n");
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
	cdev_del(&cdev_var); 		/* removes the device when the module is unloaded */
	unregister_chrdev_region(dev,1);  /* release major&minor number */
	pr_info("Kernel Module Removed Successfully...\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("abhishek <abhishekece1234@gmail.com>");
MODULE_DESCRIPTION("Simple linux driver (Dynamically allocating the Major and Minor number)");
MODULE_VERSION("1.1");
