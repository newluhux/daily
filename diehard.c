#include <u.h>
#include <libc.h>

void main(int argc, char *argv[]) {
	while(fork() == 0) {
		;
	}
	exits(nil);
}