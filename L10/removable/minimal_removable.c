#include <linux/module.h>

int minimal_init(void) {
	return 0;
}

void minimal_exit(void) {}

module_init(minimal_init);
module_exit(minimal_exit);

