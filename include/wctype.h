#ifndef _WCTYPE_H
#define _WCTYPE_H

#define _NEED_WEOF
#define _NEED_WINT
#include <sys/cdefs.h>
#include <arch/cdefs.h>

typedef const int *wctrans_t;
typedef unsigned long wctype_t;

extern int iswalnum(wint_t wc);
extern int iswalpha(wint_t wc);
extern int iswblank(wint_t wc);
extern int iswcntrl(wint_t wc);
extern int iswdigit(wint_t wc);
extern int iswgraph(wint_t wc);
extern int iswlower(wint_t wc);
extern int iswprint(wint_t wc);
extern int iswpunct(wint_t wc);
extern int iswspace(wint_t wc);
extern int iswupper(wint_t wc);
extern int iswxdigit(wint_t wc);
extern int iswctype(wint_t wc, wctype_t desc);
extern wctype_t wctype(const char *property);
extern wint_t towlower(wint_t wc);
extern wint_t towupper(wint_t wc);
extern wint_t towctrans(wint_t wc, wctrans_t desc);
extern wctrans_t wctrans(const char *property);

#endif
