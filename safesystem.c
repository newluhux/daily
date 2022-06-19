#include <signal.h>
#include <stdio.h>

int system(char *s) {
	int status, pid, w, tty;
	fflush(stdout);
	tty = open("/dev/tty",2);
	if (tty == -1) {
		fprintf(stderr,"%s: can't open /dev/tty\n",progname);
		return -1;
	}
	if ((pid = fork()) == 0) {
		close(0); dup(tty);
		close(1); dup(tty);
		close(2); dup(tty);
		close(tty);
		execlp("sh", "sh", "-c", s, NULL);
		exit(127);
	}
	close(tty);
	int (*istat)(), (*qstat)();
	istat = signal(SIGINT, SIG_IGN);
	qstat = signal(SIGQUIT, SIG_IGN);
	while ((w = wait(&status)) != pid && w != -1)
		;
	if (w == -1)
		status = -1;
	signal(SIGINT,istat);
	signal(SIGQUIT, qstat);
	return status;
}