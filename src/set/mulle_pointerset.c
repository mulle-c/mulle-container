//
//  mulle_pointerset.c
//  mulle-container
//
//  Created by Nat! on 03.04.16.
//  Copyright © 2016 Mulle kybernetiK. All rights reserved.
//

#include "mulle_pointerset.h"

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


static void   *mulle_pointerset_bsearch( void **buf,
                                         size_t n,
                                         void  *search)
{
   long       first;
   long       last;
   long       middle;
   void       *p;
   
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

void   *mulle_pointerset_member2( struct  mulle_pointerset *set, void *p);

void   *mulle_pointerset_member2( struct  mulle_pointerset *set, void *p)
{
   void  **result;
   
   if( ! set->n)
      return( NULL);
   
   if( ! set->sorted)
   {
      qsort( set->storage, set->n, sizeof( void *), (void *) _pointer_compare);
      set->sorted = 1;
   }
   result = mulle_pointerset_bsearch( set->storage, set->n, p);
   return( result ? *result : NULL);
}
