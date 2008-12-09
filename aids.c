#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include "aids.h"

static int aids_init(void)
{
	printk(KERN_ALERT "AIDS init!\n");
	return 0;
}

static void aids_exit(void)
{
}

module_init(aids_init);
module_exit(aids_exit);
