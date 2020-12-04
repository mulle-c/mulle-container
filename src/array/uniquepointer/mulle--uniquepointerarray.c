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


static int  _pointer_compare( void *p_a, void *p_b)
{
   intptr_t   a;
   intptr_t   b;

   a = *(intptr_t *) p_a;
   b = *(intptr_t *) p_b;
   return( a != b ? (a < b ? -1 : 1) : 0);
}


MULLE_C_NONNULL_FIRST_THIRD
void   *mulle_pointers_bsearch( void **buf, unsigned int n, void *search)
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
      qsort( set->_storage,
             _mulle__uniquepointerarray_get_count( set),
             sizeof( void *),
             (void *) _pointer_compare);
      set->_sorted = 1;
   }
   result = mulle_pointers_bsearch( set->_storage,
                                    _mulle__uniquepointerarray_get_count( set),
                                    p);
   return( result ? *result : NULL);
}
