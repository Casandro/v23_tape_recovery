#include <stdio.h>
#include <math.h>

#define BSIZE (2048)

double sqr(const double x)
{
	return x*x;
}

int main(int argc, char *argv[])
{
	double i[3];
	double q[3];
	for (int n=0; n<3; n++) {
		i[n]=0;
		q[n]=0;
	}
	double buffer[BSIZE];
	while (fread(buffer, sizeof(buffer), 1, stdin)>0) {
		double obuf[BSIZE/2];
		for (int n=0; n<BSIZE/2; n++) {
			//Determine vector
			if ((n%2)==0) {
				i[0]=buffer[n*2  ];
				q[0]=buffer[n*2+1];
			} else {
				i[0]=-buffer[n*2  ];
				q[0]=-buffer[n*2+1];
			}
			//Determine derivative
			double i_=i[0]-i[2];
			double q_=q[0]-q[2];

			double im=i[1];
			double qm=q[1];
			
			double f_=(im*q_)-(qm*i_);
			double p=sqr(im)+sqr(qm);

			double f=1;
			
			if (p>5e-3) f=f_/p; else f=1;
			/*if (f>1) f=1;
			if (f<-1) f=-1;*/
			obuf[n]=f*0.1;

			for (int n=2; n>0; n--) {
				q[n]=q[n-1];
				i[n]=i[n-1];
			}
		}
		fwrite(obuf, sizeof(obuf), 1, stdout);
	}
}
