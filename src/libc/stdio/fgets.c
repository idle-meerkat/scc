#include <stdio.h>

#undef fgets

char *
fgets(char *restrict s, int n, FILE *restrict fp)
{
	int ch = 0;
	char *t = s;

	for (--n; n > 0; --n) {
		if ((ch = getc(fp)) == EOF)
			break;
		if ((*t++ = ch) == '\n')
			break;
	}
	if (ch == EOF && s == t)
		return NULL;
	*t = '\0';

	return s;
}
