#include <u.h>
#include <libc.h>

long cmdoutput(char *cmd, char *buf, long len) {
	int fd[2];
	long tot;

	if (pipe(fd) < 0) {
		return -1;
	}

	switch(fork()) {
	case -1:
		return -1;
	case 0:
		close(fd[0]);
		dup(fd[1], 1);
		close(fd[1]);
		execl("/bin/rc", "-c" , cmd ,nil);
		sysfatal("exec");
	default:
		close(fd[1]);
		for (tot = 0; len - tot > 1; tot += nr) {
			nr = read(fd[0],buf+tot,len-tot);
			if (nr <= 0) {
				break;
			}
		}
		close(fd[0]);
		waitpid();
		buf[tot] = 0;
		return tot;
	}
}

