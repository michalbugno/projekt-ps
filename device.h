#include <linux/fs.h>

static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);

struct file_operations fops =
{
	read   : device_read,
	write  : device_write,
	open   : device_open,
	release: device_release
};
