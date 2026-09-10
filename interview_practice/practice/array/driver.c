
dev_t dev = MKDEV(8, 0);
register_chrdev_region(dev, 1, "my-device");
unregister_chrdev_region(dev, 1);

dev_t dev;
alloc_chrdev_region(&dev, 0, 1, "my-device");
unregister_chrdev_region(dev, 1);

static struct class *class_ptr;
static struct device *device_ptr;
static struct cdev cdev_var;

static struct file_operations fops = {

	.owner = THIS_MODULE,
	.read = my_read,
	.write = my_write,
	.open = my_open,
	.release = my_release
}

alloc_chdev_region(&dev, 0, 1, "my-device");
cdev_init(&cdev_var, &fops);
cdev_add(&cdev_var, dev, 1);
class_ptr = create_class("my-class");
device_ptr = create_device(class_ptr, NULL, dev, NULL, "my-device");

device_destroy(class_ptr, dev);
class_destroy(class_ptr);
cdev_del(cdev_var);
unregister_chrdev_region(dev, 1);

