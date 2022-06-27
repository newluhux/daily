#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
	char *haystack = "FASEASFSCLSFASLKSDLJKSFAS";
	int haystack_len = strlen(haystack);

	char *needle = "FAS";
	int needle_len = strlen(needle);

	char *haystackp = haystack;
	int haystackp_len = haystack_len;

	char *result;
	while (1) {
		if (haystackp >= haystack+haystack_len)
			break;

		result = (char *)memmem(haystackp,haystackp_len,needle,needle_len);
		if (result == NULL)
			break;
		printf("%s\n",result);
		haystackp = result + needle_len;
		haystackp_len = (haystack+haystack_len)-haystackp;
	}
}