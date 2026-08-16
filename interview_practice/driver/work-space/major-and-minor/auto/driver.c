#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>

dev_t dev=0;

static int __init auto_init(void) {
	
	/*Allocating Major number*/
	int ret = alloc_chrdev_region(&dev, 0, 1, "my_device");
        if (ret < 0) {
                pr_err("Device number registration failed\n");
                return -1;
        }

	pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
        pr_info("Kernel Module Inserted Successfully...\n");
	return 0;
}

static void __exit auto_exit(void) {
	
	unregister_chrdev_region(dev,1);
	pr_info("Kernel Module Removed Successfully...\n");
}

module_init(auto_init);
module_exit(auto_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("abhishek abhishekece1234@gmail.com");
MODULE_DESCRIPTION("Simple linux driver (Dynamically allocating the Major and Minor number)");
MODULE_VERSION("1.1");
