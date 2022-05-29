#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	int c;
	int counter = 0;

	int i;
	unsigned long len = 0;


	for(i=0;i<argc;i++) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == 'l') {
				len = atoi(argv[i+1]);
			}
		}
	}

	while ((c = getchar()) != EOF) {
		if ( c == '\n' ) {
			counter = 0;
		}
		if ( counter == len ) {
			putchar('\n');
			counter = 0;
		}
		putchar(c);
		counter++;
	}
	exit(0);
}