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

#define SRATE (BRATE*BLEN)

double *b=NULL;
int cnt=0;

double get_sample(const int p)
{
	if (p<0) return 0;
	if (p>=cnt) return 0;
	return b[p];
}

int get_bit(const int p)
{
	double v=get_sample(p);
	if (v>0) return 1;
	return 0;
}

int find_byte(const int start)
{
	int p=start;
before_find_start:
	while (get_bit(p)==0) {
		p=p+1;
		if (p>cnt) return -1;
	}
	
	while (get_bit(p)==1) {
		p=p+1;
		if (p>cnt) return -1;
	}
	p=p+BLEN/2; //find middle of start bit
	if (get_bit(p)!=0) {
		goto before_find_start; //if start bis is not 0, go up
	}

	int start_bit=p;

	int byte=0;
	for (int n=0; n<8; n++) {
		p=p+BLEN;
		byte=byte|(get_bit(p)<<n);
	}
	p=p+BLEN;
	if (get_bit(p)==0) {
	//	printf("FRAMING broken @%.3lf\n", p*1.0/SRATE);
		p=start_bit;
		goto before_find_start;
	} 
	printf("%.3lf %d\n", start_bit*1.0/SRATE, byte);
	goto before_find_start;
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
	find_byte(0);
	
	return 0;
}
