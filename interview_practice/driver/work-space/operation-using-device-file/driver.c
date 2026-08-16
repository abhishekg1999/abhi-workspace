#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>  /* for device_create */
#include <linux/kdev_t.h>  /* for class_create */
#include <linux/cdev.h>	   /* for cdev_add & cdev_init */
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/string.h>

#define mem_size 1024
dev_t dev=0;
static struct class *class_ptr;
static struct device *device_ptr;
static struct cdev my_cdev;
static uint8_t *kbuf;

static int __init my_init(void);
static void __exit my_exit(void);
static int      my_open(struct inode *inode, struct file *file);
static int      my_release(struct inode *inode, struct file *file);
static ssize_t  my_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t  my_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);

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
static ssize_t my_read(struct file *filp,char __user *ubuf,size_t len,loff_t *off)
{
	// buf >  pointer to user buffer
	// kernel_buf > pointer to kernel buffer
	if(copy_to_user(ubuf,kbuf,mem_size)){    /* copy to user space from kernel space */
		return -EFAULT;
	}

	pr_info("Data Read : Done!\n");
        return mem_size;
}

/*
** This function will be called when we write the Device file
*/
static ssize_t my_write(struct file *filp,const char __user *ubuf,size_t len,loff_t *off) {
	
	if (len > mem_size)
        	len = mem_size;

	if(copy_from_user(kbuf,ubuf,len)){    /* copy to kernel space from user space */
		return -EFAULT;
	}
	
	pr_info("Data Write : Done!\n");
	return len;
}

static int __init my_init(void)
{
	/*Allocating Major number*/
	if((alloc_chrdev_region(&dev,0,1,"my_device"))<0){  /* entry in /proc/devices/ */
		pr_info("Cannot allocate major number for driver 1\n");
                return -1;
	} 
	
	/*Creating cdev structure*/
	cdev_init(&my_cdev,&fops);

	/*Adding character device to the system*/
	if((cdev_add(&my_cdev,dev,1))<0){
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
	device_ptr = device_create(class_ptr,NULL,dev,NULL,"my_device");
	if(IS_ERR(device_ptr)){  /* entry in /dev/auto_device */
		pr_err("Cannot create the Device\n");
            	goto r_device;
	}

	/*Creating Physical memory*/
	kbuf = kmalloc(mem_size,GFP_KERNEL);
	if(!kbuf){
		pr_info("cannot allocate mr to kernel\n");
		goto r_device;
	}

	/* copy default data to kernel_buf */
	strcpy(kbuf,"hii abhishek");
	
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
        kfree(kbuf);
	device_destroy(class_ptr,dev); /* delete device file/destroy created device */
	class_destroy(class_ptr);  	/* delete struct class/destroy created class */
	cdev_del(&my_cdev); 		/* delete cdev struct instance/variable  */ 
	unregister_chrdev_region(dev,1);  /* release major&minor number */
	pr_info("Kernel Module Removed Successfully...\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("abhishek <abhishekece1234@gmail.com>");
MODULE_DESCRIPTION("Simple linux driver (Dynamically allocating the Major and Minor number)");
MODULE_VERSION("1.1");
