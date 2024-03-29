#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MAXLEN (1024*1024)

uint8_t buffer[MAXLEN];

int main(int argc, char *argv[])
{
	char *prefix="";
	if (argc==2) prefix=argv[1];
	double lt=0;
	double t=0;
	int o=0;
	double start=0;
	int cnt=0;
	while (scanf("%lf%d", &t, &o)==2) {
		double td=t-lt;
		if (td>0.05) {
			if (cnt>0) {
				char fn[256];
				int s=start;
				int minute=(s/60);
				snprintf(fn, sizeof(fn)-1, "%s%02d:%05.2lf.cpt", prefix, minute, start-minute*60);
				FILE *f=fopen(fn, "w");
				fwrite(buffer, 1, cnt, f);
				fclose(f);
				printf("File: %s contains %d oktets\n", fn, cnt);
			}
			start=t;
			memset(buffer, 0, cnt);
			cnt=0;
		}
		if (cnt<MAXLEN) {
			buffer[cnt]=o;
			cnt=cnt+1;
		}
		lt=t;
	}
}
