#include <stdio.h>
#include <stdint.h>

#define BSIZE 1024

#define MAXCNT (2000)

int main(int argc, char *argv[])
{
	int pbit=-1; //Previous bit value
	int cnt=0;
	int16_t buffer[BSIZE];
	while (fread(buffer, sizeof(buffer), 1, stdin)>0) {
		for (int n=0; n<BSIZE; n++) {
			int b=buffer[n];
			int bit=1;
			if (b<0) bit=0;
			printf("%d", bit);
		}
	}
}
