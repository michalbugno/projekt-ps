#ifndef DEVICE_H
#define DEVICE_H

#include <linux/fs.h>

ssize_t device_read(struct file *, char *, size_t, loff_t *);
ssize_t device_write(struct file *, const char *, size_t, loff_t *);
int device_open(struct inode *, struct file *);
int device_release(struct inode *, struct file *);

extern struct file_operations fops;

#endif
