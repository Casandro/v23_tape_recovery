#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>

typedef struct timed_char_s {
	struct timed_char_s *prev;
	struct timed_char_s *next;
	double time;
	int value;
	int cnt;
} timed_char_t;


timed_char_t *append_timed_char(timed_char_t *p, const double time, const int value)
{
	timed_char_t *n=malloc(sizeof(timed_char_t));
	memset(n, 0, sizeof(timed_char_t));
	n->time=time;
	n->value=value;
	n->cnt=1;
	if (p!=NULL) n->cnt=n->cnt+p->cnt; //Count octetts
	if (p!=NULL) p->next=n;
	n->prev=p;
	return n;
}

timed_char_t *find_first_char(timed_char_t *tc)
{
	if (tc==NULL) return NULL;
	if (tc->prev==NULL) return tc;
	return find_first_char(tc->prev);
}


int print_hex_file(FILE *file, const timed_char_t *cur, const timed_char_t *last, int ecnt)
{
	if (file==NULL) return ecnt;
	if (cur==NULL) {
		fprintf(file, "\n");
		return ecnt;
	}
	if (cur->value>=256) {
		fprintf(file, "!! ");
		ecnt=ecnt+1;
	} else {
		fprintf(file, "%02x ", cur->value);
	}
	if (cur==last) {
		fprintf(file, "\n");
		return ecnt;
	}
	return print_hex_file(file, cur->next, last, ecnt);
}

void print_bin_file(FILE *file, const timed_char_t *cur, const timed_char_t *last)
{
	if (file==NULL) return;
	if (cur==NULL) return;
	if (cur->value<256) fprintf(file, "%c", cur->value);
	if (cur==last) return;
	return print_bin_file(file, cur->next, last);
}

void process_file(const char *prefix, const timed_char_t *first, const timed_char_t *last, const int fileno)
{
	if ((first==NULL) || (last==NULL)) return;

	int octetts=last->cnt-first->cnt+1;

	size_t fn_len=strlen(prefix)+strlen("-0000.index")+8;
	char *fn=malloc(fn_len);
	memset(fn, 0, fn_len);

	snprintf(fn, fn_len-1, "%s%04d.hex", prefix, fileno);
	FILE *f=fopen(fn,"w");
	int errors=print_hex_file(f, first, last, 0);
	fclose(f);
	memset(fn, 0, fn_len);
	snprintf(fn, fn_len-1, "%s%04d.cept", prefix, fileno);
	f=fopen(fn,"w");
	print_bin_file(f, first, last);
	fclose(f);
	free(fn);
	printf("%04d %lf %lf %d %d\n", fileno, first->time, last->time, octetts, errors);
}

void search_files(const char *prefix, const timed_char_t *cur, const timed_char_t *first, const int fileno)
{
	if (cur==NULL) return;
	if (cur->next==NULL) return process_file(prefix, first, cur, fileno);
	if (first==NULL) return search_files(prefix, cur, cur, fileno);
	if (cur==first) return search_files(prefix, cur->next, cur, fileno);
	double td=cur->next->time-cur->time;
	if (td>0.1) {
		process_file(prefix, first, cur, fileno);
		return search_files(prefix, cur->next, cur->next, fileno+1);
	}
	return search_files(prefix, cur->next, first, fileno);
}

int main(int argc, char *argv[])
{
	if (argc!=2) {
		printf("Usage: %s <prefix>\n", argv[0]);
	}
	char *prefix=argv[1];
	double t=0;
	int c=0;
	timed_char_t *tc=NULL;
	while (scanf("%lf%d", &t, &c)==2) {
		tc=append_timed_char(tc, t, c);
	}
	tc=find_first_char(tc);
	search_files(prefix, tc, tc, 0);
}
