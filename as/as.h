enum secflags {
	SRELOC,
	SREAD,
	SWRITE,
	SFILE,
};

enum args {
	AIMM = 1,
	AMAX,
        AREP  = 1 << 7,
};

enum endianess {
	BIG_ENDIAN    = -1,
	LITTLE_ENDIAN = 1
};

typedef struct ins Ins;
typedef struct op Op;
typedef struct arg Arg;
typedef void Format(Op *, Arg *);
typedef struct section Section;

struct ins {
	int begin, end;
	char *str;
};

struct op {
	int flags;
	int size;
	void (*format)(Op *, Arg *);
	char *bytes;
	unsigned char *args;
};

struct arg {
	unsigned char type;
	TUINT val;
};

struct section {
	char *name;
	char *mem;
	int flags;
	TUINT base;
	TUINT max;
	TUINT curpc;
	TUINT pc;
	struct section *next;
};

extern void isections(void);
extern void writeout(char *name);
extern void emit(Section *sec, char *bytes, int nbytes);
extern Section *section(char *name);
extern void incpc(int siz);
extern char *pack(TUINT v, int n, int inc);

extern Section *cursec;
extern int nr_ins;
extern Ins instab[];
extern Op optab[];
extern int pass;
extern TUINT maxaddr;
extern int endian;