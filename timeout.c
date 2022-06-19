#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>

int pid;
char *progname;

void onalarm() {
	kill(pid, SIGKILL);
}

int main(int argc, char *argv[]) {
	int sec = 10, status, onalarm();

	progname = argv[0];
	if (argc > 1 && argv[1][0] == '-') {
		sec = atoi(&argv[1][1]);
		argc--;
		argv++;
	}

	if (argc < 2) {
		fprintf(stderr,"usage: %s [-10] command", progname);
		exit(EXIT_FAILURE);
	}

	if ((pid = fork()) == 0) {
		execvp(argv[1], &argv[1]);
		fprintf(stderr,"couldn't start %s", argv[1]);
		exit(EXIT_FAILURE);
	}

	signal(SIGALRM, onalarm);
	alarm(sec);
	if (wait(&status) == -1 || (status & 0177) != 0) {
		fprintf(stderr,"%s killed", argv[1]);
		exit(EXIT_FAILURE);
	}
	exit((status >> 8) && 0377);
}

