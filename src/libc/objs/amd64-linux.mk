include objs/common-objs.mk

OBJS =\
	$(COMMON_OBJS)\
	arch/amd64/linux/__sigaction.$O\
	arch/amd64/linux/_access.$O\
	arch/amd64/linux/_cerrno.$O\
	arch/amd64/linux/_close.$O\
	arch/amd64/linux/_execve.$O\
	arch/amd64/linux/_exit.$O\
	arch/amd64/linux/_fork.$O\
	arch/amd64/linux/_getpid.$O\
	arch/amd64/linux/_getrusage.$O\
	arch/amd64/linux/_gettimeofday.$O\
	arch/amd64/linux/_kill.$O\
	arch/amd64/linux/_lseek.$O\
	arch/amd64/linux/_open.$O\
	arch/amd64/linux/_read.$O\
	arch/amd64/linux/_rename.$O\
	arch/amd64/linux/_sigreturn.$O\
	arch/amd64/linux/_sys_brk.$O\
	arch/amd64/linux/_sys_errlist.$O\
	arch/amd64/linux/_unlink.$O\
	arch/amd64/linux/_wait4.$O\
	arch/amd64/linux/_write.$O\
	arch/amd64/linux/crt.$O\
	arch/amd64/longjmp.$O\
	arch/amd64/memchr.$O\
	arch/amd64/memcmp.$O\
	arch/amd64/memcpy.$O\
	arch/amd64/memmove.$O\
	arch/amd64/memset.$O\
	arch/amd64/setjmp.$O\
	arch/amd64/strchr.$O\
	arch/amd64/strcmp.$O\
	arch/amd64/strcpy.$O\
	arch/linux/_brk.$O\
	arch/linux/_getheap.$O\
	arch/linux/_sigaction.$O\
	arch/linux/_waitpid.$O\
	arch/posix/_open.$O\
	arch/posix/_systime.$O\
	arch/posix/_tzone.$O\
	arch/posix/clock.$O\
	arch/posix/getenv.$O\
	arch/posix/raise.$O\
	arch/posix/signal.$O\
	arch/posix/system.$O\
	arch/posix/time.$O\
	string/strlen.$O\
