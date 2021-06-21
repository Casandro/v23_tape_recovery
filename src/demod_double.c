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
		int n;
		for (n=0; n<BSIZE/2; n++) {
			//Determine vector
			double m=(n%2)*2.0-1;
			double i=buffer[n*2  ]*m;
			double q=buffer[n*2+1]*m;
			//Determine derivative
			double i_=i-ooi;
			double q_=i-ooq;
			
			double f_=(i*q_)-(q*i_);
			double p=sqr(i)+sqr(q);
			
			if (p>1e-3) obuf[n]=f_/p*0.1; else obuf[n]=0;

			ooi=oi;
			oi=i;
			ooq=oq;
			oq=q;
		}
		fwrite(obuf, sizeof(obuf), 1, stdout);
	}
}
