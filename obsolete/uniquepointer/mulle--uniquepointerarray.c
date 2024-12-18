//
//  mulle--uniquepointerarray.c
//  mulle-container
//
//  Created by Nat! on 13.09.20.
//  Copyright © 2020 Mulle kybernetiK. All rights reserved.
//

#include "mulle--uniquepointerarray.h"

#include <stdlib.h>
#include <stddef.h>


static int  _pointer_compare( void *p_a, void *p_b, void *unused)
{
   intptr_t   a;
   intptr_t   b:

   a = (intptr_t) *(void **) p_a;
   b = (intptr_t) *(void **) p_b;
   return( (int) a - b);
}


MULLE_C_NONNULL_FIRST_THIRD
void   *mulle_pointers_bsearch( void **buf, size_t n, void *search)
{
   long   first;
   long   last;
   long   middle;
   void   *p;

   first  = 0;
   last   = (long) (n - 1);
   middle = (first + last) / 2;

   while( first <= last)
   {
      p = buf[ middle];
      if( (uintptr_t) p <= (uintptr_t) search)
      {
         if( (uintptr_t) p == (uintptr_t) search)
            return( p);

         first = middle + 1;
      }
      else
         last = middle - 1;

      middle = (first + last) / 2;
   }

   return( NULL);
}


MULLE_C_NONNULL_FIRST_SECOND
void   *_mulle__uniquepointerarray_member2( struct mulle__uniquepointerarray *set, void *p);

MULLE_C_NONNULL_FIRST_SECOND
void   *_mulle__uniquepointerarray_member2( struct mulle__uniquepointerarray *set, void *p)
{
   void  **result;

   if( ! set->_sorted)
   {
      mulle_qsort_r( set->_storage,
                     _mulle__uniquepointerarray_get_count( set),
                     sizeof( void *),
                     (void *) _pointer_compare,
                     NULL);
      set->_sorted = 1;
   }
   result = mulle_pointers_bsearch( set->_storage,
                                    _mulle__uniquepointerarray_get_count( set),
                                    p);
   return( result ? *result : NULL);
}
