#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

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

int dump_mem_reg(unsigned long addr,size_t length,int pagesize,int fdin,int fdout) {
	unsigned char page[pagesize];
	size_t address;

	if (addr <= 0 || length <= 0 || fdin == -1 || fdout == -1)
		return -1;

	if (lseek(fdin,addr,SEEK_SET) == -1)
		return -1;

	for (address=addr;address < addr+length; address += pagesize) {
		if (read(fdin,page,pagesize) != pagesize)
			return -1;
		if (write(fdout,page,pagesize) != pagesize)
			return -1;
	}
	return 1;
}

int main(int argc, char *argv[]) {
	if (argc < 3) {
		fprintf(stderr,"usage: %s pid output\n",argv[0]);
		exit(EXIT_FAILURE);
	}

	char mapsfn[512];
	char memfn[512];
	sprintf(mapsfn,"/proc/%s/maps",argv[1]);
	sprintf(memfn,"/proc/%s/mem",argv[1]);

	FILE *mapsfile;
	mapsfile = fopen(mapsfn,"r");
	if (mapsfile == NULL) {
		fprintf(stderr,"error: can't open %s\n",mapsfn);
		exit(EXIT_FAILURE);
	}

	char line[512];
	unsigned long addr_start;
	unsigned long addr_end;
	char outputfn[512];
	int outputfd;
	long ptrace_result;
	pid_t pid;
	pid = atoi(argv[1]);
	ptrace_result = ptrace(PTRACE_ATTACH,pid,NULL,NULL);
	if (ptrace_result < 0) {
		fprintf(stderr,"error: can't attach %d\n",pid);
		exit(EXIT_FAILURE);
	}
	wait(NULL);
	int memfd;
	if ((memfd = open(memfn,O_RDONLY)) == -1) {
		fprintf(stderr,"can't open file %s\n",memfn);
		exit(EXIT_FAILURE);
	}
	struct maps_line_t maps_line;
	while (fgets(line, 512, mapsfile) != NULL) {
		if (maps_parser(line,&maps_line) == -1) {
			fprintf(stderr,"can't parser maps file %s\n",mapsfn);
			exit(EXIT_FAILURE);
		}
		printf("%lx-%lx\n",maps_line.addr_start,maps_line.addr_end);
		sprintf(outputfn,"%s_%08lx-%08lx.mem",argv[2],maps_line.addr_start,maps_line.addr_end);
		outputfd = open(outputfn,O_WRONLY|O_CREAT|O_TRUNC,S_IWUSR|S_IRUSR);
		if (outputfd == -1) {
			fprintf(stderr,"error: can't create file %s ",outputfn);
			perror("");
			exit(EXIT_FAILURE);
		}
		if (dump_mem_reg(maps_line.addr_start,maps_line.addr_end-maps_line.addr_start,4096,memfd,outputfd) == -1) {
			fprintf(stderr,"error: can't copy memory to output ");
			perror("");
		}
		close(outputfd);
	}
	ptrace(PTRACE_CONT,pid,NULL,NULL);
	ptrace(PTRACE_DETACH,pid,NULL,NULL);
	fclose(mapsfile);
	close(memfd);
	exit(EXIT_SUCCESS);	
}