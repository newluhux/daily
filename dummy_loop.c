#include <stdio.h>
#include <unistd.h>

int main(void) {
loop:
	printf("%ld\n",getpid());
	fflush(stdout);
	sleep(1);
	goto loop;
}
