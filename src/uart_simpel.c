#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>


//Länge eines Bits
#define BLEN (16)

#define MINOLEN (BLEN*10-BLEN/4)


//Länge des Buffers
#define BSIZE (BLEN*256)

//Bitrate für Zeitangabe
#define BRATE (1200)

double *b=NULL;
int cnt=0;

double get_sample(const int p)
{
	if (p<0) return 0;
	if (p>=cnt) return 0;
	return b[p];
}

double get_sample_avg(const int a, const int e)
{
	if (a==e) return get_sample(a);
	if (a>e) return get_sample_avg(e, a);
	if (a<0) return get_sample_avg(0, e);
	if (e>=cnt) return get_sample_avg(a, cnt-1);
	int c=0;
	double s=0;
	int n;
	for (n=a; n<=e; n++) {
		s=s+get_sample(n);
		c=c+1;
	}
	return s/c;
}


double get_avg_sample(const int p)
{
	return get_sample_avg(p-BLEN/8, p+BLEN/8);
}

int get_byte(int sbp)
{
	if (get_avg_sample(sbp )>0) return -1; //No start bit
	if (get_avg_sample(sbp-BLEN)<0) return -1; //No transition before start bit
	if (get_avg_sample(sbp+BLEN*9)<0) return -1; //No stopp-bit
	int byte=0;
	int n;
	for (n=0; n<8; n++) {
		if (get_avg_sample(sbp+BLEN*(n+1))>0) byte=byte | (1<<n);
	}
	return byte;
}



double quality(int sbp)
{
	double q=0;
	int n;
	for (n=-1; n<10; n++) {
		double v=get_avg_sample(sbp+BLEN*n);
		q=q+v*v;
	}
	return q;
}


int main(int argc, char *argv[])
{
	if (argc!=2) {
		fprintf(stderr, "Usage: %s filename\n", argv[0]);
		return 1;
	}
	FILE *f=fopen(argv[1], "r");
	fseek(f, 0L, SEEK_END);
	size_t fs=ftell(f);
	fseek(f, 0L, SEEK_SET);
	b=malloc(fs);
	fread(b, fs, 1, f);
	fclose(f);
	cnt=fs/sizeof(double);

	int surpress=-1;
	int n;
	int ob=-1; //previous byte
	int bt=0; //time with best quality
	double bq=0; //best quality
	int scnt=0;
	int pos=0;

	while (pos<cnt-MINOLEN)  {
		double q=quality(pos);
		int b=get_byte(pos);
		if (b!=ob) { //Different octett
			if (ob>=0) {
			//	if (scnt>BLEN-BLEN/2) {
					printf("%d %d %d\n", pos, ob, scnt);
					pos=pos+MINOLEN;
			//	}
			}
			bq=-1;
			bt=n;
			scnt=0;
		}
		if (q>bq) {
			bq=q;
			//bt=n;
		}
		ob=b;
		scnt=scnt+1;
		pos=pos+1;
	}

	return 0;
}
