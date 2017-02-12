/* See LICENSE file for copyright and license details. */
static char sccsid[] = "@(#) ./cc1/builtin.c";

#include <stdio.h>

#include "../inc/cc.h"
#include "cc1.h"

static Node *
builtin_va_arg(Symbol *sym)
{
	Node *np, *ap;
	Type *tp;

	ap = assign();
	expect(',');
	tp = typename();

	if (!valid_va_list(ap->type)) {
		errorp("incorrect parameters for va_arg");
		return constnode(zero);
	}

	np = node(OBUILTIN, tp, ap, NULL);
	np->sym = sym;
	return np;
}

static Node *
builtin_va_copy(Symbol *sym)
{
	Node *np, *src, *dst;

	dst = assign();
	expect(',');
	src = assign();

	if (!valid_va_list(dst->type) || !valid_va_list(src->type)) {
		errorp("incorrect parameters for va_copy");
		return constnode(zero);
	}

	np = node(OBUILTIN, voidtype, dst, src);
	np->sym = sym;
	return np;
}

static Node *
builtin_va_start(Symbol *sym)
{
	Node *np, *ap, *last;

	ap = assign();
	expect(',');
	if (yytoken != IDEN)
		goto error;
	last = varnode(yylval.sym);
	next();

	if (!valid_va_list(ap->type))
		goto error;

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

	ap = assign();

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