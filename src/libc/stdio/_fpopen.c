#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys.h>

#include "../syscall.h"
#include "../libc.h"

FILE *
_fpopen(const char *restrict fname,
        const char *restrict mode,
        FILE * restrict fp)
{
	int i, flags, fd, rw, bin, rights;

	flags = rw = bin = 0;
	rights = 0666;

	if (mode[0] == '\0')
		goto einval;

	for (i = 1; mode[i]; ++i) {
		switch (mode[i]) {
		case '+':
			if (rw)
				goto einval;
			rw = 1;
			break;
		case 'b':
			if (bin)
				goto einval;
			bin = 1;
			break;
		case 't':
			flags |= O_EXCL | O_CLOEXEC;
			rights = 0600;
			break;
		default:
			goto einval;
		}
	}

	switch (mode[0]) {
	case 'a':
		flags |= O_APPEND | O_CREAT;
		goto wrflags;
	case 'w':
		flags |= O_TRUNC | O_CREAT;
	wrflags:
		flags |= (rw) ? O_RDWR : O_WRONLY;
		break;
	case 'r':
		flags |= (rw) ? O_RDWR : O_RDONLY;
		break;
	default:
	einval:
		errno = EINVAL;
		return NULL;
	}

	if ((fd = _open(fname, flags, rights)) < 0)
		return NULL;

	fp->buf = NULL;
	fp->fd = fd;

	if (!bin)
		fp->flags |= _IOTXT;

	switch (flags & O_ACCMODE) {
	case O_RDWR:
		fp->flags |= _IORW;
		break;
	case O_RDONLY:
		fp->flags |= _IOREAD;
		break;
	case O_WRONLY:
		fp->flags |= _IOWRITE;
		break;
	}

	fp->lp = fp->rp = fp->wp = NULL;

	return fp;
}
