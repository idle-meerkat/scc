#include <stdio.h>

#include <scc/scc.h>
#include "cc1.h"

static Node *
builtin_va_arg(Symbol *sym)
{
	Node *np, *ap;
	Type *tp;

	ap = simplify(assign());
	expect(',');
	tp = typename();

	if (!valid_va_list(ap->type)) {
		errorp("incorrect parameters for va_arg");
		goto error;
	}
	if (tp == booltype ||
	    tp == chartype || tp == uchartype || tp == schartype ||
	    tp == shorttype || tp == ushorttype) {
		warn("bool, char and short are promoted to int when passed through '...'");
		tp = (tp->prop & TSIGNED) ? inttype : uinttype;
	}

	np = node(OBUILTIN, tp, ap, NULL);
	np->sym = sym;
	return np;

error:
	return constnode(zero);
}

static Node *
builtin_va_copy(Symbol *sym)
{
	Node *np, *src, *dst;

	dst = simplify(assign());
	expect(',');
	src = simplify(assign());

	if (!valid_va_list(dst->type) || !valid_va_list(src->type)) {
		errorp("incorrect parameters for va_copy");
		return constnode(zero);
	}

	if (dst->type != va_type)
		dst = node(OPTR, dst->type->type, dst, NULL);
	if (src->type != va_type)
		src = node(OPTR, src->type->type, src, NULL);
	np = node(OASSIGN, dst->type, dst, src);
	np = node(OCAST, voidtype, np, NULL);

	return np;
}

static Node *
builtin_va_start(Symbol *sym)
{
	Node *np, *ap, *last;
	Symbol **p;
	Type *tp;

	ap = simplify(assign());
	expect(',');
	last = assign();
	if (last->op != OSYM)
		goto error;

	if (!valid_va_list(ap->type) || !(last->sym->flags&SDECLARED))
		 goto error;

	for (p = curfun->u.pars; p && *p != last->sym; ++p)
		;
	if (!p || *p == NULL || p[1] == NULL || p[1]->type != ellipsistype)
		warn("second parameter of 'va_start' not last named argument");

	tp = last->type;
	if (tp == booltype ||
	    tp == chartype || tp == uchartype || tp == schartype ||
	    tp == shorttype || tp == ushorttype) {
		warn("last parameter before '...' must not be bool, char or short");
	}

	np = node(OBUILTIN, voidtype, ap, last);
	np->sym = sym;
	return np;

error:
	errorp("incorrect parameters for va_start");
	return constnode(zero);
}

static Node *
builtin_va_end(Symbol *sym)
{
	Node *ap, *np;

	ap = simplify(assign());

	if (!valid_va_list(ap->type)) {
		errorp("incorrect parameters for va_end");
		return constnode(zero);
	}

	np = node(OBUILTIN, voidtype, ap, NULL);
	np->sym = sym;
	return np;
}

void
ibuilts(void)
{
	struct builtin built[] = {
		{"__builtin_va_arg", builtin_va_arg},
		{"__builtin_va_copy", builtin_va_copy},
		{"__builtin_va_start", builtin_va_start},
		{"__builtin_va_end", builtin_va_end},
		{NULL}
	};
	builtins(built);
}
