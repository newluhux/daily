#include <stdio.h>
#include <sys/mman.h>

int have_bits(int prot,int bits) {
	printf("%d\n",sizeof(prot));
	printf("%08x\n",prot);
	printf("%08x\n",~prot);
	printf("%08x\n",~bits);
}

int main(void) {
	have_bits(PROT_READ,PROT_READ);
	have_bits(PROT_READ,PROT_WRITE);
}