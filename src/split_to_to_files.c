#include <stdio.h>

#define BLEN (16)

int main(int argc, char *argv[])
{
	char *prefix="";
	if (argc==2) prefix=argv[1];
	FILE *f=NULL;
	int lt=-1;
	int o=0;
	int t=0;
	int cnt=0;
	while (scanf("%d%d", &t, &o)==2) {
		if ((lt<0) || (t-lt>BLEN*100)  || (o==0x0c)) {
			char fn[256];
			int time=t/19200;
			snprintf(fn, sizeof(fn)-1, "%s%09d.cpt", prefix, t);
			if (f!=NULL) fclose(f);
			f=fopen(fn, "w");
		}
		if (f!=NULL) fprintf(f, "%c", o);
		printf("t: %d t-lt: %d\n", t, t-lt);
		lt=t;
		cnt=cnt+1;
	}
	if (f!=NULL) fclose(f);
	printf("cnt: %d\n", cnt);
}
