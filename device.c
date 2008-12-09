#include "device.h"

static ssize_t device_read(struct file *file, char *buffer, size_t size, loff_t *loff)
{
	return 0;
}

static ssize_t device_write(struct file *file, const char *buffer, size_t size, loff_t *loff)
{
	return 0;
}
static int device_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
	return 0;
}
