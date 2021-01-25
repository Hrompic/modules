#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/unistd.h>
#include <linux/init.h>
#include <linux/utsname.h>

#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/syscalls.h>
#include <linux/namei.h>

#define MAX_TRY 1024

int flag = 0;
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Robert Patison");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");



//extern void *sys_call_table[];
//extern void sys_call_ptr_t[];
//extern const sys_call_ptr_t sys_call_table[];
unsigned long *sys_call_table;
static int uid;

module_param(uid, int, 0644);

asmlinkage int (*original_call)(const char*, int, int);
asmlinkage int my_sys_open(const char __user *filename, int flags, int mode)
{
    int i=0;
    char ch;
    if(uid == current->pid)
    {
        printk("Opened file by %d: ", uid);
        do
        {
            get_user(ch, filename + i);
            i++;
            printk("%c", ch);
        } while(ch!=0);
    }
    return original_call(filename, flags, mode);
}
static int __init my_init(void)
{
    printk(KERN_ALERT "I'm dangerous. I hope you did a ");
    printk(KERN_ALERT "sync before you insmod'ed me.\n");
    printk(KERN_ALERT "My counterpart, cleanup_module(), is even");
    printk(KERN_ALERT "more dangerous. If\n");
    printk(KERN_ALERT "you value your file system, it will ");
    printk(KERN_ALERT "be \"sync; rmmod\" \n");
    printk(KERN_ALERT "when you remove this module.\n");
  //  original_call = sys_call_ptr_t[__NR_open];
  //  sys_call_ptr_t[__NR_open] = my_sys_open;
    int i = MAX_TRY;
    unsigned long *sys_table;
/*    sys_table = (unsigned long *)&system_utsname;
    while(i)
    {
        if(sys_table[__NR_read] == (unsigned long)sys_read)
        {
            sys_call_table=sys_table;
            flag=1;
            break;
        }
        i--;
        sys_table++;
    }*/
    flag = 1;
    sys_call_table = 0xffffffff820013a0; 
    if(flag)
    {
            original_call = (void*)xchg(&sys_call_table[__NR_open], my_sys_open);
    }
    printk(KERN_INFO "Spying on UID:%d\n", uid);
    return 0;
}
static void my_exit(void)
{
    if(sys_call_table[__NR_open] != my_sys_open)
    {
        printk(KERN_ALERT "Somebody else also played with the ");
        printk(KERN_ALERT "open system call\n");
        printk(KERN_ALERT "The system may be left in ");
        printk(KERN_ALERT "an unstable state.\n");
    }
    // sys_call_ptr_t[__NR_open] = original_call;
    xchg(&sys_call_table[__NR_open], original_call);
}
module_init(my_init);
module_exit(my_exit);
