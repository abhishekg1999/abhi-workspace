#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include <linux/device.h>   /* class_create, device_create */
#include <linux/kdev_t.h>   /* dev_t */

/* 
1. allocate major number and minor number
2. create device class
3. create device file

5. destroy class
6. destroy device file
7. release major and minor number
*/
 
dev_t dev=0;
static struct class *class_ptr;
static struct device *dev_ptr;

static int __init auto_init(void) {

	/*Allocating Major&minor number*/
	int ret = alloc_chrdev_region(&dev, 0, 1, "my_device"); /* add entry in file /proc/devices */
	if( ret<0 ) {
		pr_err("Cannot allocate major number\n");
                return -1;
	} 

	/*Creating device class*/
	class_ptr=class_create("my_class");  /* add entry to /sys/class/my_class */
	if(IS_ERR(class_ptr)){
		pr_err("connot create the struct class\n");
		goto r_class;
	}

	/*Creating device*/
	dev_ptr = device_create(class_ptr, NULL, dev, NULL, "my_device"); /* add entry to /dev/my_device */
	if(IS_ERR(dev_ptr)) {
		pr_err("Cannot create the Device file\n");
            	goto r_device;
	}
	
	pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
        pr_info("Kernel Module Inserted Successfully...\n");
	return 0;

r_class:
        unregister_chrdev_region(dev,1);
r_device:
        class_destroy(class_ptr);
        return -1;
}

static void __exit auto_exit(void) {

	device_destroy(class_ptr,dev); /* delete/destroy device file */
	class_destroy(class_ptr);  	/* delete/destroy struct class */
	unregister_chrdev_region(dev,1);  /* release major&minor number */
	pr_info("Kernel Module Removed Successfully...\n");
}

module_init(auto_init);
module_exit(auto_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("abhishek <abhishekece1234@gmail.com>");
MODULE_DESCRIPTION("Simple linux driver (Dynamically allocating the Major and Minor number)");
MODULE_VERSION("1.1");
