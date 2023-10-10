#include "mulle-qsort.h"

/*
 * The actual mulle_qsort() implementation is static inline. The hope is that
 * a clever compiler can improve the code better this way as it can reorganize
 * recursive _mulle_qsort_r calls. (voodoo ?)
 * (MEMO checked with godbolt, but neither gcc nor clang 17 do it)
 */
void   mulle_qsort_r( void *a, size_t n, size_t es, mulle_qsort_cmp_t *cmp, void *thunk)
{
   if( a)
      _mulle_qsort_r_inline( a, n, es, cmp, thunk);
}