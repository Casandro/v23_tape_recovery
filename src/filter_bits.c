#include <stdio.h>
#include <math.h>
#include <string.h>

#define BSIZE (8)

double sqr(const double x)
{
	return x*x;
}

int main(int argc, char *argv[])
{
	double buffer[BSIZE];
	memset(buffer, 0, sizeof(buffer));
	int pos=0;
	double x=0;
	double sum=0;
	while (fread(&x, sizeof(x), 1, stdin)>0) {
		sum=sum-buffer[pos]+x;
		buffer[pos]=x;
		pos=(pos+1)%BSIZE;
		double o=sum/BSIZE;
		fwrite(&o, sizeof(o), 1, stdout);
	}
}
