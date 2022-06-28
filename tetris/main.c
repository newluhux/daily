#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>

#define X_MAX 20
#define Y_MAX 24

uint8_t board[X_MAX][Y_MAX];

void board_init(void) {
	int x,y;
	// 全部填0
	for (y=0;y<Y_MAX;y++) {
		for (x=0;x<X_MAX;x++) {
			board[x][y] = 0;
		}
	}
	// 绘制两边的边框
	for (y=0;y<Y_MAX;y++) {
		board[0][y] = 1;
		board[X_MAX-1][y] = 1;
	}
	// 绘制底部的边框
	for (x=0;x<X_MAX;x++) {
		board[x][Y_MAX-1] = 1;
	}
}

#define EMPTY_SPACE '.' // 空白区域
#define BLOCK_CHAR  'F' // 方块和边界
#define NEWLINE	    '\n' // 换行
void board_print(void) {
	int x,y;
	for (y=0;y<Y_MAX;y++) {
		for (x=0;x<X_MAX;x++) {
			if (board[x][y] == 0) {
				putchar(EMPTY_SPACE);
			} else {
				putchar(BLOCK_CHAR);
			}
		}
		putchar(NEWLINE);
	}
}

void line_clean(int y) {
	int x;
	for (x=1;x<X_MAX-1;x++) {
		board[x][y] = 0;
	}
}

void line_fill(int y) {
	int x;
	for (x=1;x<X_MAX-1;x++) {
		board[x][y] = 1;
	}
}

int line_getblocknum(int y) {
	int x;
	int count = 0;
	for (x=1;x<X_MAX-1;x++) {
		if (board[x][y] != 0)
			count++;
	}
	return count;
}

void line_copy(int desty,int srcy) {
	if (desty == srcy)
		return;
	// 防止越界
	if (desty >= (Y_MAX-1))
		return;
	if (srcy >= (Y_MAX-1))
		return;

	int x;
	for (x=1;x<X_MAX-1;x++) {
		board[x][desty] = board[x][srcy];
	}
}

void line_move(int desty,int srcy) {
	if (desty == srcy)
		return;
	line_copy(desty,srcy);
	line_clean(srcy);
}

void line_faildown(int y) {
	int oldy = y;
	int nexty;
	while (1) {
		nexty = oldy + 1; // 下一行
		if (nexty >= Y_MAX-1) // 到底了
			return;
		if (oldy >= Y_MAX-1) // 防止越界
			return;
		if (line_getblocknum(oldy) == 0) // 跳过空行
			return;
		if (line_getblocknum(nexty) != 0) // 下一行有东西顶着
			return;
		line_move(nexty,oldy);
		oldy=nexty;
	}
}


uintmax_t score = 0;
#define SCORE_STEP 100;

void score_print(void) {
	printf("score: %llu\n",score);
}

void ui_print(void) {
	printf("%s","\033[1H\033[2J"); // 清理终端内容
	board_print();
	score_print();
}

void board_action(void) {
	int x,y;
	// 把满的行清理掉
	for (y=0;y<Y_MAX-1;y++) {
		if (line_getblocknum(y) == X_MAX-2) {
			score += SCORE_STEP;
			line_clean(y);
		}
	}
	// 从低往高检测，让没有支撑的行下落
	for (y=Y_MAX-2;y>=0;y--) {
		line_faildown(y);
	}
}

const uint8_t tetris_I0[] = {
	0,1,0,0,
	0,1,0,0,
	0,1,0,0,
	0,1,0,0};
const uint8_t tetris_I1[] = {
	0,0,0,0,
	1,1,1,1,
	0,0,0,0,
	0,0,0,0};
const uint8_t tetris_I2[] = {
	0,1,0,0,
	0,1,0,0,
	0,1,0,0,
	0,1,0,0};
const uint8_t tetris_I3[] = {
	0,0,0,0,
	1,1,1,1,
	0,0,0,0,
	0,0,0,0};

const uint8_t tetris_O0[] = {
	1,1,0,0,
	1,1,0,0,
	0,0,0,0,
	0,0,0,0};
const uint8_t tetris_O1[] = {
	1,1,0,0,
	1,1,0,0,
	0,0,0,0,
	0,0,0,0};
const uint8_t tetris_O2[] = {
	1,1,0,0,
	1,1,0,0,
	0,0,0,0,
	0,0,0,0};
const uint8_t tetris_O3[] = {
	1,1,0,0,
	1,1,0,0,
	0,0,0,0,
	0,0,0,0};

const uint8_t tetris_S0[] = {
	0,1,1,0,
	1,1,0,0,
	0,0,0,0,
	0,0,0,0};
const uint8_t tetris_S1[] = {
	1,0,0,0,
	1,1,0,0,
	0,1,0,0,
	0,0,0,0};
const uint8_t tetris_S2[] = {
	0,1,1,0,
	1,1,0,0,
	0,0,0,0,
	0,0,0,0};
const uint8_t tetris_S3[] = {
	1,0,0,0,
	1,1,0,0,
	0,1,0,0,
	0,0,0,0};

const uint8_t tetris_Z0[] = {
	1,1,0,0,
	0,1,1,0,
	0,0,0,0,
	0,0,0,0};
const uint8_t tetris_Z1[] = {
	0,1,0,0,
	1,1,0,0,
	1,0,0,0,
	0,0,0,0};
const uint8_t tetris_Z2[] = {
	1,1,0,0,
	0,1,1,0,
	0,0,0,0,
	0,0,0,0};
const uint8_t tetris_Z3[] = {
	0,1,0,0,
	1,1,0,0,
	1,0,0,0,
	0,0,0,0};

const uint8_t tetris_L0[] = {
	0,1,0,0,
	0,1,0,0,
	0,1,1,0,
	0,0,0,0};
const uint8_t tetris_L1[] = {
	0,0,1,0,
	1,1,1,0,
	0,0,0,0,
	0,0,0,0};
const uint8_t tetris_L2[] = {
	1,1,0,0,
	0,1,0,0,
	0,1,0,0,
	0,0,0,0};
const uint8_t tetris_L3[] = {
	0,0,0,0,
	1,1,1,0,
	1,0,0,0,
	0,0,0,0};

const uint8_t tetris_J0[] = {
	0,1,0,0,
	0,1,0,0,
	1,1,0,0,
	0,0,0,0};
const uint8_t tetris_J1[] = {
	0,0,0,0,
	1,1,1,0,
	0,0,1,0,
	0,0,0,0};
const uint8_t tetris_J2[] = {
	0,1,1,0,
	0,1,0,0,
	0,1,0,0,
	0,0,0,0};
const uint8_t tetris_J3[] = {
	1,0,0,0,
	1,1,1,0,
	0,0,0,0,
	0,0,0,0};

const uint8_t tetris_T0[] = {
	0,0,0,0,
	1,1,1,0,
	0,1,0,0,
	0,0,0,0};
const uint8_t tetris_T1[] = {
	0,1,0,0,
	0,1,1,0,
	0,1,0,0,
	0,0,0,0};
const uint8_t tetris_T2[] = {
	0,1,0,0,
	1,1,1,0,
	0,0,0,0,
	0,0,0,0};
const uint8_t tetris_T3[] = {
	0,1,0,0,
	1,1,0,0,
	0,1,0,0,
	0,0,0,0};

// tetris 列表
const uint8_t *tetris_list[7][4] = {
	{tetris_I0, tetris_I1, tetris_I2, tetris_I3},
	{tetris_O0, tetris_O1, tetris_O2, tetris_O3},
	{tetris_S0, tetris_S1, tetris_S2, tetris_S3},
	{tetris_Z0, tetris_Z1, tetris_Z2, tetris_Z3},
	{tetris_L0, tetris_L1, tetris_L2, tetris_L3},
	{tetris_J0, tetris_J1, tetris_J2, tetris_J3},
	{tetris_T0, tetris_T1, tetris_T2, tetris_T3} };

void print_tetris(const uint8_t tetris[]) {
	int i;
	for (i=1;i<=16;i++) {
		if (tetris[i-1] == 0)
			putchar(EMPTY_SPACE);
		else
			putchar(BLOCK_CHAR);
		if (i != 0 && (i % 4) == 0) // 每打印四个换一次行
			putchar('\n');
	}
}

void print_tetris_all(void) {
	fflush(stdout);
	int i,j;
	for (i=0;i<7;i++) {
		for (j=0;j<4;j++) {
			print_tetris(tetris_list[i][j]);
			putchar('\n');
		}
	}
	fflush(stdout);
}

uint8_t tetris_type = 0; // 类型
uint8_t tetris_orie = 0; // 方向
		     // tetris_list[类型][方向]
		     

// 存储进入游戏前的终端状态
struct termios saveterm;

void restore_termios(void) {
	if (tcsetattr(STDIN_FILENO,TCSANOW,&saveterm) < 0) {
		fprintf(stderr,"tcsetattr failed: ");
		perror("");
	}
}

int random_seed = 0;
void init(void) {
	// 初始化随机数
	srand(random_seed);

	// 保存进入游戏时的终端信息
	if (tcgetattr(STDIN_FILENO,&saveterm) < 0) {
		fprintf(stderr,"tcgetattr failed: ");
		perror("");
		exit(EXIT_FAILURE);
	}
	// 意外中断后恢复终端
	atexit(restore_termios);

	// 本游戏内的终端属性
	struct termios thisgame;
	if (tcgetattr(STDIN_FILENO,&thisgame) < 0) {
		fprintf(stderr,"tcgetattr failed: ");
		perror("");
		exit(EXIT_FAILURE);
	}

	thisgame.c_lflag &= ~(ECHO |ICANON); // 输入无回显，无缓冲
	
	if (tcsetattr(STDIN_FILENO,TCSANOW,&thisgame) < 0) {
		fprintf(stderr,"tcsetattr failed: ");
		perror("");
		exit(EXIT_FAILURE);
	}

	return;
}


// 当前tetris在board中的位置
int pos_x = 0;
int pos_y = 0;

// 重叠检测
int tetris_collision(void) {
	const uint8_t *tetris = tetris_list[tetris_type][tetris_orie];
	uint8_t dest[16];
	int i;

	for (i=0;i<16;i++) {
		dest[i] = 0;
	}

	int x = pos_x;
	int y = pos_y;

	// 把board上的tetris所在4x4区域读取进dest
	for (i=1;i<=16;i++) {
		dest[i-1] = board[x][y];
		x++;
		if (i != 0 && (i % 4) == 0) {
			y++;
			x=pos_x;
		}
	}

	// 比对tetris和dest如果有重叠则返回
	for (i=1;i<=16;i++) {
		if (dest[i-1] != 0 && tetris[i-1] != 0)
			return 1;
		x++;
		if (i != 0 && (i % 4) == 0) {
			y++;
			x=pos_x;
		}
	}
	return 0;
}


// 在board中当前的坐标画出tetris
void tetris_draw(void) {
	const uint8_t *tetris = tetris_list[tetris_type][tetris_orie];
	int i;
	int x = pos_x;
	int y = pos_y;
	for (i=1;i<=16;i++) {
		if (tetris[i-1] != 0)
			board[x][y] = 1;
		x++;
		if (i != 0 && (i % 4) == 0) {
			y++;
			x=pos_x;
		}
	}
}
// 在board中擦除当前坐标的tetris
void tetris_undraw(void) {
	const uint8_t *tetris = tetris_list[tetris_type][tetris_orie];
	int i;
	int x = pos_x;
	int y = pos_y;

	for (i=1;i<=16;i++) {
		if (tetris[i-1] != 0)
			board[x][y] = 0;
		x++;
		if (i != 0 && (i % 4) == 0) {
			y++;
			x=pos_x;
		}
	}
}

int8_t move  = 0;

// 左右移动tetris
void tetris_move(void) {
	int8_t origin_x = pos_x; // 保存原来列位置
	tetris_undraw(); 
	if (move < 0) {
		pos_x--;
		move = 0;
	}

	if (move > 0) {
		pos_x++;
		move = 0;
	}
	if (tetris_collision() == 1) {
		pos_x = origin_x;
	}
	tetris_draw();
}

void gameover(void) {
	printf ("\033[1H\033[2J");
	score_print();
	pause();
}

// 在顶部随机生成tetris
void tetris_random(void) {
	tetris_type = (uint8_t)rand() % 7;
	tetris_orie = (uint8_t)rand() & 3;

	pos_y = 0;
	pos_x = X_MAX/2;

	if (tetris_collision() == 1) {
		gameover();
	}
	tetris_draw();
	ui_print();
}

void tetris_movedown(void) {
	int8_t origin_y = pos_y; // 保存原来行位置
	tetris_undraw(); 
	pos_y++;
	if (tetris_collision() == 1) {
		pos_y = origin_y;
		tetris_draw();
		board_action();
		tetris_random();
	} else {
		tetris_draw();
	}
	ui_print();
}

uint8_t tetris_orie_next(void) {
	// 那肯定是顺时针
	if (tetris_orie == 0) {
		return 3;
	}
	return tetris_orie-1;
}

void tetris_rotate(void) {
	uint8_t origin_orie = tetris_orie;

	tetris_undraw();
	tetris_orie = tetris_orie_next();
	if (tetris_collision() == 1) {
		tetris_orie = origin_orie; // 恢复
	}
	tetris_draw();
	ui_print();
}

void getkey(void) {
	int c;
	c = getchar();
	if (c == 'w') {
		tetris_rotate();
	} else if (c == 'a') {
		move = -1;
		tetris_move();
	} else if (c == 's') {
		tetris_movedown();
	} else if (c == 'd') {
		move = 1;
		tetris_move();
	}
}

// 定时任务
void alarm_handler(int signo) {
	tetris_movedown();
}

#define SPEED 500000

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr,"Usage: %s seed\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	random_seed = atoi(argv[1]);

	signal(SIGALRM,alarm_handler);
	ualarm(SPEED,SPEED);

	init();
	board_init();
	tetris_random();
	ui_print;
	while (1) {
		getkey();
	}
	return 0;
}
