/* See LICENSE file for copyright and license details. */

#define __USE_MACROS
#include <ctype.h>
#undef isalpha

int
isalpha(int c)
{
	return _ctype[(unsigned char) c] & (_U|_L);
}
