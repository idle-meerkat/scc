#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <scc/ar.h>
#include <scc/arg.h>
#include <scc/mach.h>
#include <scc/scc.h>

#include "sys.h"

#define NR_SYMDEF 32

typedef struct symdef Symdef;

struct symdef {
	char *name;
	int type;
	long offset;
	Symdef *hash, *next;
};

static char *namidx;
static long nsymbols;
static int status, artype, nolib;
static Objops *ops;
static char *filename, *membname;
static Symdef *htab[NR_SYMDEF], *head;
static long offset;
char *argv0;

static char *
errstr(void)
{
	return strerror(errno);
}

static void
error(char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	fprintf(stderr, "ranlib: %s: ", filename);
	if (membname)
		fprintf(stderr, "%s: ", membname);
	vfprintf(stderr, fmt, va);
	putc('\n', stderr);
	va_end(va);

	status = EXIT_FAILURE;
}

Symdef *
lookup(char *name)
{
	unsigned h;
	Symdef *dp;
	char *s;
	size_t len;

	h = genhash(name) % NR_SYMDEF;

	for (dp = htab[h]; dp; dp = dp->next) {
		if (!strcmp(dp->name, name))
			return dp;
	}

	len = strlen(name) + 1;
	dp = malloc(sizeof(*dp));
	s = malloc(len);
	if (!dp || !s) {
		free(s);
		free(dp);
		return NULL;
	}

	nsymbols++;
	dp->name = s;
	memcpy(dp->name, name, len);
	dp->type = 'U';
	dp->offset = -1;
	dp->hash = htab[h];
	htab[h] = dp;
	dp->next = head;
	head = dp;

	return dp;
}

static int
newsymbol(Objsym *sym)
{
	Symdef *np;

	if (!isupper(sym->type) || sym->type == 'N')
		return 1;

	if ((np = lookup(sym->name)) == NULL) {
		error("out of memory");
		return 0;
	}

	switch (np->type) {
	case 'C':
		if (sym->type == 'C')
			break;
	case 'U':
		np->type = sym->type;
		np->offset = offset;
		break;
	default:
		if (sym->type != 'C') {
			error("multiple definitions of '%s'", sym->name);
			return 0;
		}
	}

	return 1;
}

static void
freehash(void)
{
	Symdef **npp, *next, *np;

	for (npp = htab; npp < &htab[NR_SYMDEF]; npp++)
		*npp = NULL;

	for (np = head; np; np = next) {
		next = np->next;
		free(np->name);
		free(np);
	}

	head = NULL;
}

static int
newmember(FILE *fp, char *nam)
{
	int t, ret = 0;
	Obj *obj;
	Objsym *sym;

	membname = nam;
	offset = ftell(fp);

	if (offset == EOF) {
		error(errstr());
		return 0;
	}

	t = objtype(fp, NULL);
	if (t == -1 || artype != -1 && artype != t) {
		nolib = 1;
		return 0;
	}
	artype = t;

	if ((obj = objnew(t)) == NULL) {
		error("out of memory");
		return 0;
	}
	ops = obj->ops;
	namidx = obj->index;

	if ((*ops->read)(obj, fp) < 0) {
		error("file corrupted");
		goto error;
	}

	for (sym = obj->syms; sym; sym = sym->next) {
		if (!newsymbol(sym))
			goto error;
	}

	ret = 1;

error:
	(ops->del)(obj);
	return ret;
}

static int
readsyms(FILE *fp)
{
	long r, off;
	char memb[SARNAM+1];

	if (!archive(fp)) {
		error("file format not recognized");
		return 0;
	}

	if ((off = armember(fp, memb)) < 0)
		goto corrupted;

	if (strcmp(memb, "/") != 0 && strcmp(memb, "__.SYMDEF") != 0) {
		if (fseek(fp, -off, SEEK_CUR) == EOF) {
			error(errstr());
			return 0;
		}
	}

	while ((r = armember(fp, memb)) > 0)
		newmember(fp, memb);
	if (r < 0)
		goto corrupted;
	return 1;

corrupted:
	error("corrupted ar file");
	return 0;
}

static int
merge(FILE *to, struct fprop *prop, FILE *lib, FILE *idx)
{
	int c;
	char mtime[13];
	struct ar_hdr first;

	rewind(lib);
	rewind(idx);
	fseek(lib, SARMAG, SEEK_SET);

	if (fread(&first, sizeof(first), 1, lib) != 1)
		return 0;

	if (!strncmp(first.ar_name, namidx, SARNAM))
		fseek(lib, atol(first.ar_size), SEEK_CUR);
	else
		fseek(lib, SARMAG, SEEK_SET);

	fwrite(ARMAG, SARMAG, 1, to);

        strftime(mtime, sizeof(mtime), "%s", gmtime(&prop->time));
        fprintf(to,
                "%-16.16s%-12s%-6u%-6u%-8lo%-10ld`\n",
                namidx,
                mtime,
                prop->uid,
                prop->gid,
                prop->mode,
                prop->size);

	while ((c = getc(idx)) != EOF)
		putc(c, to);
	if (prop->size & 1)
		putc('\n', to);

	while ((c = getc(lib)) != EOF)
		putc(c, to);

	fflush(to);

	if (ferror(to) || ferror(lib) || ferror(idx))
		return 0;

	return 1;
}

static int
copy(FILE *from, char *fname)
{
	int c, ret;
	FILE *fp;

	if ((fp = fopen(fname, "wb")) == NULL)
		return 0;

	rewind(from);
	while ((c = getc(from)) != EOF)
		putc(c, fp);
	fflush(fp);

	ret = !ferror(fp) && !ferror(from);

	fclose(fp);

	return ret;
}


static void
ranlib(char *fname)
{
	long *offs, i;
	char **names;
	FILE *fp, *idx, *out;
	Symdef *dp;
	struct fprop prop;

	errno = 0;
	nolib = 0;
	artype = -1;
	nsymbols = 0;
	offs = NULL;
	names = NULL;
	filename = fname;
	freehash();

	fp = fopen(fname, "rb");
	idx = tmpfile();
	out = tmpfile();
	if (!fp || !idx || !out)
		goto error;

	if (!readsyms(fp))
		goto error;

	if (nolib || nsymbols == 0)
		goto error;

	offs = malloc(sizeof(long) * nsymbols);
	names = malloc(sizeof(*names) * nsymbols);

	for (dp = head, i = 0; i < nsymbols; dp = dp->next, i++) {
		offs[i] = dp->offset;
		names[i] = dp->name;
	}

	if ((*ops->setidx)(nsymbols, names, offs, idx) < 0)
		goto error;

	if (getstat(fname, &prop) < 0)
		goto error;
	prop.size = ftell(idx);
	prop.time = time(NULL);

	if (!merge(out, &prop, fp, idx))
		goto error;

	free(offs);
	free(names);
	fclose(fp);
	fclose(idx);
	offs = NULL;
	names = NULL;
	fp = idx = NULL;

	if (!copy(out, fname))
		goto error;
	fclose(out);

	return;

error:
	if (offs)
		free(offs);
	if (names)
		free(names);
	if (errno)
		error(errstr());
	if (idx)
		fclose(idx);
	if (out)
		fclose(out);
	if (fp)
		fclose(fp);
}

static void
usage(void)
{
	fputs("usage: ranlib [-t] file...\n", stderr);
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	ARGBEGIN {
	case 't':
		break;
	default:
		usage();
	} ARGEND

	if (argc == 0)
		usage();

	for (; *argv; ++argv)
		ranlib(*argv);

	return status;
}
