#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <scc/mach.h>

#include "libmach.h"

extern newfun_t newv[];

Obj *
objnew(int type)
{
	Obj *obj;
	int fmt;
	newfun_t fn;

	fmt = FORMAT(type);
	if (fmt >= NFORMATS)
		return NULL;

	if ((obj = malloc(sizeof(*obj))) == NULL)
		return NULL;

	obj->type = type;
	obj->head = NULL;
	obj->sections = NULL;
	memset(obj->htab, 0, sizeof(obj->htab));

	fn = newv[fmt];
	if ((*fn)(obj) < 0) {
		free(obj);
		return NULL;
	}

	return obj;
}