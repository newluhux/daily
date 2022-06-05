#include <stdio.h>

typedef struct LINE {
	struct LINE *next;
	struct LINE *prev;
	int len;
	char data[1];
} LINE;

int main(int argc, char *argv[]) {
	bufSize = INITBUF_SIZE;
	bufBase = xmalloc(bufSize);
	bufPtr = bufBase;
	lines.next = &lines;
	lines.prev = &lines;
	
	if (argv[1]) {
		fileName = xstrdup(argv[1]);
		if (!readLines(fileName, 1)) {
			return EXIT_SUCCESS;
		}
		if (lastNum)
			setCurNum(1);
		dirty = FALSE;
	}

	doCommands();
	return EXIT_SUCCESS;
}