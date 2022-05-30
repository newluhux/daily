#include <stdio.h>
#include <string.h>

#define MAXLINE 5

int main(void) {
	char buf[MAXLINE];
	int len;
	while (fgets(buf,MAXLINE,stdin) != NULL) {
		len = strlen(buf);
		printf("\t%d %s\t",len,buf);
	}
	return 0;
}