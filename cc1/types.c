
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <sizes.h>
#include <cc.h>
#include "cc1.h"

#define NR_TYPE_HASH 16

Type
	*voidtype = &(Type) {
		.op = VOID,
		.letter = L_VOID
	},
	*pvoidtype = &(Type) {
		.op = PTR,
		.letter = L_POINTER
	},
	*booltype = &(Type) {
		.op = INT,
		.letter = L_BOOL,
		.defined = 1,
		.u.rank = RANK_BOOL
	},
	*schartype = &(Type) {
		.op = INT,
		.letter = L_SCHAR,
		.defined = 1,
		.u.rank = RANK_SCHAR
	},
	*uchartype = &(Type) {
		.op = INT,
		.letter = L_UCHAR,
		.sign = 1,
		.defined = 1,
		.u.rank = RANK_UCHAR
	},
	*chartype = &(Type) {
		.op = INT,
		.letter = L_CHAR,
		.sign = 1,
		.defined = 1,
		.u.rank = RANK_CHAR
	},
	*ushortype = &(Type) {
		.op = INT,
		.letter = L_USHORT,
		.defined = 1,
		.u.rank = RANK_USHORT
	},
	*shortype = &(Type) {
		.op = INT,
		.letter = L_SHORT,
		.defined = 1,
		.u.rank = RANK_SHORT
	},
	*uinttype = &(Type) {
		.op = INT,
		.letter = L_UINT,
		.sign = 1,
		.defined = 1,
		.u.rank = RANK_UINT
	},
	*inttype = &(Type) {
		.op = INT,
		.letter = L_INT,
		.defined = 1,
		.u.rank = RANK_INT
	},
	*longtype = &(Type) {
		.op = INT,
		.letter = L_LONG,
		.defined = 1,
		.u.rank = RANK_LONG
	},
	*ulongtype = &(Type) {
		.op = INT,
		.letter = L_ULONG,
		.sign = 1,
		.defined = 1,
		.u.rank = RANK_ULONG
	},
	*ullongtype = &(Type) {
		.op = INT,
		.letter = L_ULLONG,
		.sign = 1,
		.defined = 1,
		.u.rank = RANK_ULLONG
	},
	*llongtype = &(Type) {
		.op = INT,
		.letter = L_LLONG,
		.defined = 1,
		.u.rank = RANK_LLONG
	},
	*floattype = &(Type) {
		.op = FLOAT,
		.letter = L_FLOAT,
		.defined = 1,
		.u.rank = RANK_FLOAT
	},
	*doubletype = &(Type) {
		.op = FLOAT,
		.letter = L_DOUBLE,
		.defined = 1,
		.u.rank = RANK_DOUBLE
	},
	*ldoubletype = &(Type) {
		.op = FLOAT,
		.letter = L_LDOUBLE,
		.defined = 1,
		.u.rank = RANK_LDOUBLE
	};

Type *
ctype(int8_t type, int8_t sign, int8_t size)
{
	if (type == DOUBLE)
		type = FLOAT, size += LONG;

	switch (type) {
	case CHAR: if (sign == 0)
					return chartype;
				return (sign == UNSIGNED) ?  uchartype : schartype;
	case VOID:            return voidtype;
	case BOOL:            return booltype;
	case INT: switch (size) {
		case 0:          return (sign == UNSIGNED) ? uinttype   : inttype;
		case SHORT:      return (sign == UNSIGNED) ? ushortype  : shortype;
		case LONG:       return (sign == UNSIGNED) ? ulongtype  : longtype;
		case LONG+LONG:  return (sign == UNSIGNED) ? ullongtype : llongtype;
		}
	case FLOAT: switch (size) {
		case 0:          return floattype;
		case LONG:       return doubletype;
		case LONG+LONG:  return ldoubletype;
		}
	}
}

Type *
mktype(Type *tp, uint8_t op, uint16_t nelem)
{
	static Type *typetab[NR_TYPE_HASH], **tbl;
	static uint8_t t, def;
	register Type *bp;
	char letter;

	if (op == PTR && tp == voidtype)
		return pvoidtype;
	t = (op  ^  (uint8_t) ((unsigned short) tp >> 3))
	         & NR_TYPE_HASH-1;
	tbl = &typetab[t];
	if (op != FTN || op != STRUCT || op != UNION || op != ENUM) {
		for (bp = *tbl; bp; bp = bp->next) {
			if (bp->type == tp && bp->op == op &&
			    (op != ARY || bp->u.nelem == nelem)) {
				return bp;
			}
		}
	}

	switch (op) {
	case PTR:    letter = L_POINTER,  def = 1; break;
	case FTN:    letter = L_FUNCTION, def = 1; break;
	case ARY:    letter = L_ARRAY,    def = nelem != 0; break;
	case ENUM:   letter = L_INT,      def = 0; break;
	case STRUCT: letter = L_STRUCT,   def = 0; break;
	/* TODO case UNION: */
	default: letter = tp->letter; /* is it possible? */
	}
	bp = xcalloc(1, sizeof(*bp));
	bp->next = *tbl;
	bp->type = tp;
	bp->op = op;
	bp->letter = letter;
	bp->defined = def;
	return *tbl = bp;
}

