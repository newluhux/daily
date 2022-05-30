#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

// 反着写入

int main(int argc, char *argv[]) {
	int infd;
	int outfd;
	
	if (argc < 2) {
		exit(EXIT_FAILURE);
	}

	infd = open(argv[1],O_RDONLY);
	if (infd == -1) {
		exit(EXIT_FAILURE);
	}
	outfd = open(argv[2],O_WRONLY);
	if (outfd == -1) {
		exit(EXIT_FAILURE);
	}

	off_t filesize;
	filesize = lseek(infd,0,SEEK_END);
	if (filesize == -1) {
		exit(EXIT_FAILURE);
	}

	off_t inseek = filesize-1;
	off_t outseek = filesize-1;
	printf("filesize %d\n",filesize);
	unsigned char c;

	while (1) {
		pread(infd,&c,1,inseek);

		if (pwrite(outfd,&c,1,outseek) != 1) {
			break;
		}

		if (outseek == 0 || inseek == 0) {
			break;
		}
		--outseek;
		--inseek;
	}

	close(infd);
	close(outfd);
	exit(EXIT_SUCCESS);
}