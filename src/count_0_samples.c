#include <stdio.h>
#include <stdint.h>

#define BSIZE 1024

#define MAXCNT (2000)

int main(int argc, char *argv[])
{
	int cnt=0;
	int16_t buffer[BSIZE];
	int counts[MAXCNT];
	for (int n=0; n<MAXCNT; n++) counts[n]=0;
	while (fread(buffer, sizeof(buffer), 1, stdin)>0) {
		for (int n=0; n<BSIZE; n++) {
			int b=buffer[n];
			if (b>0) { // 1 Bit
				if (cnt>0) {
					int c=cnt;
					if (c<0) c=0;
					if (c>=MAXCNT) c=MAXCNT-1;
					counts[c]=counts[c]+1;
					cnt=0;
				}
			} else {
				cnt=cnt+1;
			}
		}
	}
	for (int n=0; n<MAXCNT; n++) {
		printf("%4d %d\n", n, counts[n]);
	}
}
