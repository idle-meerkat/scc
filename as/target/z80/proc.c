static char sccsid[] = "@(#) ./as/target/i386/proc.c";

#include <stdlib.h>

#include "../../../inc/scc.h"
#include "../../as.h"
#include "../x80/proc.h"

void
iarch(void)
{
	static struct {
		char *name;
		char type;
	} regs[] = {
		"AF", AREG_AF,
		"A", AREG_A,
		"F", AREG_F,

		"BC", AREG_BC,
		"B", AREG_B,
		"C", AREG_C,

		"HL", AREG_HL,
		"H", AREG_H,
		"L", AREG_L,

		"DE", AREG_DE,
		"D", AREG_D,
		"E", AREG_E,

		"IX", AREG_IX,
		"IXL", AREG_IXL,
		"IXH", AREG_IXH,

		"IY", AREG_IY,
		"IYL", AREG_IYL,
		"IYH", AREG_IYH,

		"R", AREG_R,
		"I", AREG_I,
		"AF_", AREG_AF_,

		NULL,
	}, *bp;

	for (bp = regs; bp->name; ++bp) {
		Symbol *sym = lookup(bp->name, TREG);
		sym->argtype = bp->type;
	}
}

int
match(Op *op, Node **args)
{
	unsigned char *p;
	int arg;
	Node *np;

	if (!op->args)
		return args == NULL;

	for (p = op->args; (arg = *p) && *args; ++p) {
		if (arg & AREP)
			--p;
		switch (arg & ~AREP) {
		case AIMM8:
		case AIMM16:
		case AIMM32:
		case AIMM64:
			np = *args++;
			if (np->addr != AIMM)
				return 0;
			if (toobig(np, arg))
				return 0;
			break;
		default:
			abort();
		}
	}
	return 1;
}
