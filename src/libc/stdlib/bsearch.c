#include <stdint.h>
#include <stdlib.h>

void *
bsearch(const void *key, const void *ary, size_t n, size_t size,
        int (*cmp)(const void *, const void *))
{
	int t;
	size_t mid, low, high;
	char *cur, *base = (char *) ary;

	low = 0;
	high = n - 1;
	while (low <= high) {
		mid = low + (high - low) / 2;
		cur = base + mid*size;

		if ((t = (*cmp)(key, cur)) == 0)
			return cur;
		else if (t > 0 && mid < SIZE_MAX)
			low = mid + 1;
		else if (t < 0 && mid > 0)
			high = mid - 1;
		else
			break;
	}

	return NULL;
}
