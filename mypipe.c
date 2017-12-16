#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/fs.h>

#define DEVICE_NAME "mypipe"
#define BUFFER_SIZE 131072
#define BUFFER_MASK (BUFFER_SIZE-1)


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

static int major_number;
static wait_queue_head_t queue_read;
static wait_queue_head_t queue_write;
static char buffer[BUFFER_SIZE];
static int bytes_in_buffer = 0;
static ssize_t need_write_to_buffer = 0;
int idxIN = 0;
int idxOUT = 0;

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
	int total_read = 0;
	int indexOutFile = 0;
	int i;
	do {
	
		if (bytes_in_buffer == 0 && need_write_to_buffer > 0) {
			wake_up_interruptible(&queue_write); // будим пишуший поток
			wait_event_interruptible(queue_read, bytes_in_buffer > 0); // читающий поток засыпает
		}
	
		for (i = 0; i < bytes_in_buffer; i++) {
			dst[indexOutFile++] = buffer[idxOUT++];
			idxOUT &= BUFFER_MASK;
			total_read++;
			bytes_in_buffer--;
		}
	
	} while (need_write_to_buffer > 0);
	
	return total_read;
}

static ssize_t mypipe_write (struct file *f, const char __user *src, size_t size, loff_t *l)
{
	int total_written = 0;
	int i;
	need_write_to_buffer = size;
	for (i = 0; i < size;) {
		buffer[idxIN++] = src[i++];
		total_written++;
		bytes_in_buffer++;
		need_write_to_buffer--;
		idxIN &= BUFFER_MASK;
				
		if (idxIN == idxOUT && need_write_to_buffer > 0) {
			wake_up_interruptible(&queue_read); // будим читающий поток
			wait_event_interruptible(queue_write, bytes_in_buffer == 0); // пишущий поток засыпает
		}
	}
	wake_up_interruptible(&queue_read); // будим читающий поток
	return total_written;
}

static char *param = "world";
module_param(param, charp, 0);

static int __init mypipe_init(void)
{
	major_number = register_chrdev(321, DEVICE_NAME, &fops);
	pr_alert("init module mypipe\n");
	init_waitqueue_head(&queue_read);
	init_waitqueue_head(&queue_write);
	return 0;
}

static void __exit mypipe_exit(void)
{
	unregister_chrdev(major_number, DEVICE_NAME);
	pr_alert("exit module mypipe\n");
}

module_init(mypipe_init);
module_exit(mypipe_exit);
