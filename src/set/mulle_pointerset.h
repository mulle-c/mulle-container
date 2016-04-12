//
//   mulle_pointerset.h
//  mulle-container
//
//  Created by Nat! on 03.04.16.
//  Copyright © 2016 Mulle kybernetiK. All rights reserved.
//

#ifndef mulle_pointerset_h__
#define mulle_pointerset_h__

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <mulle_allocator/mulle_allocator.h>


//
// a really simple kind of set, that just stores pointers with pointer
// equality and uses bsearch for lookup. Useful for small sets ( < 100 entries)
//
struct  mulle_pointerset
{
   size_t                   n;
   void                     **storage;
   struct mulle_allocator   *allocator;
   int                      sorted;
};


static inline void    mulle_pointerset_init( struct  mulle_pointerset *set,
                                             struct mulle_allocator *allocator)
{
   memset( set, 0, offsetof( struct  mulle_pointerset, sorted));
   set->allocator = allocator;
}


static inline void    mulle_pointerset_done( struct  mulle_pointerset *set)
{
   mulle_allocator_free( set->allocator, set->storage);
}


static inline void    mulle_pointerset_add( struct  mulle_pointerset *set, void *p)
{
   set->storage = mulle_allocator_realloc( set->allocator,
                                           set->storage,
                                           sizeof( *set->storage) * (set->n + 1));
   set->storage[ set->n++] = p;
   set->sorted = 0;
}


static inline void   *mulle_pointerset_member( struct  mulle_pointerset *set, void *p)
{
   void   **q;
   void   **sentinel;
   extern void   *mulle_pointerset_member2( struct  mulle_pointerset *set, void *p);

   if( set->n >= 12)
      return( mulle_pointerset_member2( set, p));

   q        = set->storage;
   sentinel = &q[ set->n];

   while( q < sentinel)
      if( *q++ == p)
         return( p);
   
   return( NULL);
}

#endif 
