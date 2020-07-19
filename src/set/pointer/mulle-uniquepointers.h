//
//   mulle-pointerset.h
//  mulle-container
//
//  Created by Nat! on 03.04.16.
//  Copyright © 2016 Mulle kybernetiK. All rights reserved.
//

#ifndef mulle_uniquepointers_h__
#define mulle_uniquepointers_h__

#include "mulle--uniquepointers.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>


//
// Apart from the embedded allocator, mulle_uniquepointers adds no features
// to struct mulle__uniquepointers
//
struct mulle_uniquepointers
{
   MULLE__UNIQUEPOINTERS_BASE;
   struct mulle_allocator   *allocator;
};


MULLE_C_NONNULL_FIRST
static inline void    _mulle_uniquepointers_init( struct mulle_uniquepointers *set,
                                                  struct mulle_allocator *allocator)
{
   _mulle__uniquepointers_init( (struct mulle__uniquepointers *) set);
   set->allocator = allocator;
}


static inline void    mulle_uniquepointers_init( struct mulle_uniquepointers *set,
                                                 struct mulle_allocator *allocator)
{
   if( set)
      _mulle_uniquepointers_init( set, allocator);
}


MULLE_C_NONNULL_FIRST
static inline void    _mulle_uniquepointers_done( struct mulle_uniquepointers *set)
{
   _mulle__uniquepointers_done( (struct mulle__uniquepointers *) set, set->allocator);
}


static inline void    mulle_uniquepointers_done( struct mulle_uniquepointers *set)
{
   if( set)
      _mulle_uniquepointers_done( set);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline void    _mulle_uniquepointers_insert( struct mulle_uniquepointers *set, void *p)
{
   _mulle__uniquepointers_insert( (struct mulle__uniquepointers *) set, p, set->allocator);
}


static inline void    mulle_uniquepointers_insert( struct mulle_uniquepointers *set, void *p)
{
   if( set && p)
      _mulle_uniquepointers_insert( set, p);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline void   *_mulle_uniquepointers_get( struct mulle_uniquepointers *set, void *p)
{
   return( _mulle__uniquepointers_get( (struct mulle__uniquepointers *) set, p));
}


MULLE_C_NONNULL_SECOND
static inline void   *mulle_uniquepointers_get( struct mulle_uniquepointers *set, void *p)
{
   return( set ? _mulle_uniquepointers_get( set, p) : NULL);
}

#endif
