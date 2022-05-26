#include <u.h>
#include <libc.h>

void main(int argc, char *argv[]) {
	int fd;
	switch(fork()) {
	case -1:
		sysfatal("fork failed!");
	case 0:
		fd = open("/home/luhux/.cwmrc", OREAD);
		dup(fd, 0);
		close(fd);
		execl("cat", "cat", nil);
		sysfatal("exec: %r");
	default:
		waitpid();
	}
	exits(nil);
}