#include "mulle-qsort.h"
/*
 * The actual mulle_qsort() implementation is static inline. The hope is that
 * a clever compiler can improve the code better this way as it an reorganize
 * recursive _mulle_qsort_r calls. (voodoo ?)
 */


void   mulle_qsort_r( void *a, size_t n, size_t es, mulle_qsort_cmp_t *cmp, void *thunk)
{
   if( a && cmp)
      _mulle_qsort_r_inline( a, n, es, cmp, thunk);
}