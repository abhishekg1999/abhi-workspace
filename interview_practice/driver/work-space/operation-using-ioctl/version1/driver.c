#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include <linux/device.h>   /* class_create, device_create */
#include <linux/kdev_t.h>   /* dev_t */
#include <linux/cdev.h>	   /* for cdev_add & cdev_init */
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/string.h>
#include <linux/ioctl.h>   /* _IOC_SIZE */

/* Make sure these match your user space app exactly */
#define WR_VALUE _IOW('a', 'a', char[1024])
#define RD_VALUE _IOR('a', 'b', char[1024])
#define mem_size 1024

static struct cdev cdev_var;
static struct class *class_ptr;
static struct device *dev_ptr;
dev_t dev;
static uint8_t *kbuf;

static int my_open(struct inode *inode, struct file *file);
static int my_release(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t my_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);
static long my_ioctl(struct file *filp, unsigned int cmd, unsigned long ubuf);

static struct file_operations fops = {

	.owner=THIS_MODULE,
	.read=my_read,
	.write=my_write,
	.open=my_open,
	.unlocked_ioctl=my_ioctl,
	.release=my_release
};

static int my_open(struct inode *inode, struct file *file){

	pr_info("driver pen called\n");
	return 0;
}

static int my_release(struct inode *inode, struct file *file){

	pr_info("drive release called\n");
	return 0;
}

static ssize_t my_read(struct file *filp, char __user *ubuf, size_t len, loff_t *off){

	if(copy_to_user(ubuf, kbuf, len))
		return -EFAULT;

	pr_info("data read: done\n");
	return 0;
}

static ssize_t my_write(struct file *filp, const char __user *ubuf, size_t len, loff_t *off){

	if(len>mem_size)
		len=mem_size;

	if(copy_from_user(kbuf, ubuf, len))
		return -EFAULT;

	pr_info("data write: done\n");
	return 0;
}

static long my_ioctl(struct file *filp, unsigned int cmd, unsigned long buf){

	char __user *ubuf = (char __user *)buf;
	
	switch(cmd){

		case WR_VALUE:
			 /* Use _IOC_SIZE(cmd) to automatically extract '1024' from the macro */
			if(copy_from_user(kbuf, ubuf, _IOC_SIZE(cmd)))
				return -EFAULT;

			pr_info("write :done\n");
			break;

		case RD_VALUE:
			if(copy_to_user(ubuf, kbuf, _IOC_SIZE(cmd)))
                                return -EFAULT;

                        pr_info("read :done\n");
                        break;
	}

	return 0;
}

static int __init auto_init(void){

	int ret=alloc_chrdev_region(&dev, 0, 1, "my_device");
	if(ret<0){
		pr_err("device number allocation failed\n");
		return -1;
	}
	
	cdev_init(&cdev_var, &fops);

	if(cdev_add(&cdev_var, dev, 1) <0){

		pr_err("can not add device to system\n");
		goto r_class;
	}

	class_ptr=class_create("my_class");
	if(IS_ERR(class_ptr)){
		pr_err("can not create class\n");
		goto r_class;
	}

	dev_ptr=device_create(class_ptr, NULL, dev, NULL, "my_device");
	if(IS_ERR(dev_ptr)){
		pr_err("can not create device\n");
		goto r_device;
	}
	
	/*Creating Physical memory*/
	kbuf = kmalloc(mem_size, GFP_KERNEL);
	if(!kbuf){
		pr_info("cannot allocate mr to kernel\n");
		goto r_device;
	}

	/* copy default data to kernel_buf */
	strcpy(kbuf, "hii abhishek");

	pr_info("kernel module inserted successfully:\n");
	return 0;

r_class:
	unregister_chrdev_region(dev, 1);
r_device:
	class_destroy(class_ptr);
	return -1;

}

static void __exit auto_exit(void){

	kfree(kbuf);
	device_destroy(class_ptr, dev);
	class_destroy(class_ptr);
	cdev_del(&cdev_var);
	unregister_chrdev_region(dev, 1);
	pr_info("module removed successfully:\n");
}

module_init(auto_init);
module_exit(auto_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("abhishek <abhishekece1234@gmail.com>");
MODULE_DESCRIPTION("Simple linux driver (Dynamically allocating the Major and Minor number)");
MODULE_VERSION("1.1");
