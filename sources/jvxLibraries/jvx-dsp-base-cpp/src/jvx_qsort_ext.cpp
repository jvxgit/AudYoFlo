#include "jvx_qsort_ext.h"

#define loc_min(a,b) ((a) < (b) ? a : b)

/*
 * Qsort routine from Bentley & McIlroy's "Engineering a Sort Function".
 */
template <class T>
void swapcode(char* parmi, char* parmj, size_t n, char* base_ptra, char* base_ptrb)
{ 		
	size_t i = (n) / sizeof(T);
	T* pi = (T*)(parmi);
	T* pj = (T*)(parmj);

	intptr_t deltapi = parmi - base_ptra;
	intptr_t deltapj = parmj - base_ptra;

	T* spi = (T*)(base_ptrb + deltapi);
	T* spj = (T*)(base_ptrb + deltapj);
	do
	{
		T t = *pi;
		*pi++ = *pj;
		*pj++ = t;
		t = *spi;
		*spi++ = *spj;
		*spj++ = t;

	} while (--i > 0);
}

#define SWAPINIT(a, es) swaptype = ((char *)a - (char *)0) % sizeof(long) || \
	es % sizeof(long) ? 2 : es == sizeof(long)? 0 : 1;
static __inline void
swapfunc(char* a, char* b, size_t n, int swaptype, void* base_ptra, void* base_ptrb)
{
	if (swaptype <= 1)
		swapcode<long>(a, b, n, (char*)base_ptra, (char*)base_ptrb);
	else
		swapcode<char>(a, b, n, (char*)base_ptra, (char*)base_ptrb);
}
void swap(void* a, void* b, size_t es, int swaptype, void* base_ptra, void* base_ptrb)
{
	if (swaptype == 0)
	{
		intptr_t deltaa2 = (char*)a - (char*)base_ptra;
		intptr_t deltab2 = (char*)b - (char*)base_ptra;
		char* a2 = (char*)base_ptrb + deltaa2;
		char* b2 = (char*)base_ptrb + deltab2;

		long t = *(long*)(a);
		*(long*)(a) = *(long*)(b);
		*(long*)(b) = t;

		// Do the same to secondary buffer
		t = *(long*)(a2);
		*(long*)(a2) = *(long*)(b2);
		*(long*)(b2) = t;
	}
	else
	{
		swapfunc((char*)a, (char*)b, es, swaptype, base_ptra, base_ptrb);
	}
}

#define vecswap(a, b, n, offset_ptr, sec_buffer) 	if ((n) > 0) swapfunc(a, b, n, swaptype, offset_ptr, sec_buffer)
static __inline char*
med3(char* a, char* b, char* c, int (*cmp)(const void*, const void*))
{
	return cmp(a, b) < 0 ?
		(cmp(b, c) < 0 ? b : (cmp(a, c) < 0 ? c : a))
		: (cmp(b, c) > 0 ? b : (cmp(a, c) < 0 ? a : c));
}

void
jvx_qsort_ext(void* aa, size_t n, size_t es, int (*cmp)(const void*, const void*), void* bref, void* aref)
{
	char* pa, * pb, * pc, * pd, * pl, * pm, * pn;
	int d, r, swaptype, swap_cnt;
	char* a = (char*)aa;
	if (aref == NULL)
	{
		aref = aa;
	}
loop:	SWAPINIT(a, es);
	swap_cnt = 0;
	if (n < 7) {
		for (pm = (char*)a + es; pm < (char*)a + n * es; pm += es)
			for (pl = pm; pl > (char*) a && cmp(pl - es, pl) > 0;
				pl -= es)
				swap(pl, pl - es, es, swaptype, aref, bref);
		return;
	}
	pm = (char*)a + (n / 2) * es;
	if (n > 7) {
		pl = (char*)a;
		pn = (char*)a + (n - 1) * es;
		if (n > 40) {
			d = (int) ((n / 8) * es);
			pl = med3(pl, pl + d, pl + 2 * d, cmp);
			pm = med3(pm - d, pm, pm + d, cmp);
			pn = med3(pn - 2 * d, pn - d, pn, cmp);
		}
		pm = med3(pl, pm, pn, cmp);
	}
	swap(a, pm, es, swaptype, aref, bref);
	pa = pb = (char*)a + es;

	pc = pd = (char*)a + (n - 1) * es;
	for (;;) {
		while (pb <= pc && (r = cmp(pb, a)) <= 0) {
			if (r == 0) {
				swap_cnt = 1;
				swap(pa, pb, es, swaptype, aref, bref);
				pa += es;
			}
			pb += es;
		}
		while (pb <= pc && (r = cmp(pc, a)) >= 0) {
			if (r == 0) {
				swap_cnt = 1;
				swap(pc, pd, es, swaptype, aref, bref);
				pd -= es;
			}
			pc -= es;
		}
		if (pb > pc)
			break;
		swap(pb, pc, es, swaptype, aref, bref);
		swap_cnt = 1;
		pb += es;
		pc -= es;
	}
	if (swap_cnt == 0) {  /* Switch to insertion sort */
		for (pm = (char*)a + es; pm < (char*)a + n * es; pm += es)
			for (pl = pm; pl > (char*) a && cmp(pl - es, pl) > 0;
				pl -= es)
				swap(pl, pl - es, es, swaptype, aref, bref);
		return;
	}
	pn = (char*)a + n * es;
	r = (int)(loc_min(pa - (char*)a, pb - pa));
	vecswap(a, pb - r, r, aref, bref);
	r = (int)(loc_min(pd - pc, pn - pd - (int)es));
	vecswap(pb, pn - r, r, aref, bref);
	if ((r = (int)(pb - pa)) > (int)es)
		jvx_qsort_ext(a, r / es, es, cmp, bref, aref);
	if ((r = (int)(pd - pc)) > (int)es) {
		/* Iterate rather than recurse to save stack space */
		a = pn - r;
		n = r / es;
		goto loop;
	}
	/*		qsort(pn - r, r / es, es, cmp);*/
}
