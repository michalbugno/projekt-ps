#include <asm/uaccess.h>
#include "device.h"

struct file_operations fops =
{
	read   : device_read,
	write  : device_write,
	open   : device_open,
	release: device_release
};


ssize_t device_read(struct file *filp, char *buffer, size_t size, loff_t *loff)
{
	int err;

	err = copy_to_user(buffer, "oh my\n", 7);
	if (err != 0)
	{
		printk("Couldn't copy %d bytes\n", err);
	}
	return 0;
}

ssize_t device_write(struct file *filp, const char *buffer, size_t size, loff_t *loff)
{
	return 0;
}

int device_open(struct inode *inode, struct file *filp)
{
	return 0;
}

int device_release(struct inode *inode, struct file *filp)
{
	return 0;
}
