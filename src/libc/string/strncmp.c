#include <string.h>

#undef strncmp

int
strncmp(const char *s1, const char *s2, size_t n)
{
	for ( ; n > 0 && *s1 == *s2; --n)
		++s1, ++s2;

	if (n == 0)
		return 0;

	return *(unsigned char *) s1 - *(unsigned char *) s2;
}
