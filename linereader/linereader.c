#include <stdio.h>
#include <stdlib.h>

#define MAXLINES 8192

static off_t linepos[MAXLINES];
static int sp = 0;

static char *filename = NULL;
static FILE *file = NULL;

static int load(void) {
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	off_t pos;

	while ((nread = getline(&line,&len,file)) != -1) {
		pos = ftell(file);
		pos -= nread;
		linepos[sp] = pos;
		++sp;
	}
	free(line);
	return 1;
}

static int dumpline(FILE *output,int linenum) {
	if (output == NULL) {
		return -1;
	}
	int i;
	char *line = NULL;
	size_t len = 0;
	ssize_t nread = 0;
	if (linenum >= sp) {
		return -1;
	}
	if (fseek(file,linepos[linenum],SEEK_SET) == -1) {
		return -1;
	}
	if ((nread = getline(&line,&len,file)) == -1) {
		return -1;
	}
	fprintf(output,"%s",line);
	free(line);
	return 1;
}

#define MAXINPUT 512

static char cmd[MAXINPUT];
static int current_line = 0;

static void printcurline(void) {
	if (current_line < sp && current_line >= 0) {
		dumpline(stdout,current_line);
	} else {
		fprintf(stderr,"can't print line\n");
	}
}

static void nextline(void) {
	if (current_line < sp) {
		++current_line;
		printcurline();
	} else {
		fprintf(stderr,"no next line!\n");
	}
}

static void prevline(void) {
	if (current_line > 0) {
		--current_line;
		printcurline();
	} else {
		fprintf(stderr,"not prev line!\n");
	}
}

static void setcurline(int linenum) {
	if (linenum > 0 && linenum < sp) {
		current_line = linenum-1;
	} else {
		fprintf(stderr,"can't set current line num!\n");
	}
}

static void commands(void) {
	while (fgets(cmd,MAXINPUT,stdin) != NULL) {
		if (cmd[0] == '\n' || cmd[0] == '=') {
			nextline();
		} else if (cmd[0] == 'p') {
			printcurline();
		} else if (cmd[0] == '-') {
			prevline();
		} else if (cmd[0] == ':') {
			cmd[0] = ' ';
			setcurline(atoi(cmd));
		} else if (cmd[0] == 'q') {
			fclose(file);
			exit(EXIT_SUCCESS);
		} else {
			fprintf(stderr,"?\n");
		}
	}
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr,"Usage: lr file \n");
		exit(EXIT_FAILURE);
	}
	filename = argv[1];
	file = fopen(filename,"r");
	if (file == NULL) {
		fprintf(stderr,"Can't open file %s ",filename);
		perror("");
		exit(EXIT_FAILURE);
	}
	if (load() == -1) {
		fprintf(stderr,"Can't read file %s ",filename);
		perror("");
		exit(EXIT_FAILURE);
	}

	commands();

	exit(EXIT_SUCCESS);
}