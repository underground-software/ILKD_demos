#include <stdio.h>
#define FOO(x) y##x

int main(void) {
	int FOO(bar);
	int FOO(car);
	ybar = 3;
	ycar = 7;
	printf("ybar = %d, ycar = %d\n", ybar, ycar);
	return 0;
}
