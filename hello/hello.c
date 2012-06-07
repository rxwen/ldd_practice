#include    <linux/init.h>
#include    <linux/module.h>
#include    <linux/fs.h>
#include    <linux/cdev.h>

static char* whom = "world";
int major = 0, minor = 0;
int device_count = 0;
struct cdev hello_dev;

module_param(whom, charp, S_IRUGO);

loff_t hello_llseek(struct file *filp, loff_t off, int whence) {
    return 0;
}

static int hello_open(struct inode *inode, struct file *filp) {
    printk(KERN_ALERT "hello module (major: %d) is opened\n", major);
    return 0;
}

int hello_release(struct inode *inode, struct file *filp) {
    printk(KERN_ALERT "hello module (major: %d) is closed\n", major);
    return 0;
}

ssize_t hello_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
    return 0;
}

ssize_t hello_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
    printk(KERN_ALERT "hello module write (%08x): %s\n", buf,
            buf);
    return count;
}

struct file_operations hello_fops = {
	.owner =    THIS_MODULE,
    /*.llseek =   hello_llseek,*/
    .read =     hello_read,
    .write =    hello_write,
    /*.ioctl =    hello_ioctl,*/
	.open =     hello_open,
	.release =  hello_release
};

static int hello_init(void) {
    int rc = 0;
    dev_t dev = 0;
    printk(KERN_ALERT "hello %s\n", whom);
    
    rc = alloc_chrdev_region(&dev, minor, device_count, "hello");
    if (rc < 0) {
        printk(KERN_ALERT "can't get major for hello module\n");
    }
    major = MAJOR(dev);
    printk(KERN_ALERT "hello module's major number is %d\n", major);
    cdev_init(&hello_dev, &hello_fops);
    rc = cdev_add(&hello_dev, dev, 1);
    if (rc < 0) {
        printk(KERN_ALERT "can't add  device hello module\n");
    }

    return 0;
}

static void hello_exit(void) {
	dev_t devno = MKDEV(major, minor);
	unregister_chrdev_region(devno, device_count);
    printk(KERN_ALERT "bye, hello module\n");
    cdev_del(&hello_dev);
}

module_init(hello_init);
module_exit(hello_exit);
