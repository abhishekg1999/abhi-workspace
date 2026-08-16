#if 0
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/kdev_t.h> 
#include <linux/cdev.h>	   
#include <linux/interrupt.h> /* for interrupt functions */
#include <asm/io.h>
#include <asm/hw_irq.h>
#endif

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/hw_irq.h>
#include <linux/err.h>

#define IRQ_NO 11  /* define interrupt number */
dev_t dev=0;	
static struct class *dev_class;
static struct cdev my_cdev;

static int __init my_init(void);
static void __exit my_exit(void);

static int      my_open(struct inode *inode, struct file *file);
static int      my_release(struct inode *inode, struct file *file);
static ssize_t  my_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  my_write(struct file *filp, const char *buf, size_t len, loff_t * off);

/*interrupt handler for IRQ 11. */
static irqreturn_t irq_handler(int irq,void *dev_id) 
{
  	printk(KERN_INFO "Shared IRQ: Interrupt Occurred");
  	return IRQ_HANDLED;
}

static struct file_operations fops=
{
	.owner=THIS_MODULE,
	.read=my_read,
	.write=my_write,
	.open=my_open,
	.release=my_release
};

static int my_open(struct inode *inode,struct file *file)
{
	pr_info("driver open called\n");
	return 0;
}

static int my_release(struct inode *inode,struct file *file)
{
	pr_info("driver release called \n");
	return 0;
}

static ssize_t my_read(struct file *filp,char __user *buf,size_t len,loff_t *off)
{
	pr_info("driver read called \n");
	asm("int $0x3B");  /* trigger irq 11 when this function called*/
	return 0;
}

static ssize_t my_write(struct file *filp,const char __user *buf,size_t len,loff_t *off)
{
	pr_info("driver write function called \n");
	return len;
}

static int __init my_init(void)
{
	
	if((alloc_chrdev_region(&dev,0,1,"my_device"))<0){ 
		pr_info("Cannot allocate major number for driver 1\n");
                return -1;
	} 

	pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
	cdev_init(&my_cdev,&fops);

	if((cdev_add(&my_cdev,dev,1))<0){
		pr_err("connot add device to system \n");
		goto r_class;
	}

	dev_class=class_create(THIS_MODULE,"my_class");
	if(IS_ERR(dev_class)){
		pr_err("connot create the struct class for the device\n");
		goto r_class;
	}

	if(IS_ERR(device_create(dev_class,NULL,dev,NULL,"my_device"))){  
		pr_err("Cannot create the Device file for device\n");
            	goto r_device;
	}

	/* register interrupt handler */
	if(request_irq(IRQ_NO, irq_handler, IRQF_SHARED, "my_device", (void *)(irq_handler))) {
            printk(KERN_INFO "my_device: cannot register IRQ ");
                    goto irq;
    }

        pr_info("Kernel Module Inserted Successfully...\n");
		return 0;

irq:
		free_irq(IRQ_NO,(void *)(irq_handler));

r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}

static void __exit my_exit(void)
{
	free_irq(IRQ_NO,(void *)(irq_handler)); /* unregister interrupt handler */
	device_destroy(dev_class,dev); 
	class_destroy(dev_class);  
	cdev_del(&my_cdev); 
	unregister_chrdev_region(dev,1);
	pr_info("Kernel Module Removed Successfully...\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("abhishek <abhishekece1234@gmail.com>");
MODULE_DESCRIPTION("Simple linux driver (Dynamically allocating the Major and Minor number)");
MODULE_VERSION("1.1");
