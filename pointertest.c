#include <stdio.h>
#include <stdlib.h>

struct abc {
	int a;
};

int main(void) {
	struct abc *a = malloc(sizeof(struct abc));
	a->a = 123;
	printf("%d %d\n",&a->a,a->a);
	return 0;
}