#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

/* creating device number with custom major and minor number */
#define MAJORNO 8  
#define MINORNO 0
dev_t dev = MKDEV(MAJORNO,MINORNO);

/* module init function */
static int __init static_init(void) {
	
	int ret = register_chrdev_region(dev,1,"my_device"); //allocating device number/device id
	if (ret < 0) {
        pr_err("Device number registration failed\n");
        return ret;
        }

	pr_info("Major=%d Monor=%d\n",MAJOR(dev),MINOR(dev));
	pr_info("kernel module inserted successfully..\n");
	return 0;
}

/* module exit function */
static void __exit static_exit(void) {

	unregister_chrdev_region(dev,1); // freeing device number
	pr_info("kernel module removed successfully..\n");
}

module_init(static_init); //When this module is inserted, call static_init()
module_exit(static_exit);  //When this module is removed, call static_exit()

MODULE_LICENSE("GPL");
MODULE_AUTHOR("abhishek <abhishekece1234@gmail.com>");
MODULE_DESCRIPTION("Simple linux driver (Statically allocating the Major and Minor number)");
MODULE_VERSION("1.0");

/*        flow          

          insmod mydriver.ko
                  |
                  V
        static_init()
                  |
                  V
     MKDEV(8,0) creates device number
                  |
                  V
 register_chrdev_region(dev,1,"my_device")
                  |
                  V
      Device number 8:0 reserved
                  |
                  V
      Print major/minor information


-----------------------------------------


          rmmod mydriver
                  |
                  V
          static_exit()
                  |
                  V
 unregister_chrdev_region(dev,1)
                  |
                  V
      Device number released
                  |
                  V
       "module removed" printed

*/

