#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int aids_init(void);
static void aids_exit(void);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michal Bugno & Antek Piechnik");
MODULE_DESCRIPTION("Statistical Anomaly IDS");

module_init(aids_init);
module_exit(aids_exit);
