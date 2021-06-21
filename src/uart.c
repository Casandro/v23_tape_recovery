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
	return get_sample_avg(p-BLEN/4, p+BLEN/4);
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

/*List of octets*/
typedef struct octet_s {
	struct octet_s *next; //Next octet
	struct octet_s *prev; //previous octet
	int time; //Time in samples of the middle of the start bit
	uint8_t o; //value of the octet
	double q; //quality of the octet
	struct octet_s *start; //If this is part of a run of octetts, this points to the first octet. Otherwise NULL;
	struct octet_s *next_run; //if start!=NULL this points to the next octet;
	double q_sum; //If start!=NULL this is the added q of all octets from the start, including this one
} octet_t;

/* Basic idea:
 *  Ennumerate all the potential octetts
 *  Start with first candidate, if next candidate is to early, recursively use it to build up a run.
 *  Runs are buildt by setting up *start, next_run, and q_sum
 *  If those fields are already set, overwrite them if your q_sum is larger than q_sum
 *  	Output the octetts from *start to the current octett
 */

octet_t *add_octet(octet_t *po, const int time, const uint8_t b, const double q)
{
	octet_t *o=malloc(sizeof(octet_t));
	memset(o, 0, sizeof(octet_t));
	o->time=time;
	o->o=b;
	o->q=q;
	o->prev=po;
	if (po!=NULL) po->next=o;
	return o;
}


octet_t *first_octet(octet_t *p)
{
	if (p==NULL) return p;
	if (p->prev==NULL) return p;
	return first_octet(p->prev);
}

void delete_run(octet_t *p)
{
	return;
	if (p==NULL) return;
	p->start=NULL;
	p->q_sum=p->q;
	return delete_run(p->next_run);
}

double idle_between_octets(const octet_t *p, const octet_t *n)
{
	int p_end=p->time+BLEN*8+BLEN/2;
	int n_start=n->time-BLEN/2;
	return get_sample_avg(p_end, n_start)*(n_start-p_end+1);
}

void add_to_run(octet_t *p, octet_t *n)
{
	if (p==NULL) return;
	if (n==NULL) return;
	if (p->start!=NULL) delete_run(p->start); //If it's already part of another run, delete it
	n->start=p->start;
	p->next_run=n;
	n->q_sum=p->q_sum+n->q;
}

void start_run(octet_t *p)
{
	if (p==NULL) return;
	if (p->start!=NULL) return start_run(p->next); //If this is alredy part of a run, do not go further in this run, try the next
	p->start=p;
	double q_sum=p->q;
	p->q_sum=q_sum;
	octet_t *np=p->next; //Next octet we want to look at
	octet_t *pp=p; //previous octet we considered part of the run
	while (np!=NULL) {
		int dtime=np->time-pp->time;
		if (dtime>MINOLEN) { //This could be it
			//FIXME Look for best octet
			double q_idle=idle_between_octets(pp, np);
			if (np->start==NULL) { //A free octet => Make it part of our run
				add_to_run(pp, np);
				q_sum=q_sum+np->q;
				pp=np;
			} else {
				if (np->q_sum<q_sum) { //This is part of a run that's worse than ours, but gets us there.
					add_to_run(pp, np);
					q_sum=q_sum+np->q;
					pp=np;
				} else { //our run is worse!
					delete_run(p->start);
					return start_run(p->next);
				}
			}
		}
		np=np->next;
	}
	//Handle left over octets
	return start_run(p->next);

}


void print_octet(const octet_t *p)
{
	printf("%d %d\n", p->time, p->o);
}

void print_run(const octet_t *p)
{
	if (p==NULL) return;
	print_octet(p);
	return print_run(p->next_run);
}


octet_t *find_best_run(octet_t *p)
{
	octet_t *best=p;
	octet_t *n=p;
	while (n!=NULL) {
		if (n->start==NULL) printf("start==NULL %d\n", n->time);
		if (n->q_sum>best->q_sum) {
			best=n;
		}
		n=n->next;
	}
	return best->start;
}

void print_best_run(octet_t *p)
{
	octet_t *best=find_best_run(p);
	print_run(best);
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
	octet_t *o=NULL;
	for (n=0; n<cnt; n++) {
		double q=quality(n);
		int b=get_byte(n);
		if (b!=ob) { //Different octett
			if (ob>=0) {
				if (scnt>BLEN-BLEN/2) {
					o=add_octet(o, bt, ob, bq*scnt);
				}
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
	}
	octet_t *first=first_octet(o);

	start_run(first);
	print_best_run(first);

	return 0;
}
