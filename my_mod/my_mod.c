#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Robert Patison");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

static char *name = "Mod";
module_param(name, charp, 0700);//S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(name, "Name mod");


static int __init my_mod_init(void)
{
	printk(KERN_INFO "Hello World!%s\n", name);
		return 0;
}
static void __exit my_mod_exit(void)
{
	printk(KERN_INFO "Goog bye%s\n", name);
}
module_init(my_mod_init);
module_exit(my_mod_exit);
