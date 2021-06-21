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
	if (b[p]>0) return 1;
	return -1;
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
	return get_sample_avg(p-BLEN/2, p+BLEN/2);
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

	int n;
	int pos=0;
	int start=-1;
	int nstart=-1;

	int oktett=0;

	double ps=0; //Previous sample

	while (pos<cnt-MINOLEN)  {
		double s=get_avg_sample(pos);
		if (start<0) { //in mode idle
			if (s<0) {
				start=pos;
				nstart=-1; //delete nstart
				oktett=0;
			}
		} else {
			if ((nstart<0) && ( (ps>0) && (s<0) )) {
				nstart=pos; //Store first high to low transistion after start bit to back track
			}
			int bitnum=(pos-start) / BLEN;
			int bitpos=(pos-start) % BLEN;
//			printf("pos-start: %d bitnum: %d, bitpos: %d, sample: %lf\n", pos-start, bitnum, bitpos, s*1000);
			if (bitpos==BLEN/2) { //Mitte des Bits
				if ( ( (bitnum==0) && (s>0) ) || //start bit high => error
				     ( (bitnum==9) && (s<0) ) ) { //stop bit low => error
					//Backtrack on framing errors
//					printf("Backtrack %d bn: %d %lf\n", start, bitnum, s);
					//if (nstart>=0) pos=nstart;
					start=-1;
					nstart=-1;
				} else
				if ( (bitnum>=1) && (bitnum<=8) ) {
					if (s>0) oktett=oktett | (1 <<(bitnum-1));
				} else
				if (bitnum==9) { //Stopp bit
					printf("%d %d\n", start, oktett);
					oktett=0;
					start=-1;
				}
			}
		}
		ps=get_avg_sample(pos); //Store previous sample;
		pos=pos+1;
	}

	return 0;
}
