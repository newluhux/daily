#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define PAGESIZE 25
#define MAXLINES 8192
#define MAX_STACK MAXLINES

FILE *efopen(char *file,char *mode);
char ttyin(void);
long pop(void);
long push(long value);
void print(void);

char *progname;
unsigned int oldpagesize;
unsigned int pagesize = PAGESIZE;
FILE *fp;

long stack[MAX_STACK];
long sp = 1;

FILE *efopen(char *file,char *mode) {
	FILE *fp;

	if ((fp = fopen(file,mode)) != NULL) {
		return fp;
	}
	fprintf(stderr,"%s: can't open file %s mode %s\n",
			progname, file, mode);
	exit(1);
}

char ttyin(void) {
	char buf[BUFSIZ];
	
	static FILE *tty = NULL;
	
	if (tty == NULL) {
		tty = efopen("/dev/tty", "r");
	}
	if (fgets(buf,BUFSIZ,tty) == NULL || buf[0] == 'q') {
		exit(0);
	} else if (buf[0] == '!') {
		if (strlen(buf) > 1) {
			system(buf+1);
		}
		printf("\n");
	} else if (buf[0] == ':') {
		if (isdigit(buf[1])) {
			if (atoi(buf+1) < sp) {
				sp = atoi(buf+1);
				fseek(fp,stack[sp],SEEK_SET);
			}
		}
	} else {
		return buf[0];
	}
}




long pop(void) {
	if (sp <= 1) {
		sp = 1;
		return -1;
	} else {
		return stack[sp--];
	}
}

long push(long value) {
	if (sp < MAX_STACK) {
		stack[sp++] = value;
		return value;
	} else {
		return -1;
	}
}

void print(void) {
	static int lines = 0;
	char buf[BUFSIZ];

	stack[0] = 0;
	while (fgets(buf,sizeof(buf),fp) != NULL) {
		if (buf[strlen(buf)-1] == '\n') {
			if (push(fseek(fp,0,SEEK_CUR)) < 0) {
				fprintf(stderr,"stack max");
				exit(EXIT_FAILURE);
			}
		}
		if (++lines < pagesize) {
			fputs(buf,stdout);
		} else {
			buf[strlen(buf)-1] = '\0';
			fputs(buf,stdout);
			fflush(stdout);
			ttyin();
			lines = 0;
		}
	}
}


int main(int argc, char *argv[]) {
	int i;

	progname = argv[0];

	if (argc == 1) {
		print();
	} else {
		for (i = 1;i < argc;++i) {
			if (argv[i][0] == '-') {
				if (argv[i][1] == 'n') {
					pagesize = atoi(argv[++i]);
					continue;
				}
			}
			fp = efopen(argv[i], "r");
			print();
			fclose(fp);
		}
	}
	exit(0);
}