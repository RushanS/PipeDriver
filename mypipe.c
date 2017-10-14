#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/fs.h>

#define DEVICE_NAME "mypipe"
#define MAJOR_NUMBER 333
#define BUFFER_SIZE 10000

static int major_number;
static int mypipe_open(struct inode *, struct file *);
static int mypipe_release(struct inode *, struct file *);
static ssize_t mypipe_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t mypipe_write (struct file *, const char __user *, size_t, loff_t *);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("My Pipe module");
MODULE_AUTHOR("Rushan");

static struct file_operations fops = 
{
	.open = mypipe_open,
	.release = mypipe_release,
	.read = mypipe_read,
	.write = mypipe_write
};

char buffer[BUFFER_SIZE];

static int mypipe_open(struct inode *i, struct file *f)
{
	pr_alert("file open\n");
	return 0;
}

static int mypipe_release(struct inode *i, struct file *f)
{
	pr_alert("file close\n");
	return 0;
}

static ssize_t mypipe_read(struct file *f, char __user *dst, size_t size, loff_t * l)
{
	memcpy(dst, buffer, size);
	printk(KERN_INFO "read %d bytes", size);
	return size;
}

static ssize_t mypipe_write (struct file *f, const char __user *src, size_t size, loff_t *l)
{
	if (size > BUFFER_SIZE) {
		memcpy(buffer, src, BUFFER_SIZE);
	}
	else {
		memcpy(buffer, src, size);
	}
	printk(KERN_INFO "write %d bytes", size);
	return size;
}


static char *param = "world";
module_param(param, charp, 0);

static int __init mypipe_init(void)
{
	major_number = register_chrdev(333, DEVICE_NAME, &fops);
	pr_alert("init module mypipe\n");
	return 0;
}

static void __exit mypipe_exit(void)
{
	unregister_chrdev(major_number, DEVICE_NAME);
	pr_alert("exit module mypipe\n");
}

module_init(mypipe_init);
module_exit(mypipe_exit);
