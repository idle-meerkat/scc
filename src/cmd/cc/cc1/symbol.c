#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <scc/cstd.h>
#include <scc/scc.h>
#include "cc1.h"

#define NR_SYM_HASH 64
#define NR_CPP_HASH 32
#define NR_LBL_HASH 16

struct keyword {
	char *str;
	unsigned char token, value;
};

unsigned curctx;
static unsigned short counterid;

static Symbol *head, *labels;
static Symbol *htab[NR_SYM_HASH];
static Symbol *htabcpp[NR_CPP_HASH];
static Symbol *htablbl[NR_LBL_HASH];

#ifndef NDEBUG
void
dumpstab(Symbol **tbl, char *msg)
{
	Symbol **bp, *sym;
	unsigned size;

	fprintf(stderr, "Symbol Table dump at ctx=%u\n%s\n", curctx, msg);
	if (tbl == htab)
		size = NR_SYM_HASH;
	else if (tbl == htabcpp)
		size = NR_CPP_HASH;
	else if (tbl == htablbl)
		size = NR_LBL_HASH;
	else
		abort();

	for (bp = tbl; bp < &tbl[size]; ++bp) {
		if (*bp == NULL)
			continue;
		fprintf(stderr, "%d", (int) (bp - htab));
		for (sym = *bp; sym; sym = sym->hash)
			fprintf(stderr, "->[%d,%d:'%s'=%p]",
			        sym->ns, sym->ctx, sym->name, (void *) sym);
		putc('\n', stderr);
	}
	fputs("head:", stderr);
	for (sym = head; sym; sym = sym->next) {
		fprintf(stderr, "->[%d,%d:'%s'=%p]",
		        sym->ns, sym->ctx,
		        (sym->name) ? sym->name : "", (void *) sym);
	}
	fputs("\nlabels:", stderr);
	for (sym = labels; sym; sym = sym->next) {
		fprintf(stderr, "->[%d,%d:'%s'=%p]",
		        sym->ns, sym->ctx,
		        (sym->name) ? sym->name : "", (void *) sym);
	}
	putc('\n', stderr);
}
#endif

static Symbol **
hash(char *s, int ns)
{
	unsigned h, size;
	Symbol **tab;

	h = genhash(s);

	switch (ns) {
	case NS_CPP:
		tab = htabcpp;
		size = NR_CPP_HASH-1;
		break;
	case NS_LABEL:
		tab = htablbl;
		size = NR_LBL_HASH-1;
		break;
	default:
		tab = htab;
		size = NR_SYM_HASH-1;
		break;
	}
	return &tab[h & size];
}

static void
unlinkhash(Symbol *sym)
{
	Symbol **h;

	if ((sym->flags & SDECLARED) == 0)
		return;
	h = hash(sym->name, sym->ns);
	assert(sym->ns == NS_CPP || *h == sym);
	while (*h != sym)
		h = &(*h)->hash;
	*h = sym->hash;
}

void
pushctx(void)
{
	DBG("SYM: pushed context %d", curctx+1);
	if (++curctx == NR_BLOCK+1)
		error("too many nested blocks");
}

void
killsym(Symbol *sym)
{
	short f;
	char *name;

	if (!sym)
		return;
	f = sym->flags;
	if (f & SSTRING)
		free(sym->u.s);
	if (sym->ns == NS_TAG)
		sym->type->prop &= ~TDEFINED;
	unlinkhash(sym);
	if ((name = sym->name) != NULL) {
		switch (sym->ns) {
		case NS_LABEL:
			if ((f & SDEFINED) == 0)
				errorp("label '%s' is not defined", name);
		case NS_IDEN:
			if ((f & (SUSED|SGLOBAL|SDECLARED)) == SDECLARED)
				warn("'%s' defined but not used", name);
			break;
		}
	}
	free(name);
	free(sym);
}

void
popctx(void)
{
	Symbol *next, *sym;
	int ns, dangling = 0;

	DBG("SYM: popped context %d", curctx);
	/*
	 * we have to be careful before popping the current
	 * context, because since the parser is one token
	 * ahead it may already have read an identifier at
	 * this point, and yylval.sym is a pointer to
	 * the symbol associated to such token. If that
	 * symbol is from the context that we are popping
	 * then we are going to generate a dangling pointer.
	 * We can detect this situation and call again to
	 * lookup.
	 */
	if ((yytoken == IDEN || yytoken == TYPEIDEN) &&
	    yylval.sym->ctx == curctx) {
		ns = yylval.sym->ns;
		dangling = 1;
	}

	for (sym = head; sym && sym->ctx == curctx; sym = next) {
		/*
		 * Since we are unlinking them in the inverse order
		 * we do know that sym is always the head of the
		 * collision list
		 */
		next = sym->next;
		killsym(sym);
	}
	head = sym;

	if (--curctx == GLOBALCTX) {
		for (sym = labels; sym; sym = next) {
			next = sym->next;
			killsym(sym);
		}
		labels = NULL;
	}

	if (dangling) {
		yylval.sym = lookup(ns, yytext, ALLOC);
		yytoken = yylval.sym->token;
	}
}

unsigned
newid(void)
{
	unsigned short id;

	if (lexmode == CPPMODE)
		return 0;
	id = ++counterid;
	if (id == 0) {
		die("cc1: overflow in %s identifiers",
		    (curctx) ? "internal" : "external");
	}
	return id;
}

Symbol *
newsym(int ns, char *name)
{
	Symbol *sym;

	sym = xmalloc(sizeof(*sym));
	if (name)
		name = xstrdup(name);
	sym->name = name;
	sym->id = 0;
	sym->hide = 0;
	sym->ns = ns;
	sym->ctx = curctx;
	sym->token = IDEN;
	sym->flags = 0;
	sym->u.s = NULL;
	sym->type = NULL;
	sym->hash = NULL;

	if (ns == NS_LABEL) {
		sym->next = labels;
		labels = sym;
	} else if (ns != NS_CPP) {
		sym->next = head;
		head = sym;
	}
	return sym;
}

static Symbol *
linkhash(Symbol *sym)
{
	Symbol **h;

	h = hash(sym->name, sym->ns);
	sym->hash = *h;
	*h = sym;

	if (sym->ns != NS_CPP)
		sym->id = newid();
	sym->flags |= SDECLARED;
	return sym;
}

Symbol *
newstring(char *s, size_t len)
{
	Symbol *sym = newsym(NS_IDEN, NULL);

	if (lexmode != CPPMODE)
		sym->type = mktype(chartype, ARY, len, NULL);
	sym->id = newid();
	sym->flags |= SSTRING | SCONSTANT | SPRIVATE;
	sym->u.s = xmalloc(len);
	if (s)
		memcpy(sym->u.s, s, len);

	return sym;
}

Symbol *
newlabel(void)
{
	Symbol *sym = newsym(NS_LABEL, NULL);
	sym->id = newid();
	return sym;
}

Symbol *
lookup(int ns, char *name, int alloc)
{
	Symbol *sym;
	int sns, c;
	char *t;

	c = *name;
	for (sym = *hash(name, ns); sym; sym = sym->hash) {
		t = sym->name;
		if (*t != c || strcmp(t, name))
			continue;
		sns = sym->ns;
		if (sns == ns)
			return sym;
		/*
		 * When a lookup is done in a namespace associated
		 * to a struct we also want symbols of NS_IDEN which
		 * are typedef, because in other case we cannot declare
		 * fields of such types.
		 * TODO: Remove this trick
		 */
		if (sns == NS_KEYWORD ||
		    (sym->flags & STYPEDEF) && ns >= NS_STRUCTS) {
			return sym;
		}
	}
	return (alloc == ALLOC) ? newsym(ns, name) : NULL;
}

Symbol *
install(int ns, Symbol *sym)
{
	if (sym->flags & SDECLARED || sym->ctx != curctx) {
		if (sym->ctx == curctx && ns == sym->ns)
			return NULL;
		sym = newsym(ns, sym->name);
	}
	return linkhash(sym);
}

void
keywords(struct keyword *key, int ns)
{
	Symbol *sym;

	for ( ; key->str; ++key) {
		sym = linkhash(newsym(ns, key->str));
		sym->token = key->token;
		sym->u.token = key->value;
	}
}

void
builtins(struct builtin *built)
{
	Symbol *sym;
	struct builtin *bp;

	for (bp = built; bp->str; ++bp) {
		sym = linkhash(newsym(NS_KEYWORD, bp->str));
		sym->token = BUILTIN;
		sym->u.fun = bp->fun;
	}
}

void
isyms(void)
{
	static struct keyword cppoper[] = {
		{"defined", DEFINED, DEFINED},
		{NULL, 0, 0}
	};
	static struct keyword cppkeys[] = {
		{"define", DEFINE, DEFINE},
		{"include", INCLUDE, INCLUDE},
		{"line", LINE, LINE},
		{"ifdef", IFDEF, IFDEF},
		{"if", IF, IF},
		{"elif", ELIF, ELIF},
		{"else", ELSE, ELSE},
		{"ifndef", IFNDEF, IFNDEF},
		{"endif", ENDIF, ENDIF},
		{"undef", UNDEF, UNDEF},
		{"pragma", PRAGMA, PRAGMA},
		{"error", ERROR, ERROR},
		{NULL, 0, 0}
	};
	static struct keyword lexkeys[] = {
		{"auto", SCLASS, AUTO},
		{"break", BREAK, BREAK},
		{"_Bool", TYPE, BOOL},
		{"__builtin_va_list", TYPE, VA_LIST},
		{"case", CASE, CASE},
		{"char", TYPE, CHAR},
		{"const", TQUALIFIER, CONST},
		{"continue", CONTINUE, CONTINUE},
		{"default", DEFAULT, DEFAULT},
		{"do", DO, DO},
		{"double", TYPE, DOUBLE},
		{"else", ELSE, ELSE},
		{"enum", TYPE, ENUM},
		{"extern", SCLASS, EXTERN},
		{"float", TYPE, FLOAT},
		{"for", FOR, FOR},
		{"goto", GOTO, GOTO},
		{"if", IF, IF},
		{"inline", TQUALIFIER, INLINE},
		{"int", TYPE, INT},
		{"long", TYPE, LONG},
		{"register", SCLASS, REGISTER},
		{"restrict", TQUALIFIER, RESTRICT},
		{"return", RETURN, RETURN},
		{"short", TYPE, SHORT},
		{"signed", TYPE, SIGNED},
		{"sizeof", SIZEOF, SIZEOF},
		{"static", SCLASS, STATIC},
		{"struct", TYPE, STRUCT},
		{"switch", SWITCH, SWITCH},
		{"typedef", SCLASS, TYPEDEF},
		{"union", TYPE, UNION},
		{"unsigned", TYPE, UNSIGNED},
		{"void", TYPE, VOID},
		{"volatile", TQUALIFIER, VOLATILE},
		{"while", WHILE, WHILE},
		{NULL, 0, 0},
	};

	keywords(lexkeys, NS_KEYWORD);
	keywords(cppkeys, NS_CPPCLAUSES);
	keywords(cppoper, NS_CPP);
	ibuilts();

	/*
	 * Remove all the predefined symbols from * the symbol list. It
	 * will make faster some operations. There is no problem of memory
	 * leakeage because this memory is not ever freed
	 */
	counterid = 0;
	head = NULL;
}
