static char sccsid[] = "@(#) ./as/ins.c";

#include "../inc/scc.h"
#include "as.h"

extern Section *sabs, *sbss, *sdata, *stext;

char *
tobytes(TUINT v, int nbytes, int inc)
{
	static char buf[sizeof(TUINT)];
	int idx;

	idx = (inc < 0) ? nbytes-1 : 0;
	while (nbytes--) {
		buf[idx] = v;
		idx += inc;
		v >>= 8;
	}

	if (v)
		error("overflow in immediate value");
	return buf;
}

void
noargs(Op *op, Node **args)
{
	emit(op->bytes, op->size);
}

void
def(Node **args, int siz)
{
	Node *np;

	for ( ; np = *args; ++args) {
		Symbol *sym = np->sym;

		if (sym->flags & FRELOC)
			reloc(sym, 0, siz, siz * 8, 0);
		emit(tobytes(sym->value, siz, endian), siz);
	}
}

void
defb(Op *op, Node **args)
{
	def(args, 1);
}

void
defw(Op *op, Node **args)
{
	def(args, 2);
}

void
defd(Op *op, Node **args)
{
	def(args, 4);
}

void
defq(Op *op, Node **args)
{
	def(args, 8);
}

void
equ(Op *op, Node **args)
{
	if (!linesym)
		error("label definition lacks a label");
	else
		linesym->value = (*args)->sym->value;
}

void
section(Op *op, Node **args)
{
	Symbol *sym = args[0]->sym;

	setsec(sym->name.buf, NULL);
}

void
text(Op *op, Node **args)
{
	cursec = stext;
}

void
data(Op *op, Node **args)
{
	cursec = sdata;
}

void
bss(Op *op, Node **args)
{
	cursec = sbss;
}
