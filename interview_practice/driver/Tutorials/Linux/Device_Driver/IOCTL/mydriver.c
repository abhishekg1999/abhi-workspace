#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to()/from_user()
#include <linux/ioctl.h>
#include <linux/err.h>

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

int32_t value=0;
dev_t dev=0;
static struct class *ptr_class;
static struct cdev var_cdev;

/*
** Function Prototypes
*/
static int      __init my_driver_init(void);
static void     __exit my_driver_exit(void);
static int      my_open(struct inode *inode, struct file *file);
static int      my_release(struct inode *inode, struct file *file);
static ssize_t  my_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  my_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static long     my_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

/*
** File operation sturcture
*/

static struct file_operations fops=
{
	.owner=THIS_MODULE,
	.read=my_read,
	.write=my_write,
	.open=my_open,
	.unlocked_ioctl=my_ioctl,
	.release=my_release,
};

/*
** This function will be called when we open the Device file
*/
static int my_open(struct inode *inode,struct file *file)
{
	pr_info("Device File Opened...!!!\n");
	return 0;
}

/*
** This function will be called when we close the Device file
*/
static int my_release(struct inode *inode,struct file *file)
{
	pr_info("Device File Closed...!!!\n");
	return 0;
}

/*
** This function will be called when we read the Device file
*/
static ssize_t my_read(struct file *filp,char __user *buf,size_t len,loff_t * off)
{
	pr_info("Read Function\n");
	return 0;
}

/*
** This function will be called when we write the Device file
*/
static ssize_t my_write(struct file *filp,const char *buf,size_t len,loff_t * off)
{
	pr_info("Write function\n");
	return len;
}

/*
** This function will be called when we write IOCTL on the Device file
*/
static long my_ioctl(struct file *file,unsigned int cmd,unsigne long arg)
{
	switch(cmd){
		case WR_VALUE:
			if(copy_from_user(&value,(int32_t*)arg,sizeof(value))){
				pr_err("Data Write : Err!\n");
			}
			pr_info("Value = %d\n", value);
			break;
		case RD_VALUE:
			if( copy_to_user((int32_t*) arg, &value, sizeof(value)) ){
				pr_err("Data Read : Err!\n");
			}
			break;
		default:
			pr_info("Default\n");
			break;
		}

	return 0;
}

/*
** Module Init function
*/
static int __init my_driver_init(void)
{
	/*Allocating Major number*/
	if((alloc_chrdev_region(&dev,0,1."my_device"))<0){
		pr_err("Cannot allocate major number\n");
		return -1;
	}

	pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
	/*Creating cdev structure*/
	cdev_init(&var_cdev,&fops);

	/*Adding character device to the system*/
	if((cdev_add(&var_cdev,dev,1))<0){
		pr_err("connot add the device to the sytem\n");
		goto r_class;
	}

}
