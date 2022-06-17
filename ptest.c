#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void afree(char *a) {
	free(a);
	a = 1024;
	return;
}


int main(void) {
	char *a;
	a = (char *)malloc(sizeof(char));
	afree(a);
	printf("%ld\n",a);
	return 0;
}


