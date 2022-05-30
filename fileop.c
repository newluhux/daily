#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

size_t insertfile(char *filename,size_t offset,void *buf,size_t size) {
	if (filename == NULL) {
		return -1;
	}
	if (buf == NULL) {
		return -1;
	}
	if ((offset < 0) || (size < 0)) {
		return -1;
	}

	int rfd;
	rfd = open(filename,O_RDONLY);
	if (rfd == -1) {
		return -1;
	}

	size_t old_size;
	old_size = lseek(rfd,0,SEEK_END);
	if (old_size == -1) {
		close(rfd);
		return -1;
	}

	int wfd;
	wfd = open(filename,O_WRONLY);
	if (wfd == -1) {
		close(rfd);
		return -1;
	}

	size_t new_size;
	new_size = old_size + size;

	if (lseek(rfd,new_size,SEEK_SET) == -1) { // 移动到新文件结尾位置
		close(fd);
		close(nfd);
		return -1;
	}
	if (lseek(wfd,old_size,SEEK_SET) == -1) { // 移动到旧文件结尾位置
		close(fd);
		close(nfd);
		return -1;
	}

	size_t ncopy = 0;
	size_t total = old_size - offset;

	size_t readpos = old_size;
	size_t writepos = new_size;

	while (1) {
		readpos++;
		writepos--;
		if (lseek(rfd,readpos,SEEK_SET) == -1) {
			
		}
	}
}


size_t cutfile(char *filename,size_t offset,size_t size) {
	if (filename == NULL) {
		return -1;
	}
	if ((offset < 0) || (size < 0)) {
		return -1;
	}

	int fd;
	fd = open(filename,O_WRONLY);
	if (fd == -1) {
		return -1;
	}
	if (lseek(fd,offset,SEEK_SET) == -1) {
		close(fd);
		return -1;
	}
	int lfd;
	lfd = open(filename,O_RDONLY);
	if (lfd == -1) {
		return -1;
	}
	if (lseek(lfd,offset+size,SEEK_SET) == -1) {
		close(lfd);
		return -1;
	}
	size_t nread;
	unsigned char buf[BUFSIZ];
	while ((nread = read(lfd,buf,BUFSIZ)) > 0) {
		if (write(fd,buf,nread) != nread) {
			close(lfd);
			return -1;
		}
	}
	close(lfd);

	size_t new_size;
	new_size = lseek(fd,-size,SEEK_END);
	if (new_size == -1) {
		close(fd);
		return -1;
	}
	if (ftruncate(fd,new_size) == -1) {
		close(fd);
		return -1;
	}
	close(fd);
	return 1;
}

int main(int argc, char *argv[]) {
	if (cutfile(argv[1],4,10) == -1) {
		fprintf(stderr,"Can't cut file %s ",argv[1]);
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}