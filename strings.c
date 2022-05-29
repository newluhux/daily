#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
	int c;

	while((c=getchar()) != EOF) {
		if (isprint(c)) {
			putchar(c);
		} else if (c == '\n') {
			putchar(c);
		}
	}
	exit(0);
}