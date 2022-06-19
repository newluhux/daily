#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

char *progname;

int main(int argc, char *argv[]) {
	int fd;
	struct stat stbuf;
	time_t old_time = 0;

	progname = argv[0];
	if (argc < 2) {
		fprintf(stderr,"Usage: %s filename [cmd]\n", progname);
		exit(EXIT_FAILURE);
	}
	if ((fd = open(argv[1], 0)) == -1) {
		fprintf(stderr,"can't open %s\n",argv[1]);
		exit(EXIT_FAILURE);
	}
	fstat(fd, &stbuf);
	while (stbuf.st_mtime != old_time) {
		old_time = stbuf.st_mtime;
		sleep(5);
		fstat(fd, &stbuf);
	}
	if (argc == 2) {
		execlp("cat", "cat", argv[1], NULL);
		fprintf(stderr,"can't execute cat %s\n", argv[1]);
		exit(EXIT_FAILURE);
	} else {
		execvp(argv[2], &argv[2]);
		fprintf(stderr,"can't execute %s\n",argv[2]);
	}
	exit(EXIT_SUCCESS);
}