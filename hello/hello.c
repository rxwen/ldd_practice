#include    <linux/init.h>
#include    <linux/module.h>
#include    <linux/fs.h>
#include    <linux/cdev.h>

static char* whom = "world";
static int howmany = 1;
int major = 0, minor = 0;
struct cdev hello_dev;

module_param(howmany, int, S_IRUGO);
module_param(whom, charp, S_IRUGO);

static int hello_init(void) {
    int rc = 0;
    dev_t dev = 0;
    printk(KERN_ALERT "hello %s\n", whom);
    
    rc = alloc_chrdev_region(&dev, minor, 1, "hello");
    if (rc < 0) {
        printk(KERN_ALERT "can't get major for hello module\n");
    }
    major = MAJOR(dev);
    printk(KERN_ALERT "hello module's major number is %d\n", major);
	/*cdev_init(&hello_dev, &hello_fops);*/

    return 0;
}

static void hello_exit(void) {
	dev_t devno = MKDEV(major, minor);
	unregister_chrdev_region(devno, 1);
    printk(KERN_ALERT "bye, hello module\n");
    /*cdev_del(&hello_dev);*/
}

module_init(hello_init);
module_exit(hello_exit);
