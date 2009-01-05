#include "device.h"
#include "init.h"

int aids_device_number;

static int aids_init(void)
{
	printk(KERN_ALERT "AIDS init!\n");
	aids_device_number = register_chrdev(0, "aids", &fops);
	if (aids_device_number < 0)
	{
		printk(KERN_ALERT "Couldn't register character device\n");
		return -1;
	} else
	{
		printk(KERN_ALERT "Registered device under '%d'\n", aids_device_number);
	}
	return 0;
}

static void aids_exit(void)
{
	unregister_chrdev(aids_device_number, "aids");
	printk(KERN_ALERT "AIDS exit!\n");
	return;
}
