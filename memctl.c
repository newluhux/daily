#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <ctype.h>

#define MAX_MEM_REGION 8129
#define VVAR_PATHNAME "[vvar]"

void sysfatal(char *msg);

void sysfatal(char *msg) {
	fprintf(stderr,"%s ",msg);
	perror("");
	exit(EXIT_FAILURE);
}

/*
/proc/<pid>/maps

describe: man 5 proc
*/
struct maps_line_t {
	unsigned long addr_start;
	unsigned long addr_end;
	char perms[4];
	unsigned long offset;
	short dev_major;
	short dev_minor;
	ino_t inode;
	char pathname[PATH_MAX];
};

struct mem_region_t {
	void *mem; // 存放读取的内存内容
	struct maps_line_t info;
};

struct proc_t {
	pid_t pid;
	int memfd;
	int region_count; // 多少个count
	struct mem_region_t *region_list[MAX_MEM_REGION]; // 除了[vvar]区域之外的所有区域
};

struct mem_region_t *mem_region_t_new(void);
int mem_region_t_init(char *line,struct mem_region_t *mr,struct proc_t *proc);
void mem_region_t_free(struct mem_region_t *mr);
void mem_region_t_print(struct mem_region_t *mr);

struct proc_t *proc_t_new(pid_t pid);
int proc_t_clean_regions(struct proc_t *proc);
int proc_t_reload_regions(struct proc_t *proc);
void proc_t_print(struct proc_t *proc);

char *strstripspace(char *s) {
	int i;
	int n = 0;
	for (i=0;i<strlen(s);i++) {
		if (isspace(s[i])) {
			n++;
		}
	}
	memmove(s,s+n,strlen(s)+1-n);
	return s;
}

struct mem_region_t *mem_region_t_new(void) {
	struct mem_region_t *new = NULL;
	new = (struct mem_region_t *)malloc(sizeof(struct mem_region_t));
	if (new == NULL)
		sysfatal("can't malloc memory ");
	memset(new,0,sizeof(*new));
	new->mem = NULL;
	return new;
}

int mem_region_t_init(char *line,struct mem_region_t *mr,struct proc_t *proc) {
	if (line == NULL || mr == NULL || proc == NULL)
		sysfatal("don't access null ptr ");
	if ( proc->memfd < 0)
		sysfatal("please open mem ");

	mr->info.addr_start = strtoull(strtok(line,"-"), NULL, 16);
	mr->info.addr_end = strtoull(strtok(NULL," "), NULL, 16);
	strncpy(mr->info.perms,strtok(NULL, " "),4);
	mr->info.offset = strtoull(strtok(NULL, " "), NULL, 16);
	mr->info.dev_major = atoi(strtok(NULL, ":"));
	mr->info.dev_minor = atoi(strtok(NULL, " "));
	mr->info.inode = atoi(strtok(NULL, " "));
	
	char *pathname;
	pathname = strtok(NULL,"\n");
	if (pathname != NULL) {
		strstripspace(pathname);
		strcpy(mr->info.pathname,pathname);
	} else {
		mr->info.pathname[0] = '\0';
	}

	if (lseek(proc->memfd,mr->info.addr_start,SEEK_SET) == -1)
		sysfatal("can't lseek mem file! ");

	if (strncmp(mr->info.pathname,VVAR_PATHNAME,strlen(VVAR_PATHNAME)) == 0) // 跳过不可访问的 vvar 区域
		return 1;

	unsigned long addr_cur = mr->info.addr_start;
	unsigned char *memload = (unsigned char *)malloc(mr->info.addr_end - mr->info.addr_start);
	if (memload == NULL)
		sysfatal("can't malloc memory! ");
	mr->mem = memload;
	long pagesize = sysconf(_SC_PAGESIZE);
	if (((mr->info.addr_end - mr->info.addr_start) % pagesize) != 0)
		sysfatal("wrong pagesize for reading ");
	for (;addr_cur<mr->info.addr_end;addr_cur += pagesize) {
		if (read(proc->memfd,memload,pagesize) != pagesize)
			sysfatal("can't read memory ");
		memload += pagesize;
	}
	return 1;
}

void mem_region_t_free(struct mem_region_t *mr) {
	if (mr->mem != NULL)
		free(mr->mem);
	free(mr);
	return;
}

void mem_region_t_print(struct mem_region_t *mr) {
	if (mr == NULL)
		sysfatal("don't access null ptr ");
	printf("memory buf : %lx\n",mr->mem);
	printf("memory range : %lx-%lx\n",mr->info.addr_start,mr->info.addr_end);
	printf("permission : %s \n",mr->info.perms);
	printf("offset : %ld\n",mr->info.offset);
	printf("dev : %d:%d\n",mr->info.dev_major,mr->info.dev_minor);
	printf("inode : %ld\n",mr->info.inode);
	printf("pathname : %s\n",mr->info.pathname);
	fflush(stdout);
	return;
}

int proc_t_clean_regions(struct proc_t *proc) {
	if (proc == NULL)
		sysfatal("don't access nullptr ");
	int i;
	for (i=0;i<MAX_MEM_REGION;i++) {
		if (proc->region_list[i] != NULL) {
			mem_region_t_free(proc->region_list[i]);
			proc->region_list[i] = NULL;
		}
	}
	proc->region_count = 0;
	return 1;
}

struct proc_t *proc_t_new(pid_t pid) {
	struct proc_t *new = NULL;
	new = (struct proc_t *)malloc(sizeof(struct proc_t));
	if (new == NULL)
		sysfatal("can't malloc memory!");
	memset(new,0,sizeof(struct proc_t));
	proc_t_clean_regions(new);
	new->memfd = -1;
	new->pid = pid;
	return new;
}

int proc_t_reload_regions(struct proc_t *proc) {
	if (proc == NULL)
		sysfatal("don't access null ptr ");
	proc_t_clean_regions(proc);

	int memfd = -1;
	int mapsfd = -1;
	char fname[PATH_MAX];

	if (ptrace(PTRACE_ATTACH,proc->pid,NULL,NULL) < 0)
		sysfatal("can't ptrace attach process");

	snprintf(fname,PATH_MAX,"/proc/%ld/maps",proc->pid);
	mapsfd = open(fname,O_RDONLY);
	if (mapsfd == -1)
		sysfatal("can't open maps");
	snprintf(fname,PATH_MAX,"/proc/%ld/mem",proc->pid);
	memfd = open(fname,O_RDONLY);
	if (memfd == -1)
		sysfatal("can't open mem");
	proc->memfd = memfd;

	FILE *maps_stream = fdopen(mapsfd,"r");
	if (maps_stream == NULL)
		sysfatal("can't open maps stream");

	char line[PATH_MAX];
	while (fgets(line,PATH_MAX,maps_stream) != NULL) {
		proc->region_list[proc->region_count] = mem_region_t_new();
		mem_region_t_init(line,proc->region_list[proc->region_count],proc);
		++(proc->region_count);
	}

	fclose(maps_stream);
	close(mapsfd);
	ptrace(PTRACE_CONT,proc->pid,NULL,NULL);
	ptrace(PTRACE_DETACH,proc->pid,NULL,NULL);
	return 1;
}

void proc_t_print(struct proc_t *proc) {
	if (proc == NULL)
		sysfatal("don't access null ptr");

	printf("pid: %ld\n",proc->pid);
	printf("memfd: %d\n",proc->memfd);
	printf("regions: %d\n",proc->region_count);

	int i;
	for (i=0;i<(proc->region_count);i++) {
		mem_region_t_print(proc->region_list[i]);
		putchar('\n');
	}
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr,"Usage: %s pid\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	pid_t pid = atol(argv[1]);

	struct proc_t *p = proc_t_new(pid);
	proc_t_reload_regions(p);
	proc_t_print(p);
	exit(EXIT_SUCCESS);
}