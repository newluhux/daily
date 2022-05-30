#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

// 反着写入
// 鉴定为纯纯的奥里给

int main(int argc, char *argv[]) {
	FILE *infile;
	FILE *outfile;
	
	if (argc < 2) {
		exit(EXIT_FAILURE);
	}

	infile = fopen(argv[1],"r");
	if (infile == NULL) {
		exit(EXIT_FAILURE);
	}
	outfile = fopen(argv[2],"w+");
	if (outfile == NULL) {
		exit(EXIT_FAILURE);
	}

	off_t filesize;
	if (fseek(infile,0,SEEK_END) == -1) {
		exit(EXIT_FAILURE);
	}
	filesize = ftell(infile);
	if (filesize == -1) {
		exit(EXIT_FAILURE);
	}

	off_t inseek = filesize-1;
	off_t outseek = filesize-1;
	printf("filesize %d\n",filesize);
	unsigned char c;

	while (1) {
		if (fseek(infile,inseek,SEEK_SET) == -1) {
			break;
		}
		fread(&c,1,1,infile);

		if (fseek(outfile,outseek,SEEK_SET) == -1) {
			break;
		}
		if (fwrite(&c,1,1,outfile) != 1) {
			break;
		}

		if (outseek == 0 || inseek == 0) {
			break;
		}
		--outseek;
		--inseek;
	}

	fclose(infile);
	fclose(outfile);
	exit(EXIT_SUCCESS);
}
