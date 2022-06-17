#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINE 4096

struct maps_line_t {
	unsigned long addr_start;
	unsigned long addr_end;
	char perms[4];
	unsigned long offset;
	short devmajor;
	short devminor;
	ino_t inode;
	char *pathname;
};

int maps_parser(char *line,struct maps_line_t *map) {
	if (line == NULL || map == NULL)
		return -1;
	char tline[MAXLINE];
	strncpy(tline,line,MAXLINE);

	map->addr_start = strtoull(strtok(tline, "-"), NULL, 16);
	map->addr_end = strtoull(strtok(NULL, " "), NULL, 16);
	strncpy(map->perms,strtok(NULL, " "),4);
	map->offset = strtoull(strtok(NULL, " "), NULL, 16);
	map->devmajor = atoi(strtok(NULL, ":"));
	map->devminor = atoi(strtok(NULL, " "));
	map->inode = atoi(strtok(NULL, " "));
	map->pathname = strtok(NULL, "\n");

	return 1;
}

void maps_line_t_print(struct maps_line_t map) {
	printf("%lx-%lx %s %lx %i:%i %l",
		map.addr_start, map.addr_end,
		map.perms,
		map.offset,
		map.devmajor,map.devminor,
		map.inode);
	if (map.pathname != NULL) {
		printf("%s",map.pathname);
	}
	putchar('\n');
	return;
}

int main(void) {
	FILE *maps;
	struct maps_line_t mapline;

	maps = fopen("/proc/32752/maps","r");
	char line[MAXLINE];
	while (fgets(line,MAXLINE,maps) != NULL) {
		maps_parser(line,&mapline);
		maps_line_t_print(mapline);
	}
}