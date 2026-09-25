##  if driver function is in use and we want to remove the module than how we will handle it

If a kernel module's functions are currently in use—such as a user-space application holding an open file descriptor to your driver, or your kthreads running their while loops—the kernel provides built-in mechanisms to prevent or safely handle module removal.

1. For Driver Files / Character Devices (Module Reference Counting):
The Linux kernel uses a reference counter (try_module_get and module_put) to track how many entities are using your driver. If a user-space application has opened your driver file (e.g., /dev/my_device), the kernel strictly blocks rmmod from unloading your module.

You handle this automatically when defining your file operations (file_operations structure) by linking it to your module owner:
static const struct file_operations fops = {
    .owner   = THIS_MODULE, /* Increments ref count on open, decrements on close */
    .open    = my_open,
    .read    = my_read,
    .release = my_close,
};

If someone runs rmmod while the file is open, the kernel returns an error: ERROR: Module my_module is in use.


## use of .owner = THIS_MODULE

- By setting .owner = THIS_MODULE, you tell the kernel: "This file operations structure belongs to my module."
- When a program opens the file, the kernel looks at .owner and automatically increments your module's reference count.
- If you type lsmod, you will see the use count go up (e.g., my_module  1).
- If you try to run rmmod my_module right now, the kernel blocks it and throws an error: ERROR: Module my_module is in use.

## macros (MKDEV , MAJOR & MINOR)

MKDEV is a helper macro used in Linux kernel programming to combine a major number and a minor number into a single, unified 32-bit integer called a device number (dev_t).

In the Linux kernel, every hardware device file (like /dev/my_i2c_device) is identified by two identifier numbers:
- Major Number: Identifies the specific driver or subsystem responsible for the device.
- Minor Number: Identifies the specific device instance or channel controlled by that driver (e.g., if you have 3 identical sensors connected, they all share one major number but have minor numbers 0, 1, and 2).

The kernel stores these two numbers together inside a single data type called dev_t. dev_t is a 32-bit integer:
- The top 12 bits hold the Major number and The bottom 20 bits hold the Minor number.

If you ever have a dev_t structure and need to separate it back into individual numbers, the kernel provides two opposite macros:
- MAJOR(dev_t dev): Extracts the major number out of a dev_t.
- MINOR(dev_t dev): Extracts the minor number out of a dev_t.

example :
When you have multiple I2C slave devices controlled by the same driver, they will all share the exact same major number, but each unique device will be assigned its own distinct minor number.

	(slave)			   (driver)	     (major no)     (minor no)		(device node)
Sensor A (Address 0x48)		my_temp_sensor 		240 		0 		/dev/temp_sensor0
Sensor B (Address 0x49)		my_temp_sensor 		240 		1		/dev/temp_sensor1
Sensor C (Address 0x4A)		my_temp_sensor 		240 		2		/dev/temp_sensor2

Efficiency for the Major Number: 
When a user-space application attempts to read from /dev/temp_sensor2, the kernel looks strictly at the Major Number (240). It instantly knows, "Ah, Major 240 belongs to the my_temp_sensor driver module. Route this read request to its file operations!"

Context for the Minor Number: 
Once the request reaches your driver's .read or .open function, your driver looks at the Minor Number (2) to understand which specific chip the user is talking to

How to Extract the Minor Number inside your Driver:
static int my_driver_open(struct inode *inode, struct file *file)
{
    unsigned int minor = iminor(inode);
    
    pr_info("User opened device instance with minor number: %d\n", minor);
    
    /* 
     * You can now use this 'minor' variable as an array index 
     * to look up the correct physical I2C client address struct!
     */
     
    return 0;
}

## usage of module_init() and module_exit()

#1 module_init() — The Setup Phase:
This macro registers your module's setup/initialization function. The moment you type sudo insmod my_driver.ko or modprobe, the kernel jumps directly to the function named inside this macro.

- Purpose: 
Allocating memory, registering character device major/minor numbers, setting up spinlocks, creating kthreads, or registering an I2C driver structure.
- Return Value: 
It must return 0 if everything succeeded. If it returns a negative error code (like -ENOMEM or -EINVAL), the kernel aborts loading and automatically unloads the module.

#2 module_exit() — The Cleanup Phase:
This macro registers your module's teardown/cleanup function. The moment you type sudo rmmod my_driver or modprobe -r, the kernel jumps directly to this function.

- Purpose: 
Undoing everything done in the initialization phase. You must free any allocated memory, unregister device structures, delete /dev/ files, and gracefully stop background kthreads.
- Consequence of missing it: 
If you do not include a module_exit() macro in your code, the kernel marks your module as permanently loaded. You will not be allowed to remove it until you reboot the entire system.


## What do __init and __exit mean?

__init: 
Tells the kernel that this function is only executed once at startup. Once the initialization finishes, the kernel safely throws away this function's code from RAM to free up memory.

__exit: 
Tells the kernel that if this driver is built directly into the core kernel binary (not as a separate .ko file), this function can be completely ignored because a built-in driver can never be removed anyway.

## how driver s APIs called from user space

when user call "open" bellow call happen to reach to my_open():

Flow:
open() --> sys_open() --> vfs_open() --> my_open()

explanation:
open():  Represents the application initiating the request in User Space.
sys_open():  Represents the kernel's system call entry wrapper entrypoint.
vfs_open():  Represents the Virtual File System Layer, which translates standard files into character device file definitions and performs the internal major/minor number table lookup. 
my_open(): Represents the entry point to your custom hardware driver implementation.



