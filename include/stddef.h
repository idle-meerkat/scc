#ifndef _STDDEF_H
#define _STDDEF_H

#define _NEED_SIZET
#define _NEED_PTRDIFFT
#define _NEED_NULL
#define _NEED_WCHART
#include <sys/cdefs.h>
#include <arch/cdefs.h>

#define offsetof(st, m) ((size_t) &(((st *) 0)->m))

#endif
