/*
PATTERN:
0018-voidparam.c:27: error: a named argument is requiered before '...'
.
*/


int
a(void, int i)
{
	return i;
}

int
b(int i, void)
{
	return i;
}

int
c(void, void)
{
	return 0;
}

int
d(void, ...)
{
	return 0;
}

int
main()
{
	return 0;
}
