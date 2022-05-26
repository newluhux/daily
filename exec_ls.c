#include <u.h>
#include <libc.h>

void main(int argc, char *argv[]) {
	switch(fork()) {
		case -1:
			sysfatal("fork failed");
		case 0:
			execl("/bin/sh", "sh", nil);
			break;
		default:
			print("sh started\n");
		}
	exits(nil);
}