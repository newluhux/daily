#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

#define MAXLINES 8192

char* progname = "myed";
FILE* fileread = NULL;
size_t maxlines = MAXLINES; // 编辑器支持的最大行数
uint64_t *line_num_list; // 存放每行开头的offset位置, 是 stack
size_t line_pos_cur = 0; // sp

int line_num_list_init(void) { // 初始化stack
	line_num_list = (uint64_t *)malloc(sizeof(*line_num_list) * maxlines);
	if (line_num_list == NULL) {
		return -1;
	}
	return 1;
}

int push_line_pos(uint64_t line_pos) { // 往stack插入行位置
	if (line_pos_cur < maxlines) { // 如果到 stack
		*(line_num_list+line_pos_cur) = line_pos;
		++line_pos_cur;
		return 1;
	} else { // 如果到 stack 顶部
		line_pos_cur = maxlines -1;
		return -1;
	}
}

int pop_line_pos(uint64_t *ret) { // 从stack顶取出行位置
	if (line_pos_cur > 0) {
		*ret = *(line_num_list+line_pos_cur);
		--line_pos_cur;
		return 1;
	} else { // 如果到 stack 底部
		line_pos_cur = 0;
		return -1;
	}
}

int get_line_num_pos(uint64_t line_num,uint64_t *ret) { // 取出行的位置存放在ret
	if (line_num > maxlines+1) {
		return -1;
	}
	*ret = *(line_num_list+line_num-1);
	return 1;
}

int scan_file(void) {
	if (fileread == NULL) {
		return -1;
	}
	int c;
	uint64_t line_curr = 1;
	long pos = 0;

	push_line_pos(0); // 第一行从0开始

	if ((c = getc(fileread)) != EOF) {
		if (c == '\n') {
			line_curr++;
			pos = fseek(fileread,0,SEEK_CUR);
			if (push_line_pos(pos) == -1) {
				fprintf(stderr,"file too long\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	return 1;
}

int main(int argc, char *argv[]) {
	if (line_num_list_init() == -1) {
		fprintf(stderr,"can't malloc memory\n");
		exit(EXIT_FAILURE);
	}

	if (argc < 2) {
		fprintf(stderr,"Usage: %s file\n",progname);
		exit(EXIT_FAILURE);
	}
	fileread = fopen(argv[1],"r");
	if (fileread == NULL) {
		fprintf(stderr,"can't open file ");
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	if (scan_file() == -1) {
		fprintf(stderr,"can't scan file");
		exit(EXIT_FAILURE);
	}

	size_t i;
	uint64_t value;
	for(i=1;i<=line_pos_cur;i++) {
		value = get_line_num_pos(i,&value);
		if (value == -1) {
			fprintf(stderr,"invdail number\n");
			exit(EXIT_FAILURE);
		}
		printf("%d %d\n",i,value);
	}

	printf("%d\n",line_pos_cur);
	
	return 0;
}