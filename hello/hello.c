#include    <linux/init.h>
#include    <linux/module.h>
#include    <linux/fs.h>
#include    <linux/cdev.h>
#include    <linux/proc_fs.h>
#include    <linux/poll.h>
#include    <linux/sched.h>

static char* whom = "world";
int major = 0, minor = 0;
#define DEVICE_COUNT 3
struct cdev hello_dev[DEVICE_COUNT];
wait_queue_head_t inq;       /* write queue */
int data_len = 0;   /* the data available for reading */

module_param(major, int, S_IRUGO);
module_param(whom, charp, S_IRUGO);

loff_t hello_llseek(struct file *filp, loff_t off, int whence) {
    return 0;
}

static int hello_open(struct inode *inode, struct file *filp) {
    printk(KERN_ALERT "hello module (major: %d, minor: %d) is opened\n", 
            major, iminor(inode));
    return 0;
}

int hello_release(struct inode *inode, struct file *filp) {
    printk(KERN_ALERT "hello module (major: %d) is closed\n", major);
    return 0;
}

ssize_t hello_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
    int read_len = data_len;
    data_len = 0;
    return read_len;
}

ssize_t hello_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
    printk(KERN_ALERT "hello module write (%08x): %s\n", buf, buf);
    data_len += count;
	wake_up_interruptible(&inq);  /* awake select() */
    return count;
}

static unsigned int hello_poll(struct file *filp, poll_table *wait) {
    unsigned int mask = 0;
    printk(KERN_ALERT "hello poll begin\n", major);
    poll_wait(filp, &inq, wait);
    if(data_len > 0)
        mask |= POLLIN | POLLRDNORM;
    printk(KERN_ALERT "hello poll end\n", major);
    return mask;
}

struct file_operations hello_fops = {
	.owner =    THIS_MODULE,
    .llseek =   hello_llseek,
    .read =     hello_read,
    .write =    hello_write,
	.poll =		hello_poll,
    /*.ioctl =    hello_ioctl,*/
	.open =     hello_open,
	.release =  hello_release
};

int hello_read_proc(char *page, char **start, off_t offset, int count,
        int *eof, void *data) {
    printk(KERN_ALERT "run hello_read_proc\n");
    int len = 0;
    len = sprintf(page, "  info of hello module exported to /proc\n");
    return len;
}

static int hello_init(void) {
    int rc = 0, i = 0;
    dev_t dev = 0;
    struct proc_dir_entry* proc_entry = NULL;
    printk(KERN_ALERT "hello %s\n", whom);
    
    if(major) { // use fixed major number
		dev = MKDEV(major, minor);
		rc = register_chrdev_region(dev, DEVICE_COUNT, "hello");
        if(rc < 0)
            return -1;
    }
    else { // use dynamic major number
        rc = alloc_chrdev_region(&dev, minor, DEVICE_COUNT, "hello");
        if (rc < 0) {
            printk(KERN_ALERT "can't get major for hello module\n");
            return -1;
        }
        major = MAJOR(dev);
        minor = MINOR(dev);
    }
    printk(KERN_ALERT "hello module's major number is %d, minor is %d\n", major, minor);

    for(i = 0; i < DEVICE_COUNT; ++i) {
		dev = MKDEV(major, minor+i);
        cdev_init(&hello_dev[i], &hello_fops);
        rc = cdev_add(&hello_dev[i], dev, 1);
    }
    if (rc < 0) {
        printk(KERN_ALERT "can't add  device hello module\n");
    }
    
    proc_entry = create_proc_read_entry("hello_proc_entry",
            0, NULL, hello_read_proc, NULL);
    printk(KERN_ALERT "create /proc entry for hello module\n");

    printk(KERN_ALERT "init inq");
	init_waitqueue_head(&inq);

    return 0;
}

static void hello_exit(void) {
    int i = 0;
	dev_t devno = MKDEV(major, minor);
	unregister_chrdev_region(devno, DEVICE_COUNT);
    printk(KERN_ALERT "bye, hello module\n");
    for(i = 0; i < DEVICE_COUNT; ++i)
        cdev_del(&hello_dev[i]);
    remove_proc_entry("hello_proc_entry", NULL);
}

module_init(hello_init);
module_exit(hello_exit);
