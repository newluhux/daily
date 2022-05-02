#include <stdio.h>
#include <stdlib.h>

void f1(void) {
	puts("This f1");
}

void f2(void) {
	puts("This f2");
}

void f3(void) {
	puts("This f3");
}

int main(void) {

	puts("Start");

	atexit(f1);
	atexit(f2);
	atexit(f3);


	puts("Stop");

	exit(0);
}