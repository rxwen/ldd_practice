#include    <linux/init.h>
#include    <linux/module.h>

static char* whom = "world";
static int howmany = 1;

module_param(howmany, int, S_IRUGO);
module_param(whom, charp, S_IRUGO);

static int hello_init(void) {
    printk(KERN_ALERT "hello %s\n", whom);
    return 0;
}

static void hello_exit(void) {
    printk(KERN_ALERT "bye, world\n");
}

module_init(hello_init);
module_exit(hello_exit);
