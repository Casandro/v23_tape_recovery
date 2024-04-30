#include <stdio.h>
#include <stdint.h>

#define BSIZE 1024

int main(int argc, char *argv[])
{
	int16_t buffer[BSIZE];
	while (fread(buffer, sizeof(buffer), 1, stdin)>0) {
		for (int n=0; n<BSIZE; n=n+4) {
			buffer[n+2]=-buffer[n+2];
			buffer[n+3]=-buffer[n+3];
		}
		fwrite(buffer, sizeof(buffer), 1, stdout);
	}
}
