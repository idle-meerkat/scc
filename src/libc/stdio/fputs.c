#include <stdio.h>

#undef fputs

int
fputs(const char *restrict bp, FILE *restrict fp)
{
	int r, ch;

	r = 0;

	while ((ch = *bp++) != '\0')
		r = putc(ch, fp);

	return r;
}
