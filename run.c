#include <u.h>
#include <libc.h>

int run(char *cmd, char *argv[]) {
	Waitmsg* m;
	int ret;
	int pid;

	if (access(cmd,AEXEC) < 0) {
		return -1;
	}
	pid = fork();
	switch(pid) {
	case -1:
		return -1;
	case 0:
		exec(cmd, argv);
		sysfatal("exec: %r");
	default:
		while (m = wait()) {
			if (m->pid == pid) {
				if (m->msg[0] == 0) {
					ret = 0;
				} else {
					werrstr(m->msg);
					ret = -1;
				}
				free(m);
				return ret;
			}
			free(m);
		}
	}
}

void main(int argc, char *argv[]) {
	char *argvr[] = { "/usr/bin/env", "sleep", "5",  nil };
	print("%d\n",run("/usr/bin/env", argvr));
	exits(nil);
}