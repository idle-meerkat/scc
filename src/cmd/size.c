#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <scc/arg.h>
#include <scc/mach.h>

struct sizes {
	unsigned long long text;
	unsigned long long data;
	unsigned long long bss;
};

static int status;
static char *filename, *membname;
static int tflag;
static unsigned long long ttext, tdata, tbss, ttotal;
char *argv0;

static void
error(char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	fprintf(stderr, "size: %s: ", filename);
	if (membname)
		fprintf(stderr, "%s: ", membname);
	vfprintf(stderr, fmt, va);
	putc('\n', stderr);
	va_end(va);

	status = EXIT_FAILURE;
}

int
newsect(Objsect *secp, void *data)
{
	unsigned long long *p;
	struct sizes *sp = data;

	switch (secp->type) {
	case 'T':
		p = &sp->text;
		break;
	case 'D':
		p = &sp->data;
		break;
	case 'B':
		p = &sp->bss;
		break;
	default:
		return 1;
	}

	if (*p > ULLONG_MAX - secp->size)
		return -1;
	*p += secp->size;

	return 1;
}

void
newobject(FILE *fp, int type)
{
	Obj *obj;
	struct sizes siz;
	unsigned long long total;

	if ((obj = objnew(type)) == NULL) {
		error("out of memory");
		goto error;
	}

	if (objread(obj, fp) < 0) {
		error("file corrupted");
		goto error;
	}

	siz.text = siz.data = siz.bss = 0;
	forsect(obj, newsect, &siz);

	total = siz.text + siz.data + siz.bss;
	printf("%llu\t%llu\t%llu\t%llu\t%llx\t%s\n",
	       siz.text,
	       siz.data,
	       siz.bss,
	       total, total, filename);

	ttext += siz.text;
	tdata += siz.data;
	tbss += siz.bss;
	ttotal += total;

error:
	if (obj)
		objdel(obj);
}

static int
newmember(FILE *fp, char *name, void *data)
{
	int t;

	membname = name;
	if ((t = objtype(fp, NULL)) != -1)
		newobject(fp, t);

	return 1;
}

static void
size(char *fname)
{
	int t;
	FILE *fp;

	filename = fname;
	if ((fp = fopen(fname, "rb")) == NULL) {
		error(strerror(errno));
		return;
	}

	if ((t = objtype(fp, NULL)) != -1)
		newobject(fp, t);
	else if (archive(fp))
		formember(fp, newmember, NULL);
	else
		error("bad format");

	if (ferror(fp))
		error(strerror(errno));

	fclose(fp);
}

static void
usage(void)
{
	fputs("usage: size [-t] [file...]\n", stderr);
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	unsigned long long total;

	ARGBEGIN {
	case 't':
		tflag = 1;
		break;
	default:
		usage();
	} ARGEND

	puts("text\tdata\tbss\tdec\thex\tfilename");

	if (argc == 0) {
		size("a.out");
	} else {
		for (; *argv; ++argv)
			size(*argv);
	}

	if (tflag) {
		total = ttext + tdata + tbss;
		printf("%llu\t%llu\t%llu\t%llu\t%llx\t%s\n",
		       ttext, tdata, tbss, total, total, "(TOTALS)");
	}

	if (fflush(stdout)) {
		filename = "stdout";
		error(strerror(errno));
	}

	return status;
}