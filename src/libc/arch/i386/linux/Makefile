.POSIX:

PROJECTDIR =../../../../..
include $(PROJECTDIR)/scripts/rules.mk
include ../../../rules.mk

GENOBJS =\
	_access.$O\
	_close.$O\
	_execve.$O\
	_exit.$O\
	_fork.$O\
	_getpid.$O\
	_kill.$O\
	_lseek.$O\
	_open.$O\
	_read.$O\
	_rename.$O\
	_sigaction.$O\
	_sys_brk.$O\
	_write.$O\
	_wait4.$O\

GENSRC = $(GENOBJS:.$O=.s)

OBJS =\
	$(GENOBJS)\
	_cerrno.$O\
	_sys_errlist.$O\

all: $(OBJS) $(CRT)

crt.$O: ../crt-posix.s

$(GENSRC): syscall.lst
	./gensys.sh $(@:.s=)

clean:
	rm -f $(GENSRC) _sys_errlist.c

include deps.mk
