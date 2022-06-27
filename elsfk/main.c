#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

/*
 * 游戏中一共有7种形状
 */


// 一条杠
/*
0 0 0 1 | 0 0 0 0
0 0 0 1 | 0 0 0 0
0 0 0 1 | 0 0 0 0
0 0 0 1 | 1 1 1 1
*/
#define I0 0B0001000100010001
#define I1 0B0000000000001111
#define I2 0B0001000100010001
#define I3 0B0000000000001111


// 一个 L
/*
0 0 0 0 | 0 0 0 0 | 0 0 0 0 | 0 0 0 0
0 0 1 0 | 0 0 0 0 | 0 0 1 1 | 0 0 0 0
0 0 1 0 | 0 0 0 1 | 0 0 0 1 | 0 1 1 1
0 0 1 1 | 0 1 1 1 | 0 0 0 1 | 0 1 0 0
*/
#define L0 0B0000001000100011
#define L1 0B0000000000010111
#define L2 0B0000001100010001
#define L3 0B0000000001110100

// 一个 J
/*
0 0 0 0 | 0 0 0 0 | 0 0 0 0 | 0 0 0 0
0 0 0 1 | 0 0 0 0 | 0 0 1 1 | 0 0 0 0
0 0 0 1 | 0 1 1 1 | 0 0 1 0 | 0 1 0 0
0 0 1 1 | 0 0 0 1 | 0 0 1 0 | 0 1 1 1
*/
#define J0 0B0000000100010011
#define J1 0B0000000001110001
#define J2 0B0000001100100010
#define J3 0B0000000001000111

// 一个 田
/*
0 0 0 0
0 0 0 0
0 0 1 1
0 0 1 1
*/
#define O0 0B0000000000110011
#define O1 0B0000000000110011
#define O2 0B0000000000110011
#define O3 0B0000000000110011

// 一个 Z
/*
0 0 0 0 | 0 0 0 0
0 0 0 0 | 0 0 0 1
0 1 1 0 | 0 0 1 1
0 0 1 1 | 0 0 1 0
*/
#define Z0 0B0000000001100011
#define Z1 0B0000000100110010
#define Z2 0B0000000001100011
#define Z3 0B0000000100110010

// 一个 S
/*
0 0 0 0 | 0 0 0 0
0 0 0 0 | 0 0 1 0
0 0 1 1 | 0 0 1 1
0 1 1 0 | 0 0 0 1
*/
#define S0 0B0000000000110110
#define S1 0B0000001000110001
#define S2 0B0000000000110110
#define S3 0B0000001000110001

// 一个T
/*
0 0 0 0 | 0 0 0 0 | 0 0 0 0 | 0 0 0 0
0 0 0 0 | 0 0 1 0 | 0 0 0 0 | 0 0 0 1 
0 1 1 1 | 0 0 1 1 | 0 0 1 0 | 0 0 1 1
0 0 1 0 | 0 0 1 0 | 0 1 1 1 | 0 0 0 1
*/
#define T0 0B0000000001110010
#define T1 0B0000001000110010
#define T2 0B0000000000100111
#define T3 0B0000000100110001

// 形状列表
// 形状使用位图存放
uint16_t shape_list[7][4] = {
	{I0,I1,I2,I3},
	{L0,L1,L2,L3},
	{J0,J1,J2,J3},
	{O0,O1,O2,O3},
	{Z0,Z1,Z2,Z3},
	{S0,S1,S2,S3},
	{T0,T1,T2,T3}
};

#define GET_BIT(x,bit) ((x & (1 << bit)) >> bit)

void print_shape(uint16_t shape) {
	uint8_t i;
	for (i=1;i<=16;i++) {
		putchar(' ');
		if (GET_BIT(shape,i-1) == 1) {
			putchar('1');
		} else {
			putchar('0');
		}
		if ((i % 4) == 0)
			putchar('\n');
		
	}
}

void print_shape_all(void) {
	int i,j;
	for (i=0;i<7;i++) {
		for (j=0;j<4;j++) {
			print_shape(shape_list[i][j]);
			putchar('\n');
		}
	}
}

/*
池的样子
1000000000000001
1000000000000001
1000000000000001
1000000000000001
1000000000000001
1000000000000001
1000000000000001
1111111111111111
*/

#define POOL_LINE   0B1000000000000001 // 空状态
#define POOL_FULL   0B1111111111111111 // 满状态
#define POOL_BOTTOM 0B1111111111111111 // 池底部

#define POOL_ROW 16
#define ROW_START 4 // 从第四行开始显示
#define ROW_END ((POOL_ROW)-1) // 最底部的有效行

// 池
uint16_t pool[POOL_ROW];

int pool_init(void) {
	if (POOL_ROW < 8) // 行数太少
		return -1;
	int r;
	for (r=0;r<POOL_ROW-1;r++) {
		pool[r] = POOL_LINE; // 把池的墙壁初始化
	}
	pool[POOL_ROW-1] = POOL_BOTTOM; // 把池的底部初始化
	return POOL_ROW;
}

void printbits(uint16_t n) {
	uint8_t i;
	for (i=0;i<16;i++) {
		if (GET_BIT(n,i))
			putchar('E');
		else
			putchar(' ');
	}
}

void print_pool(void) {
	int r;
	for (r=0;r<POOL_ROW;r++) {
		printbits(pool[r]);
		putchar('\n');
	}
	return;
}

// 当前形状坐标
int8_t x;
int8_t y;

// 当前形状类型
uint8_t tetris_type = 0;

// 当前形状旋转状态
uint8_t tetris_orie = 0;

void init_tetris(void) {
	srand((unsigned)time(NULL));

	// 设置类型
	tetris_type = rand() % 7;

	// 设置方向
	tetris_orie = rand() & 3;
}

// 获取新的形状
uint16_t tetris_random(void) {
	// 随机设置类型

	uint16_t tetris;
	tetris = shape_list[tetris_type_current][tetris_orie_current];

	// 设置类型
	tetris_type_current = rand() % 7;

	// 设置方向
	tetris_orie_current = rand() & 3;

	y = 0;
	x = 8;
}

// 检测形状底部碰撞
int tetris_collision(void) {
	uint16_t tetris = [tetris_type]	
	uint16_t dest = 0;
	dest |= (((pool[y+0] >> x) << 0x0) & 0x000F);
	dest |= (((pool[y+1] >> x) << 0x0) & 0x00F0);
	dest |= (((pool[y+2] >> x) << 0x0) & 0x0F00);
	dest |= (((pool[y+3] >> x) << 0x0) & 0xF000);
	return ((dest & tetris) != 0);
}

// 在池中画出当前形状
void tetris_draw(void) {
	uint16_t tetris = shap_list[tetris_type][tetris_orie];

	pool[y+0] |= (((tetris >> 0x0) && 0x000F) << x);
	pool[y+1] |= (((tetris >> 0x4) && 0x000F) << x);
	pool[y+2] |= (((tetris >> 0x8) && 0x000F) << x);
	pool[y+3] |= (((tetris >> 0xC) && 0x000F) << x);
}

// 在池中擦除当前形状
void tetris_undraw(void) {
	uint16_t tetris = shap_list[tetris_type][tetris_orie];

	pool[y+0] &= ~(((tetris >> 0x0) & 0x000F) << x);
	pool[y+1] &= ~(((tetris >> 0x4) & 0x000F) << x);
	pool[y+2] &= ~(((tetris >> 0x8) & 0x000F) << x);
	pool[y+3] &= ~(((tetris >> 0xC) & 0x000F) << x);
}

void tetris_rotate(void) {
	int8_t orign_orie = tetris_orie;
	tetris_undraw();
	tetris_orie = (orign_orie + 1) & 3;
	if (tetris_collision()) {
		tetris_orie = origin_orie;
	}

}


int move = 0;

// 左右移动
void tetris_move(void) {
	int orig_x = x;
	
	tetris_undraw();
	if (move > 0)
		--x;
	else
		++x;

	if (tetris_collision()) {
		x = orig_x;
		tetris_draw();
	} else {
		tetris_draw();
	}
}

void tetris_down(void) {
	int8_t origin_y = y;

	tetris_undraw();
	++y;
	if (tetris_collision()) {
		y = origin_y;
		tetris_draw();
		tetris_cleanline()
	} else {
		tetris_draw();
	}
}

uintmax_t score = 0;

void tetris_cleanline(void) {
	uint8_t y_copy = y;
	uint8_t i;
	do {
		if (y_copy < ROW_END && pool[y_copy] == POOL_FULL) {
			score += SCORE_STEP;
			memmove(pool+1,pool,sizeof((uint16_t) *y));
		} else {
			--y_copy;
			++i;
		}
	} while (y_copy >= y && i < 4)
	tetris_random();

	return; //
}

void getkey(void) {
	int key = getchar();
	if (key == 'w') {
		tetris_rotate();
	} else if (key == 'a') {
		move = -1;
		tetris_move();
	} else if (key == 's') {
		tetris_down();
	} else if (key == 'd') {
		move = 1;
		tetris_move();
	}
}



int main(int argc, char *argv[]) {
	pool_init();
	init_tetris();
	while (1) {
		getkey();
		print_pool();
	}
	return 0;
}
