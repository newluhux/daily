#include <u.h>
#include <libc.h>

void main(int argc, char *argv[]) {
	print("running ls\n");
	execl("ls","ls", "-l", "/usr/", nil);
	print("exec failed: %r\n");
}