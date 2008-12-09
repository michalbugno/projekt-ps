#include "aids.h"

static int aids_init(void)
{
	printk(KERN_ALERT "AIDS init!\n");
	return 0;
}

static void aids_exit(void)
{
	printk(KERN_ALERT "AIDS exit!\n");
}
