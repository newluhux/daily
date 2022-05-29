#include <u.h>
#include <libc.h>

void main(int argc, char *argv[]) {
	int fd[2];
//	char buf[128];
//	int nr;
	if (pipe(fd) < 0) {
		sysfatal("can't create a pipe: %r");
	}
	close(fd[0]);
	write(fd[1],"Hello!\n",7);
	exits(nil);
}