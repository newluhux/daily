#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

char *tempfile = "temp.XXXXXX";

onintr(void) {
	unlink(tempfile);
	exit(1);
}

int main(int argc, char *argv[]) {
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, &onintr);
	mktemp(tempfile);

	sleep(50);
	exit(0);
}

