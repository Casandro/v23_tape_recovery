#include <stdio.h>
#include <math.h>

#define BSIZE (2048)

double sqr(const double x)
{
	return x*x;
}

int main(int argc, char *argv[])
{
	double oi=0;
	double ooi=0;
	double oq=0;
	double ooq=0;
	double buffer[BSIZE];
	while (fread(buffer, sizeof(buffer), 1, stdin)>0) {
		double obuf[BSIZE/2];
		for (int n=0; n<BSIZE/2; n++) {
			//Determine vector
			double i=buffer[n*2  ];
			double q=buffer[n*2+1];
			if ((n%2)==0) {
				i=-i;
				q=-q;
			}
			//Determine derivative
			double i_=i-ooi;
			double q_=q-ooq;
			
			double f_=(i*q_)-(q*i_);
			double p=sqr(i)+sqr(q);

			double f=1;
			
			if (p>5e-3) f=f_/p; else f=1;
			if (f>1.5) f=1.5;
			if (f<-1.5) f=-1.5;
			obuf[n]=f*0.1;

			ooi=oi;
			oi=i;
			ooq=oq;
			oq=q;
		}
		fwrite(obuf, sizeof(obuf), 1, stdout);
	}
}
