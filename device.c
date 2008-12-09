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
