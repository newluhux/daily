#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

union abcu {
	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;
};

struct abcs {
	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;
};

//typedef union abcu sixfour;
typedef struct abcs sixfour;

int main(void) {
	sixfour *mem;
	size_t size = sizeof(sixfour) * 40000000;

	mem = (sixfour *)malloc(size);

	printf("%ld\n",size);

	size_t i;

	FILE *rand;
	FILE *blackhole;

	rand = fopen("/dev/urandom","rb");
	blackhole = fopen("/dev/null","wb");


	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;

	for (i=0;i<(size/sizeof(sixfour));i++) {
		a = getc(rand);
		b = getc(rand);
		c = getc(rand);
		d = getc(rand);

		(mem + i)->a = a;
		(mem + i)->b = b;
		(mem + i)->c = c;
		(mem + i)->d = d;

		if ((mem+i)->a != a) {
			putchar('e');
		}
		if ((mem+i)->b != b) {
			putchar('e');
		}
		if ((mem+i)->c != c) {
			putchar('e');
		}
		if ((mem+i)->c != d) {
			putchar('e');
		}
		fputc((mem+i)->a,blackhole);
		fputc((mem+i)->b,blackhole);
		fputc((mem+i)->c,blackhole);
		fputc((mem+i)->d,blackhole);
	}

loop:
	goto loop;

	return 0;
}
