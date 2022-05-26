#include <u.h>
#include <libc.h>

void main(void) {
	fork();
	wait();
	fork();
	wait();
	print("hi\n");
}