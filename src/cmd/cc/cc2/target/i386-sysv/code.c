#include <stdio.h>
#include <stdlib.h>

#include <scc/cstd.h>
#include <scc/scc.h>

#include "arch.h"
#include "../../cc2.h"

enum segment {
	CODESEG,
	DATASEG,
	BSSSEG,
	NOSEG
};

static int curseg = NOSEG;

static void
segment(int seg)
{
	static char *txt[] = {
		[CODESEG] = "\t.text\n",
		[DATASEG] = "\t.data\n",
		[BSSSEG] = "\t.bss\n",
	};

	if (seg == curseg)
		return;
	fputs(txt[seg], stdout);
	curseg = seg;
}

static char *
symname(Symbol *sym)
{
	static char name[INTIDENTSIZ+1];

	if (sym->name) {
		switch (sym->kind) {
		case SEXTRN:
		case SGLOB:
		case SPRIV:
			return sym->name;
		}
	}

	sprintf(name, ".L%d", sym->numid);

	return name;
}

static void
emitconst(Node *np)
{
	switch (np->type.size) {
	case 1:
		printf("%d", (int) np->u.i & 0xFF);
		break;
	case 2:
		printf("%d", (int) np->u.i & 0xFFFF);
		break;
	case 4:
		printf("%ld", (long) np->u.i & 0xFFFFFFFF);
		break;
	case 8:
		printf("%lld", (long long) np->u.i & 0xFFFFFFFF);
		break;
	default:
		abort();
	}
}

static void
emittree(Node *np)
{
	if (!np)
		return;

	switch (np->op) {
	case OSTRING:
		pprint(np->u.s);
		free(np->u.s);
		np->u.s = NULL;
		break;
	case OCONST:
		emitconst(np);
		break;
	case OADDR:
		emittree(np->left);
		break;
	case OMEM:
		fputs(symname(np->u.sym), stdout);
		break;
	default:
		emittree(np->left);
		printf(" %c ", np->op);
		emittree(np->right);
		break;
	}
}
static void
size2asm(Type *tp)
{
	char *s;

	if (tp->flags & STRF) {
		s = "\t.ascii\t";
	} else {
		switch (tp->size) {
		case 1:
			s = "\t.byte\t";
			break;
		case 2:
			s = "\t.short\t";
			break;
		case 4:
			s = "\t.long\t";
			break;
		case 8:
			s = "\t.quad\t";
			break;
		default:
			s = "\t.space\t%lu,";
			break;
		}
	}
	printf(s, tp->size);
}

void
data(Node *np)
{
	size2asm(&np->type);
	emittree(np);
	putchar('\n');
}

static void
label(Symbol *sym)
{
	int seg;
	char *name = symname(sym);
	Type *tp = &sym->type;

	if (sym->type.flags & FUNF)
		seg = CODESEG;
	else if (sym->type.flags & INITF)
		seg = DATASEG;
	else
		seg = BSSSEG;
	segment(seg);

	switch (sym->kind) {
	case SEXTRN:
		printf("\t.extern\t%s\n", name);
	case SLOCAL:
		return;
	case SGLOB:
		printf("\t.global\t%s\n", name);
		if (seg == BSSSEG)
			printf("\t.comm\t%s,%lu\n", name, tp->size);
		break;
	}
	if (sym->type.align != 1)
		printf("\t.align\t%lu\n", sym->type.align );
	printf("%s:\n", name);
}

void
defglobal(Symbol *sym)
{
	label(sym);
	if (sym->kind == SEXTRN || (sym->type.flags & INITF))
		return;
	size2asm(&sym->type);
	puts("0");
}

void
deftype(Type *tp)
{
}

void
defpar(Symbol *sym)
{
}

void
defvar(Symbol *sym)
{
}

void
newfun(void)
{
}

void
writeout(void)
{
}

void
endinit(void)
{
}

void
getbblocks(void)
{
}
