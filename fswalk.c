#include <u.h>
#include <libc.h>

#define PATH_MAX 8192

vlong fswalk(char *file) {
	int fd;
	Dir *sp;
	vlong sum = 0;
	Dir *dents;
	int ndents;
	int i;
	char nextfile[PATH_MAX];

	fprint(1,"%s\n",file);
	sp = dirstat(file);
	if (sp == nil) {
		return 0;
	}
	sum += sp->length;
	if (!(sp->qid.type & QTDIR)) { // 如果不是目錄
		free(sp);
		return sum;
	} else { // 如果是目录
		free(sp);
		fd = open(file,OREAD);
		if (fd < 0) {
			return 0;
		}
		while (1) {
			ndents = dirread(fd,&dents);
			if (ndents == 0) {
				break;
			}
			for (i = 0; i < ndents; i++) { // 遍历目录
				if (strcmp(dents[i].name,"..") == 0) {
					continue;
				}
				if (strcmp(dents[i].name,".") == 0) {
					continue;
				}
				if (strlen(file) >= PATH_MAX) {
					continue;
				}
				strcpy(nextfile,file);
				if ((strlen(file)+strlen("/")) >= PATH_MAX) {
					continue;
				}
				strcat(nextfile,"/");
				if ((strlen(nextfile)+strlen(dents[i].name)) >= PATH_MAX) {
					continue;
				}
				strcat(nextfile,dents[i].name);
				sum += fswalk(nextfile);	
			}
			free(dents);
		}
	}
	return sum;
}

void main(int argc, char *argv[]) {
	if (argc < 2) {
		fprint(2,"Usage: %s: dir\n",argv[0]);
		exits("usage");
	}
	print("%d\n",fswalk(argv[1]));
	exits(nil);
}