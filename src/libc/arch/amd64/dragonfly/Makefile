.POSIX:

PROJECTDIR =../../../../..
include $(PROJECTDIR)/scripts/rules.mk
include ../../../rules.mk

GENOBJS =\
	_brk.$O\
	_close.$O\
	_exit.$O\
	_fork.$O\
	_getpid.$O\
	_kill.$O\
	_lseek.$O\
	_open.$O\
	_read.$O\
	_write.$O\

OBJS =\
	$(GENOBJS)\
	_sys_errlist.$O\

GENSRC = $(GENOBJS:.$O=.s)

# Rules

all: $(OBJS) $(CRT)

crt.$O: ../crt-posix.s

$(GENSRC): syscall.lst
	./gensys.sh $(@:.s=)

clean:
	rm -f $(GENSRC) _sys_errlist.c

include deps.mk
