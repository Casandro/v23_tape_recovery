#include <stdio.h>
#include <stdint.h>

#define BSIZE 1024

int main(int argc, char *argv[])
{
	double i[3];
	double q[3];
	for (int n=0; n<2; n++) {
		i[n]=0; q[n]=0;
	}
	double pavg=1e-5;
	int16_t buffer[BSIZE];
	while (fread(buffer, sizeof(buffer), 1, stdin)>0) {
		int16_t obuf[BSIZE/2];
		for (int n=0; n<BSIZE/2; n++) {
			i[0]=buffer[n*2+0]*0.00001;
			q[0]=buffer[n*2+1]*0.00001;

			double i_=i[2]-i[0];
			double q_=q[2]-q[0];

			double im=i[1];
			double qm=q[1];

			double f=im*q_-qm*i_;
			double p=im*im+qm*qm;
			pavg=(pavg*0.99)+p*0.01;

			obuf[n]=-f/pavg*1000;
			if (pavg<1e-4) obuf[n]=1000;

			for (int n=2; n>0; n--) {
				q[n]=q[n-1];
				i[n]=i[n-1];
			}
		}
		fwrite(obuf, sizeof(obuf), 1, stdout);
	}
}
