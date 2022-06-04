#include <stdio.h>

int main(void) {
	char *line;
	off_t linelen;
	while (getline(&line,&linelen,stdin) != -1) {
		printf("%s",line);
	}
	return 0;
}
