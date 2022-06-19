#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>

struct map {
	// 在 /proc/<pid>/maps 中存储的项
	void *addr_start; // man 2 mmap
	void *addr_end; // man 2 mmap
	int prot; // man 2 mmap
	int flags; // man 2 mmap
	off_t offset; // man 2 mmap
	unsigned int dev_major; // man 3 major
	unsigned int dev_minor; // man 3 major
	ino_t inode; // man 2 stat
	char *pathname; // man 5 proc
};
typedef struct map map;

map *map_init(void);
int map_parser(char *mapsline, map *map);
int map_free(map *map);
void map_show(map *map);

#define MAX_REGIONS 4096 // 内存区域个数上限
map *map_list[MAX_REGIONS];
int regions = 0;

int load_maps(int mapsfd);
int unload_maps(void);
void maps_show(void);

int virtaddr2phyaddr(int pagemapfd,void *vaddr,void **paddr);

map *map_init(void) {
	map *n = (map*)calloc(1,sizeof(*n));
	n->pathname = NULL;
	return n;
}

int make_perms(char *perms,int *prot,int *flags) {
	if (strlen(perms) != 4 || prot == NULL || flags == NULL)
		return -1;
	*prot = PROT_NONE;
	*flags = 0;
	if (index(perms,'r') != NULL)
		*prot |= PROT_READ;
	if (index(perms,'w') != NULL)
		*prot |= PROT_WRITE;
	if (index(perms,'x') != NULL)
		*prot |= PROT_EXEC;
	if (index(perms,'p') != NULL)
		*flags = MAP_SHARED;
	if (index(perms,'s') != NULL)
		*flags = MAP_PRIVATE;
	return 1;
}

// 移除字符串开头和结尾的空白，移除后的结果使用char *指针返回
char *strstripwhite(const char *s) {
	char *ns = strdup(s); // 新的字符串用作操作
	char *saveptr = ns;
	int len = strlen(ns);
	int i;

	for (i=len-1;isspace(ns[i]);i--) { // 移除结尾的空白
		ns[i] = '\0';
	}
	for (;isspace(*ns);ns++) // 移动指针到行首非空白字符上
		;
	ns = strdup(ns); // 拷贝新的字符串
	free(saveptr); // 释放旧的字符串
	return ns;
}

int map_parser(char *mapsline,map *map) {
	if (mapsline == NULL || map == NULL)
		return -1;
	char *linecopy = strdup(mapsline);
	map->addr_start = (void *)strtoull(strtok(linecopy,"-"), NULL, 16);
	map->addr_end = (void *)strtoull(strtok(NULL," "), NULL, 16);
	make_perms(strtok(NULL," "),&map->prot,&map->flags);
	map->offset = strtoull(strtok(NULL," "), NULL, 16);
	map->dev_major = atoi(strtok(NULL, ":"));
	map->dev_minor = atoi(strtok(NULL, " "));
	map->inode = atol(strtok(NULL, " "));

	char *pathname;
	pathname = strtok(NULL, "\n");
	if (pathname == NULL)
		map->pathname = NULL;
	else
		map->pathname = strstripwhite(pathname);
	return 1;
}

int map_free(map *map) {
	if (map->pathname != NULL) {
		free(map->pathname);
	}
	free(map);
	return 1;
}

void show_perms(int prot,int flags) {
	printf("perms: ");
	if (prot == PROT_READ)
		printf("PROT_READ ");
	else if (prot == PROT_WRITE)
		printf("PROT_WRITE ");
	else if (prot == PROT_EXEC)
		printf("PROT_EXEC ");
	else if (prot == (PROT_READ | PROT_WRITE))
		printf("PROT_READ|PROT_WRITE ");
	else if (prot == (PROT_READ | PROT_EXEC))
		printf("PROT_READ|PROT_EXEC ");
	else if (prot == (PROT_READ | PROT_WRITE | PROT_EXEC))
		printf("PROT_READ|PROT_WRITE|PROT_EXEC ");
	if (flags == MAP_SHARED)
		printf("MAP_SHARED ");
	else if (flags == MAP_PRIVATE)
		printf("MAP_PRIVATE ");
	putchar('\n');
	return;
}

void map_show(map *map) {
	if (map == NULL)
		return;
	printf("memory range: %lx-%lx\n",
			(unsigned long)map->addr_start,
			(unsigned long)map->addr_end);
	show_perms(map->prot,map->flags);
	printf("offset: %lx\n",map->offset);
	printf("dev: %d:%d\n",map->dev_major,map->dev_minor);
	printf("inode: %ld\n",map->inode);
	printf("pathname: ");
	if (map->pathname != NULL)
		printf("%s",map->pathname);
	putchar('\n');
	return;
}


#define LINEMAX 512
int load_maps(int mapsfd) {
	if (mapsfd < 0)
		return -1;
	FILE *fp = fdopen(mapsfd,"r");
	if (fp == NULL)
		return -1;
	char line[LINEMAX];
	while (fgets(line,LINEMAX,fp) != NULL) {
		if (regions == MAX_REGIONS)
			break;
		map_list[regions] = map_init();
		map_parser(line,map_list[regions]);
		++regions;
	}
	fclose(fp);
	return regions;
}

int unload_maps(void) {
	int i;
	for (i=0;i<regions;i++) {
		map_free(map_list[i]);
	}
	regions = 0;
	return regions;
}

void maps_show(void) {
	int i;
	for (i=0;i<regions;i++) {
		map_show(map_list[i]);
		putchar('\n');
	}
	return;
}

int virtaddr2phyaddr(int pagemapfd,void *vaddr,void **paddr) {
	
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr,"usage: %s pid\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	pid_t pid = atol(argv[1]);
	char filename[PATH_MAX];
	snprintf(filename,PATH_MAX,"/proc/%ld/maps",pid);
	int mapsfd = open(filename,O_RDONLY);
	if (mapsfd == -1) {
		fprintf(stderr,"Can't open file %s ",filename);
		perror("");
		exit(EXIT_FAILURE);
	}
	load_maps(mapsfd);
	maps_show();
	unload_maps();
	exit(EXIT_SUCCESS);
}