#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

/*
output:
First assert
Second assert, that must fail
0002-assert.c:32: assertion failed 'sizeof(i) < sizeof(c)'
end:
*/

void
handler(int dummy)
{
	_Exit(0);
}

int
main()
{
	int i;
	char c;

	assert(signal(SIGABRT, handler) != SIG_ERR);

	printf("First assert\n");
	assert(sizeof(i) >= sizeof(c));

	printf("Second assert, that must fail\n");
	assert(sizeof(i) < sizeof(c));

	return 0;
}
