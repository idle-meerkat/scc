#include <stdlib.h>
#include <stdio.h>

#include "../libc.h"
#include "../syscall.h"

#undef fclose

int
fclose(FILE *fp)
{
	int r = EOF;

	if ((fp->flags & _IOSTRG) == 0 &&
	    fp->flags & (_IOWRITE | _IOREAD | _IORW)) {
		r = 0;
		if (_flsbuf(fp) == EOF)
			r = EOF;
		if (_close(fp->fd) < 0)
			r = EOF;
	}

	if (fp->flags & _IOALLOC) {
		free(fp->buf);
		fp->rp = fp->wp = fp->lp = fp->buf = NULL;
	}

	fp->flags &= ~(_IOWRITE | _IOREAD | _IORW |
	               _IOERR | _IOEOF |
	               _IOALLOC |
	               _IOTXT |
	               _IOSTRG);

	return r;
}
