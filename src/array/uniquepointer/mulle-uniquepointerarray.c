//
//  mulle-uniquepointerarray.c
//  mulle-container
//
//  Created by Nat! on 03.04.16.
//  Copyright © 2016 Mulle kybernetiK. All rights reserved.
//

#include "mulle-uniquepointerarray.h"

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
extern void *
   mulle_pointers_bsearch( void **buf, unsigned int n, void  *search);



MULLE_C_NONNULL_FIRST_SECOND
void   *_mulle_uniquepointerarray_member2( struct mulle_uniquepointerarray *set, void *p);


MULLE_C_NONNULL_FIRST_SECOND
void   *_mulle_uniquepointerarray_member2( struct mulle_uniquepointerarray *set, void *p)
{
   void  **result;

   if( ! set->_sorted)
   {
      qsort( set->_storage, _mulle_uniquepointerarray_get_count( set), sizeof( void *), (void *) _pointer_compare);
      set->_sorted = 1;
   }
   result = mulle_pointers_bsearch( set->_storage,
                                    _mulle_uniquepointerarray_get_count( set),
                                    p);
   return( result ? *result : NULL);
}
