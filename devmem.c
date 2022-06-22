#include <stdio.h>

// start 区域起始地址 必须内存对齐
// length 区域长度 必须内存对齐
// map mmap 返回映射的地址
int phy_mem_mmap(int devmemfd,void *start,size_t length,void **map) {
	int pagesize = sysconf(_SC_PAGE_SIZE);
	if (devmemfd == -1
		|| (start % pagesize) != 0
		|| (length % pagesize) != 0) {
		return -1;
	}
	map = mmap(NULL,length,PROT_READ | PROT_WRITE,MAP_SHARED,devmemfd,start);
	if (map == MAP_FAILED) {
		return -1;
	}
	return 1;
}

// map mmap 映射的地址
int phy_mem_munmap(void *map,size_t length) {
	if (munmap(map,length) == -1) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}
}	