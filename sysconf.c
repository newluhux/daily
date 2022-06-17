#include <stdio.h>
#include <unistd.h>
#include <limits.h>

int main(void) {
	printf("%ld\n",sysconf(_SC_PAGESIZE));
	return 0;
}
