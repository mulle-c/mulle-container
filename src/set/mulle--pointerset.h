//
//  mulle--pointerset.h
//  mulle-container
//
//  Created by Nat! on 13.09.20.
//  Copyright © 2020 Mulle kybernetiK. All rights reserved.
//

#ifndef mulle__pointerset_h__
#define mulle__pointerset_h__

#include "include.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>


//
// A really simple kind of set, that just stores pointers with pointer
// equality and uses bsearch for lookup. Useful for small sets ( < 100 entries)
//
#define MULLE__POINTERSET_BASE   \
   void           **storage;     \
   unsigned int   n;             \
   unsigned int   size;          \
   int            sorted


struct mulle__pointerset
{
   MULLE__POINTERSET_BASE;
};


MULLE_C_NONNULL_FIRST
static inline void    _mulle__pointerset_init( struct mulle__pointerset *set)
{
   memset( set, 0, offsetof( struct mulle__pointerset, sorted));
}


MULLE_C_NONNULL_FIRST
static inline void    _mulle__pointerset_done( struct mulle__pointerset *set,
                                               struct mulle_allocator *allocator)
{
   mulle_allocator_free( allocator, set->storage);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline void    _mulle__pointerset_insert( struct mulle__pointerset *set,
                                                 void *p,
                                                 struct mulle_allocator *allocator)
{
   if( set->n == set->size)
   {
      set->size += set->size;
      if( ! set->size)
         set->size = 4;
      set->storage = mulle_allocator_realloc( allocator,
                                              set->storage,
                                              sizeof( *set->storage) * set->size);
   }
   set->storage[ set->n++] = p;
   set->sorted = 0;
}


MULLE_C_NONNULL_FIRST_SECOND
static inline void   *_mulle__pointerset_get( struct mulle__pointerset *set, void *p)
{
   MULLE_C_NONNULL_FIRST_SECOND
   extern void   *_mulle__pointerset_member2( struct mulle__pointerset *set, void *p);
   void          **q;
   void          **sentinel;

   if( set->n >= 12)
      return( _mulle__pointerset_member2( set, p));

   q        = set->storage;
   sentinel = &q[ set->n];

   while( q < sentinel)
      if( *q++ == p)
         return( p);

   return( NULL);
}

static inline unsigned
   _mulle__pointerset_get_count( struct mulle__pointerset *set)
{
   return( set->n);
}


#endif
