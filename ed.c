#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

struct line_t {
	off_t offset;
	struct line_t *prev;
	struct line_t *next;
};

/*
 创建 line_t 结构
 如果成功返回 malloc 申请的内存空间地址
 如果失败返回 NULL
*/
struct line_t *line_t_init(off_t offset);

/*
 在链上添加 line_t 节点
 如果where == 1 ，在next添加
 如果where == -1 ，在prev添加
 如果成功返回 1
 如果失败返回 -1
*/
int line_t_insert(struct line_t *lt,int where,off_t offset);

/*
 在链上删除指定的 line_t 节点
 如果成功返回 1
 如果失败返回 -1
*/
int line_t_free(struct line_t *lt);

/*
 删除整条链
 如果成功返回 1
 如果失败返回 -1
*/
int line_t_freeall(struct line_t *lt);

/*
 返回链的长度
 如果成功返回长度
 如果失败返回0
*/
unsigned long line_t_length(struct line_t *lt);
/*
 返回当前 line_t 节点在链中的位置 （从1开始计数）
 如果成功返回位置
 如果失败返回0
*/
unsigned long line_t_position(struct line_t *lt);

/*
 返回链中第position个节点的指针 （从1开始计数）
 如果成功则返回指针
 如果失败则返回 NULL
*/
struct line_t *line_t_get(struct line_t *lt,unsigned long position);

int test_line_t(void) {

	// 初始化随机数生成器
	FILE *randsrcfp = NULL;
	char *randsrcfn = "/dev/random";
	randsrcfp = fopen(randsrcfn,"r");
	if (randsrcfp == NULL) {
		assert(0);
	}

    struct line_t *line_t_node = NULL;
	line_t_node = line_t_init(1);
	if (line_t_node == NULL) {
		assert(0);
	}
	if (line_t_node->prev != NULL || line_t_node->next != NULL) {
		assert(0);
	}

    unsigned long chain_len = 0;
	chain_len = line_t_length(line_t_node);
	if (chain_len != 1) {
		assert(0);
	}

	struct line_t *line_t_temp = NULL;
	line_t_temp = line_t_get(line_t_node,1);
	if (line_t_temp != line_t_node) {
		assert(0);
	}
	line_t_temp = line_t_get(line_t_temp,50);
	if (line_t_temp != NULL) {
		assert(0);
	}

	unsigned long i = 0;
	uint16_t randu16;

	if (fread(&randu16,sizeof(randu16),1,randsrcfp) != 1) {
		assert(0);
	}
	unsigned long opcounter = 0;
	chain_len += randu16;
	line_t_temp = line_t_node;
	for (i=1;i<=chain_len;i++) {

		if (line_t_insert(line_t_temp,1,i) == -1) {
			assert(0);
		}
		if (line_t_temp->next->prev != line_t_temp) {
			assert(0);
		}

		line_t_temp = line_t_temp->next;
	}

	unsigned long chain_len_computed = 0;
	chain_len_computed = line_t_length(line_t_node);

	if (i != chain_len_computed) {
		assert(0);
	}

	if (line_t_node->prev != NULL) {
		assert(0);
	}

	line_t_temp = line_t_node;
	while (line_t_temp->next != NULL) {
		if (line_t_free(line_t_temp->next) == -1) {
			assert(0);
		}
		opcounter++;
	}

	line_t_temp = line_t_node;

	if (opcounter != chain_len_computed-1) {
		assert(0);
	}

	if (line_t_node->next != NULL || line_t_node->prev != NULL) {
		assert(0);
	}

	if (line_t_free(line_t_node) != 1) {
		assert(0);
	}
	return 0;
}

struct line_t *line_t_init(off_t offset) {
	struct line_t *new = NULL;
	new = (struct line_t *)malloc(sizeof(*new));
	if (new == NULL) {
		return new;
	}
	new->prev = NULL;
	new->next = NULL;
	new->offset = 0;
	return new;
}

int line_t_insert(struct line_t *lt,int where,off_t offset) {
	struct line_t *new = NULL;
	if (lt == NULL) {
		return -1;
	}
	if (where != 1 && where != -1) {
		return -1;
	}
	new = line_t_init(offset);
	if (new == NULL) {
		return -1;
	}
	if (where == 1) {
		new->next = lt->next;
		if (new->next != NULL) {
			new->next->prev = new;
		}
		lt->next = new;
		new->prev = lt;
	} else if (where == -1) {
		new->prev = lt->prev;
		if (new->prev != NULL) {
			new->prev->next = new;
		}
		lt->prev = new;
		new->next = lt;
	} else {
		return -1;
	}
	return 1;

}

int line_t_free(struct line_t *lt) {
	if (lt == NULL) {
		return -1;
	}
	if (lt->prev != NULL) {
		lt->prev->next = lt->next;
	}
	if (lt->next != NULL) {
		lt->next->prev = lt->prev;
	}
	free(lt);
	return 1;
}

int line_t_freeall(struct line_t *lt) {
	if (lt == NULL) {
		return -1;
	}
	while (lt->next != NULL) {
		if (line_t_free(lt) == -1) {
			return -1;
		}
	}
	while (lt->prev != NULL) {
		if (line_t_free(lt) == -1) {
			return -1;
		}
	}
	if (line_t_free(lt) == -1) {
		return -1;
	}
	return -1;
}

unsigned long line_t_length(struct line_t *lt) {
	struct line_t *temp;
	if (lt == NULL) {
		return 0;
	}
	temp = lt;
	unsigned long len = 1;
	while (temp->prev != NULL) {
		temp = temp->prev;
		++len;
	}
	temp = lt;
	while (temp->next != NULL) {
		temp = temp->next;
		++len;
	}
	return len;
}

unsigned long line_t_position(struct line_t *lt) {
	if (lt == NULL) {
		return 0;
	}
	unsigned long pos = 1;
	struct line_t *temp = NULL;
	temp = lt;
	while (temp->prev != NULL) {
		temp=temp->prev;
		++pos;
	}
	return pos;
}

struct line_t *line_t_get(struct line_t *lt,unsigned long position) {
	if (lt == NULL || position == 0) {
		return NULL;
	}
	unsigned long p;
	struct line_t *ret = NULL;
	ret = lt;
	while (ret->prev != NULL) {
		ret = ret->prev;
	}
	if (position == 1) {
		return ret;
	}
	for (p = 1;p<=position;p++) {
		if (ret->next == NULL) {
			return NULL;
		}
		ret = ret->next;
	}
	return ret;
}

int main(void) {
	if (test_line_t() == -1) {
		return -1;
	}
	return 0;
}