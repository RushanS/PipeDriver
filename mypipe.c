#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

struct file_operations fops = 
{
	.open = mypiipe_open;
	.release = mypipe_release;
}

int mypipe_open(struct inode *i, struct file *f)
{
	pr_alert("file open\n");
}

int mypipe_release(struct inode *i, struct file *f)
{
	pr_alert("file close\n");
}


static char *param = "world";
module_param(whom, charp, 0);

//static int howmany = 1;
//module_param(howmany, int, 0);

static int __init mypipe_init(void)
{
	pr_alert("init module mypipe\n");
	return 0;
}

static void __exit mypipe_exit(void)
{
	pr_alert("exit module mypipe\n");
}

module_init(mypipe_init);
module_exit(mypipe_exit);
