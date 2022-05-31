#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

struct line_t {
	off_t offset;
	void *line;
	uint64_t len;
	struct line_t *next;
	struct line_t *prev;
};

struct line_t *line_t_new(void) {
	struct line_t *new;
	new = (struct line_t *)malloc(sizeof(*new));
	if (new == NULL) {
		return NULL;
	}
	new->offset = 0;
	new->next = NULL;
	new->prev = NULL;
	new->line = NULL;
	new->len = 0;
	return new;
}

// 将 lt 从链上移除
int line_t_free(struct line_t *lt) {
	// 把断开的链接起来
	if (lt->next != NULL) {
		lt->next->prev = lt->prev;
	}
	if (lt->prev != NULL) {
		lt->prev->next = lt->next;
	}

	if (lt->line != NULL) {
		free(lt->line);
	}
	free(lt);
	return 1;
}

// 向链插入新的项
int line_t_insert_new(struct line_t *lt,int where) {
	struct line_t *new = NULL;
	new = line_t_new();
	if (new == NULL) {
		goto err;
	}
	if (where == 1) { // after
		new->next = lt->next;
		if (new->next != NULL) {
			new->next->prev = new;
		}
		lt->next = new;
		new->prev = lt;
	} else if (where == -1) { // before
		new->prev = lt->prev;
		if (new->prev != NULL) {
			new->prev->next = new;
		}
		lt->prev = new;
		new->next = lt;
	} else { // err
		goto err;
	}

	return 1;
err:
	return -1;	
}

// 返回链表第一个结构
// 如果失败，返回NULL
struct line_t *line_t_get_head(struct line_t *lt) {
	struct line_t *head = NULL;
	if (lt == NULL) {
		return NULL;
	}
	head = lt;
	while (head->prev != NULL) {
		head = head->prev;
	}
	return head;
}

// 返回链表最后一个结构
// 如果失败，返回NULL
struct line_t *line_t_get_tail(struct line_t *lt) {
	struct line_t *tail = NULL;
	if (lt == NULL) {
		return NULL;
	}
	tail = lt;
	while (tail->next != NULL) {
		tail = tail->next;
	}
	return tail;
}

// 获取链表的长度
uint64_t line_t_get_len(struct line_t *lt) {
	struct line_t *temp = NULL;
	uint64_t len = 1;
	if (lt == NULL) {
		return 0;
	}
	temp = lt;
	while (1) {
		if (temp->next != NULL) {
			temp = temp->next;
			len++;
		} else {
			break;
		}
	}
	temp = lt;
	while (1) {
		if (temp->prev != NULL) {
			temp = temp->prev;
			len++;
		} else {
			break;
		}
	}
	return len;
}

// 返回链表中从前往后第 num 个结构
// 如果失败，返回NULL
struct line_t *line_t_get(struct line_t *lt,uint64_t num) {
	struct line_t *head = NULL;
	head = line_t_get_head(lt);
	if (head == NULL) {
		goto err;
	}
	struct line_t *ret = NULL;

	ret = head;
	uint64_t i;
	for (i=1;i<num;i++) {
		if (ret->next == NULL) {
			goto err;
		}
		ret = ret->next;
	}
	return ret;

err:
	return NULL;
}

int line_t_test(void) {
	struct line_t *chain = NULL;

	// 初始化测试
	chain = line_t_new();
	if (chain == NULL) {
		assert(0);
		return -1;
	}

	// 创建长度为一百的链表
	uint64_t i;
	for (i=1;i<=99;i++) {
		if (line_t_insert_new(chain,1) == -1) {
			assert(0);
			return -1;
		}
	}

	// 检查长度
	if (line_t_get_len(chain) != 100) {
		assert(0);
		return -1;
	}

	// 链表长度增加100
	for (i=1;i<=100;i++) {
		if (line_t_insert_new(chain,1) == -1) {
			assert(0);
			return -1;
		}
	}

	// 检查长度
	if (line_t_get_len(chain) != 200) {
		assert(0);
		return -1;
	}

	// 获取第一个项
	struct line_t *head = NULL;
	head = line_t_get_head(chain);
	if (head == NULL) {
		assert(0);
		return -1;
	}
	if (head->prev != NULL) {
		assert(0);
		return -1;
	}

	// 获取最后一个项
	struct line_t *tail = NULL;
	tail = line_t_get_tail(chain);
	if (tail == NULL) {
		assert(0);
		return -1;
	}
	if (tail->next != NULL) {
		assert(0);
		return -1;
	}

	struct line_t *current = NULL;

	// 从前往后赋值
	current = head;
	for (i=1;i<=200;++i) {
		current->offset = i;
		current = (current->next);
	}

	// 从后往前赋值
	current = tail;
	for (i=200;i>=1;--i) {
		current->offset = i;
		current = current->prev;
	}

	// 检查第一个项
	current = line_t_get(chain,1);
	if (current == NULL) {
		assert(0);
		return -1;
	}
	if (current->offset != 1) {
		assert(0);
		return -1;
	}

	// 检查第20个项
	current = line_t_get(chain,20);
	if (current == NULL) {
		assert(0);
		return -1;
	}
	if (current->offset != 20) {
		assert(0);
		return -1;
	}

	// 检查第60个项
	current = line_t_get(chain,60);
	if (current == NULL) {
		assert(0);
		return -1;
	}
	if (current->offset != 60) {
		assert(0);
		return -1;
	}

	// 检查第100个项
	current = line_t_get(chain,100);
	if (current == NULL) {
		assert(0);
		return -1;
	}
	if (current->offset != 100) {
		assert(0);
		return -1;
	}

	// 检查第150个项
	current = line_t_get(chain,150);
	if (current == NULL) {
		assert(0);
		return -1;
	}
	if (current->offset != 150) {
		assert(0);
		return -1;
	}

	// 检查第200个项
	current = line_t_get(chain,200);
	if (current == NULL) {
		assert(0);
		return -1;
	}
	if (current->offset != 200) {
		assert(0);
		return -1;
	}

	// 释放测试
	current = head;
	for (i=1;i<=199;i++) {
		line_t_free(current->next);
	}
	line_t_free(current);

	return 1;
}

struct file_t {
	FILE *fp;
	uint64_t lines;
	struct line_t *start;
	struct line_t *end;
};

struct file_t *file_t_new(char *fname) {
	if (fname == NULL) {
		return NULL;
	}
	struct file_t *new;
	new = (struct file_t *)malloc(sizeof(*new));
	if (new == NULL) {
		return NULL;
	}

	new->fp = fopen(fname,"r");
	if (new->fp == NULL) {
		goto err;
	}

	struct line_t *line1 = NULL;
	line1 = line_t_new();
	if (line1 == NULL) {
		goto err;
	}

	new->lines = 0;
	new->start = line1;
	new->end = line1;

	return new;

err:
	if (line1 != NULL) {
		line_t_free(line1);
	}
	if (new != NULL) {
		if (new->fp != NULL) {
			fclose(new->fp);
		}
		free(new);
	}
	return NULL;
}

int file_t_load_file(struct file_t *ft) {
	if (ft == NULL) {
		return -1;
	}

	if (ft->fp == NULL) {
		return -1;
	}

	struct line_t *cur;
	char *line = NULL;
	uint64_t len = 0;

	cur = ft->start;

	while ((len = getline(&line,&len,ft->fp)) != -1) {
		cur->line = strdup(line);
		cur->len = len;
		if (line_t_insert_new(cur,1) == -1) {
			return -1;
		}
		cur = cur->next;
		ft->lines++;
	}
	free(line);

	ft->end = line_t_get(cur,ft->lines);
	return 1;
}

int file_t_free(struct file_t *ft) {
	if (ft == NULL) {
		return -1;
	}

	if (ft->fp != NULL) {
		fclose(ft->fp);
	}

	if (ft->start != NULL) {
		if (ft->start->prev != NULL) {
			ft->start = line_t_get_head(ft->start);
		}
		while (ft->start->next != NULL) {
			line_t_free(ft->start->next);
		}
	}

	if (ft->end != NULL) {
		if (ft->end->next != NULL) {
			ft->end = line_t_get_tail(ft->end);
		}
		while (ft->end->prev != NULL) {
			line_t_free(ft->end->prev);
		}
	}
	free(ft);

	return 1;
}



int file_t_test(void) {
	struct file_t *testfile = NULL;

	testfile = file_t_new("/tmp/test");
	if (testfile == NULL) {
		assert(0);
	}
	if (file_t_load_file(testfile) == -1) {
		assert(0);
	}
	printf("lines : %d\n",testfile->lines);
	
	uint64_t i;
	struct line_t *cur;
	for (i=1;i<=(testfile->lines);i++) {
		cur = line_t_get(testfile->start,i);
		if (cur == NULL) {
			assert(0);
		}
		printf("%s",cur->line);
	}
	return 1;
}

int main(int argc, char *argv[1]) {
	if (line_t_test() == -1) {
		fprintf(stderr,"line_t Test Failed!\n");
		exit(EXIT_FAILURE);
	}


	if (file_t_test() == -1) {
		fprintf(stderr,"file_t Test Failed!\n");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}