#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_MEM_REGION 8129     // 读入的内存区域个数上限
#define VVAR_PATHNAME "[vvar]"  // vvar 区域，不可被读

/*
参考文档: man 5 proc
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
	void *mem; // 存放读取的内存区域的内容
	struct maps_line_t info;
};

struct proc_t {
	pid_t pid;
	int memfd;
	int region_count; // 一共读入的内存区域个数
	struct mem_region_t *region_list[MAX_MEM_REGION];
};

struct mem_region_t *mem_region_t_new(void);
int mem_region_t_init(char *line,struct mem_region_t *mr,struct proc_t *proc);
void mem_region_t_free(struct mem_region_t *mr);
void mem_region_t_print(struct mem_region_t *mr);
int mem_region_t_dump(int outfd,struct mem_region_t *mr);

struct proc_t *proc_t_new(void);
int proc_t_clean_regions(struct proc_t *proc);
int proc_t_reload_regions(pid_t pid,struct proc_t *proc);
void proc_t_print(struct proc_t *proc);
int proc_t_dumpall(struct proc_t *proc);

// 删除字符串开头的空白
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
	if (new == NULL) {
		fprintf(stderr,"%s: can't alloc memory\n",__func__);
		exit(EXIT_FAILURE);
	}
	memset(new,0,sizeof(*new));
	new->mem = NULL;
	return new;
}

int mem_region_t_init(char *line,struct mem_region_t *mr,struct proc_t *proc) {
	if (line == NULL || mr == NULL || proc == NULL) {
		fprintf(stderr,"%s: try to access null ptr\n",__func__);
		exit(EXIT_FAILURE);
	}
		
	if ( proc->memfd < 0) {
		fprintf(stderr,"%s: invalid memfd\n",__func__);
		exit(EXIT_FAILURE);
	}

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

	if (lseek(proc->memfd,mr->info.addr_start,SEEK_SET) == -1) {
		fprintf(stderr,"%s: can't lseek memfd to 0x%lx\n",__func__,mr->info.addr_start);
		exit(EXIT_FAILURE);		
	}
	if (strncmp(mr->info.pathname,VVAR_PATHNAME,
		strlen(VVAR_PATHNAME)) == 0) // 跳过不可访问的 vvar 区域
		return 1;

	unsigned long addr_cur = mr->info.addr_start;
	unsigned char *memload = (unsigned char *)malloc(mr->info.addr_end - mr->info.addr_start);
	if (memload == NULL) {
		fprintf(stderr,"%s: can't alloc memory\n",__func__);
		exit(EXIT_FAILURE);			
	}

	mr->mem = memload;
	long pagesize = sysconf(_SC_PAGESIZE);
	if (((mr->info.addr_end - mr->info.addr_start) % pagesize) != 0) {
		fprintf(stderr,"%s: wrony pagesize\n",__func__);
		exit(EXIT_FAILURE);			
	}

	for (;addr_cur<mr->info.addr_end;addr_cur += pagesize) {
		if (read(proc->memfd,memload,pagesize) != pagesize) {
			fprintf(stderr,"%s: read error",__func__);
			perror("");
			exit(EXIT_FAILURE);
		}
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
	if (mr == NULL) {
		fprintf(stderr,"%s: can't alloc memory\n",__func__);
		exit(EXIT_FAILURE);
	}
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

int mem_region_t_dump(int outfd,struct mem_region_t *mr) {
	if (outfd < 1 || mr == NULL) {
		fprintf(stderr,"%s: try access null ptr\n",__func__);
		exit(EXIT_FAILURE);
	}
		
	if (mr->mem == NULL) {
		if (strcmp(mr->info.pathname,"[vvar]") == 0) {
			fprintf(stderr,"skip vvar region\n");
		} else {
			fprintf(stderr,"can't dump %lx-%lx\n",
							mr->info.addr_start,
							mr->info.addr_end);
		}
		return -1;
	}
	long pagesize = sysconf(_SC_PAGESIZE);
	unsigned char *p = (unsigned char *)mr->mem;
	unsigned long wn;
	for (wn=mr->info.addr_start;wn<mr->info.addr_end;wn+=pagesize) {
		if (write(outfd,p,pagesize) != pagesize) {
			fprintf(stderr,"%s:can't write ",__func__);
			perror("");
			return -1;
		}
		p += pagesize;
	}
	return 1;
}

int proc_t_clean_regions(struct proc_t *proc) {
	if (proc == NULL) {
		fprintf(stderr,"%s: try access null ptr\n",__func__);
		exit(EXIT_FAILURE);
	}

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

struct proc_t *proc_t_new(void) {
	struct proc_t *new = NULL;
	new = (struct proc_t *)malloc(sizeof(struct proc_t));
	if (new == NULL) {
		fprintf(stderr,"%s: can't alloc memory\n",__func__);
		exit(EXIT_FAILURE);
	}
		
	memset(new,0,sizeof(struct proc_t));
	proc_t_clean_regions(new);
	new->memfd = -1;
	new->pid = -1;
	return new;
}

int proc_t_reload_regions(pid_t pid,struct proc_t *proc) {
	if (proc == NULL) {
		fprintf(stderr,"%s: try access null ptr\n",__func__);
		exit(EXIT_FAILURE);
	}
		
	proc->pid = pid;

	proc_t_clean_regions(proc);

	int memfd = -1;
	int mapsfd = -1;
	char fname[PATH_MAX];

	if (ptrace(PTRACE_ATTACH,proc->pid,NULL,NULL) < 0) {
		fprintf(stderr,"%s: ptrace can't attch process %ld ",__func__,proc->pid);
		perror("");
		exit(EXIT_FAILURE);
	}

	snprintf(fname,PATH_MAX,"/proc/%ld/maps",proc->pid);
	mapsfd = open(fname,O_RDONLY);
	if (mapsfd == -1) {
		fprintf(stderr,"%s: can't open maps file %s ",__func__,fname);
		exit(EXIT_FAILURE);
	}
		
	snprintf(fname,PATH_MAX,"/proc/%ld/mem",proc->pid);
	memfd = open(fname,O_RDONLY);
	if (memfd == -1) {
		fprintf(stderr,"%s: can't open mem file %s ",__func__,fname);
		exit(EXIT_FAILURE);
	}
	proc->memfd = memfd;

	FILE *maps_stream = fdopen(mapsfd,"r");
	if (maps_stream == NULL) {
		fprintf(stderr,"%s: can't open maps stream\n",__func__);
		exit(EXIT_FAILURE);
	}

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
	if (proc == NULL) {
		fprintf(stderr,"%s: try access null ptr\n",__func__);
		exit(EXIT_FAILURE);
	}

	printf("pid: %ld\n",proc->pid);
	printf("memfd: %d\n",proc->memfd);
	printf("regions: %d\n",proc->region_count);

	int i;
	for (i=0;i<(proc->region_count);i++) {
		mem_region_t_print(proc->region_list[i]);
		putchar('\n');
	}
}

int proc_t_dumpall(struct proc_t *proc) {
	if (proc == NULL) {
		fprintf(stderr,"%s: try access null ptr\n",__func__);
		exit(EXIT_FAILURE);
	}
	char fname[PATH_MAX];
	int i;
	int outfd;
	for(i=0;i<(proc->region_count);i++) {
		snprintf(fname,PATH_MAX,"%ld_%lx-%lx.mem",
			proc->pid,
			proc->region_list[i]->info.addr_start,
			proc->region_list[i]->info.addr_end);
		outfd = open(fname,O_WRONLY|O_CREAT|O_TRUNC,S_IWUSR|S_IRUSR);
		if (outfd == -1) {
			fprintf(stderr,"can't open file for write %s ",fname);
			perror("");
			return -1;
		}
		mem_region_t_dump(outfd,proc->region_list[i]);
		close(outfd);
	}
	return 1;
}

void commands(FILE *in,pid_t pid) {
	// 初始化
	struct proc_t *proc = proc_t_new();
	proc_t_clean_regions(proc);
	proc_t_reload_regions(pid,proc);

	char line[PATH_MAX];

	while (fgets(line,PATH_MAX,in) != NULL) {
		if (strstr(line,"exit") != NULL) {
			exit(EXIT_FAILURE);
		} else if (strstr(line,"reload") != NULL) {
			proc_t_reload_regions(proc->pid,proc);
		} else if (strstr(line,"info") != NULL) {
			proc_t_print(proc);
		} else if (strstr(line,"dump") != NULL) {
			proc_t_dumpall(proc);
		} else if (strstr(line,"clean") != NULL) {
			proc_t_clean_regions(proc);
		}
	}
}

int main(int argc, char *argv[]) {
	pid_t pid;
	if (argc < 2) {
		fprintf(stderr,"usage: %s pid\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	pid = atol(argv[1]);
	commands(stdin,pid);
	exit(EXIT_SUCCESS);
}