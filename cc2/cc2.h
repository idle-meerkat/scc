
enum tflags {
	SIGNF   =    1,
	INTF    =    2,
	STRF    =    8,
	UNIONF  =    16,
	FUNF    =    32,
	PARF    =    64,
	INITF   =   128
};

enum sclass {
	SAUTO     = 'A',
	SREG      = 'R',
	SLABEL    = 'L',
	SINDEX    = 'I',
	STMP      = 'N',
	SGLOB     = 'G',
	SEXTRN    = 'X',
	SPRIV     = 'Y',
	SLOCAL    = 'T',
	SMEMB     = 'M',
	SCONST    = '#',
	STRING    = '"',
	SNONE     = 0
};

enum types {
	ELLIPSIS = 'E',
	INT8     = 'C',
	INT16    = 'I',
	INT32    = 'W',
	INT64    = 'Q',
	UINT8    = 'K',
	UINT16   = 'N',
	UINT32   = 'Z',
	UINT64   = 'O',
	POINTER  = 'P',
	FUNCTION = 'F',
	VECTOR   = 'V',
	UNION    = 'U',
	STRUCT   = 'S',
	BOOL     = 'B',
	FLOAT    = 'J',
	DOUBLE   = 'D',
	LDOUBLE  = 'H',
	VOID     = '0'
};

enum op {
	/* kind of operand */
	/* operands */
	OMEM     = 'M',
	OTMP     = 'T',
	OAUTO    = 'A',
	OREG     = 'R',
	OCONST   = '#',
	OSTRING  = '"',
	OLOAD    = 'D',
	OLABEL   = 'L',
	OADD     = '+',
	OSUB     = '-',
	OMUL     = '*',
	OMOD     = '%',
	ODIV     = '/',
	OSHL     = 'l',
	OSHR     = 'r',
	OLT      = '<',
	OGT      = '>',
	OLE      = '[',
	OGE      = ']',
	OEQ      = '=',
	ONE      = '!',
	OBAND    = '&',
	OBOR     = '|',
	OBXOR    = '^',
	OCPL     = '~',
	OASSIG   = ':',
	ONEG     = '_',
	OCALL    = 'c',
	OPAR     = 'p',
	OFIELD   = '.',
	OCOMMA   = ',',
	OASK     = '?',
	OCOLON   = ' ',
	OADDR    = '\'',
	OAND     = 'a',
	OOR      = 'o',
	OPTR     = '@',
	OCAST    = 'g',
	OINC     = 'i',
	ODEC     = 'd',
	/*statements */
	ONOP     = 'n',
	OJMP     = 'j',
	OBRANCH  = 'y',
	ORET     = 'h',
	OBLOOP   = 'b',
	OELOOP   = 'e',
	OCASE    = 'v',
	ODEFAULT = 'f',
	OTABLE   = 't',
	OSWITCH  = 's',
};

enum nerrors {
	EEOFFUN,       /* EOF while parsing function */
	ENLABEL,       /* label without statement */
	EIDOVER,       /* identifier overflow */
	EOUTPAR,       /* out pf params */
	ESYNTAX,       /* syntax error */
	ESTACKA,       /* stack unaligned */
	ESTACKO,       /* stack overflow */
	ESTACKU,       /* stack underflow */
	ELNLINE,       /* line too long */
	EFERROR,       /* error reading from file:%s*/
	EBADID,        /* incorrect symbol id */
	ENUMERR
};

typedef struct node Node;
typedef struct type Type;
typedef struct symbol Symbol;
typedef struct addr Addr;
typedef struct inst Inst;

struct type {
	TSIZE size;
	TSIZE align;
	char flags;
};

struct symbol {
	Type type;
	Type rtype;
	unsigned short id;
	unsigned short numid;
	char *name;
	char kind;
	union {
		TSIZE off;
		Node *nlabel;
		Inst *ilabel;
	} u;
	Symbol *next;
	Symbol *h_next;
};

struct node {
	char op;
	Type type;
	char complex;
	char address;
	unsigned char flags;
	union {
		TUINT i;
		char reg;
		char *s;
		Symbol *sym;
		char subop;
	} u;
	Symbol *label;
	Node *left, *right;
	Node *stmt;
};

struct addr {
        char kind;
        union {
                char reg;
                TUINT i;
                Symbol *sym;
        } u;
};

struct inst {
        char op;
	Symbol *label;
        Addr from1, from2, to;
        Inst *next, *prev;
};

/* main.c */
extern void error(unsigned nerror, ...);

/* parse.c */
extern void parse(void);

/* optm.c */
extern void optimize(void);

/* cgen.c */
extern Node *sethi(Node *np);
extern Node *cgen(Node *np);

/* peep.c */
extern void peephole(void);

/* code.c */
extern void data(Node *np);
extern void writeout(void), endinit(void), newfun(void);
extern void code(int op, Node *to, Node *from1, Node *from2);
extern void defvar(Symbol *), defpar(Symbol *), defglobal(Symbol *);

/* node.c */
extern void apply(Node *(*fun)(Node *));
extern void cleannodes(void);
extern void delnode(Node *np);
extern void deltree(Node *np);
extern Node *newnode(void);
extern Symbol *curfun;

/* symbol.c */
#define TMPSYM  0
extern Symbol *getsym(unsigned id);
extern void popctx(void);
extern void pushctx(void);
extern void freesym(Symbol *sym);

/* globals */
extern Symbol *locals;
extern Inst *pc, *prog;
