//
//  mulle--uniquepointers.h
//  mulle-container
//
//  Created by Nat! on 13.09.20.
//  Copyright © 2020 Mulle kybernetiK. All rights reserved.
//

#ifndef mulle__uniquepointers_h__
#define mulle__uniquepointers_h__

#include "mulle--pointers.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>


//
// A really simple kind of set, that just stores pointers with pointer
// equality and uses bsearch for lookup. Useful for small sets ( < 100 entries)
//
#define MULLE__UNIQUEPOINTERS_BASE   \
   MULLE__POINTERS_BASE;             \
   int   _sorted


struct mulle__uniquepointers
{
   MULLE__UNIQUEPOINTERS_BASE;
};


MULLE_C_NONNULL_FIRST
static inline void    _mulle__uniquepointers_init( struct mulle__uniquepointers *set)
{
   memset( set, 0, sizeof( *set));
}


MULLE_C_NONNULL_FIRST
static inline void    _mulle__uniquepointers_done( struct mulle__uniquepointers *set,
                                               struct mulle_allocator *allocator)
{
   _mulle__pointers_done( (struct mulle__pointers *) set, allocator);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline void    _mulle__uniquepointers_insert( struct mulle__uniquepointers *set,
                                                     void *p,
                                                    struct mulle_allocator *allocator)
{
   _mulle__pointers_add( (struct mulle__pointers *) set, p, allocator);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline void   *_mulle__uniquepointers_get( struct mulle__uniquepointers *set, void *p)
{
   MULLE_C_NONNULL_FIRST_SECOND
   extern void   *_mulle__uniquepointers_member2( struct mulle__uniquepointers *set, void *p);
   void          **q;
   void          **sentinel;

   if( set->used >= 12)
      return( _mulle__uniquepointers_member2( set, p));

   q        = set->storage;
   sentinel = &q[ set->used];

   while( q < sentinel)
      if( *q++ == p)
         return( p);

   return( NULL);
}


static inline unsigned int
   _mulle__uniquepointers_get_count( struct mulle__uniquepointers *set)
{
   return( _mulle__pointers_get_count( (struct mulle__pointers *) set));
}


#endif
