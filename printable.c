#include <stdio.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
	unsigned char c;
	unsigned char prev;

	prev = 0;
	for(c=0;c >= prev;c++) {
		prev = c;
		if (isprint(c)) {
			putchar(c);
		}
	}
	return 0;
}