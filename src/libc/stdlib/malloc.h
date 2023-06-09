/* minimum amount of required units */
#define NALLOC 16

typedef union header Header;
union header {
	struct hdr {
		Header *next;
		size_t size;
	} h;
	/* most restrictive type fixes the union size for alignment */
	_ALIGNTYPE most;
};

extern void *_prevchunk(Header *);

extern Header *_freep;
