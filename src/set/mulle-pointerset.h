//
//   mulle-pointerset.h
//  mulle-container
//
//  Created by Nat! on 03.04.16.
//  Copyright © 2016 Mulle kybernetiK. All rights reserved.
//

#ifndef mulle_pointerset_h__
#define mulle_pointerset_h__

#include "mulle--pointerset.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>


//
// A really simple kind of set, that just stores pointers with pointer
// equality and uses bsearch for lookup. Useful for small sets ( < 100 entries)
// Apart from the embedded allocator, mulle_pointerset adds no features
// to struct mulle__pointerset
//
struct mulle_pointerset
{
   MULLE__POINTERSET_BASE;
   struct mulle_allocator   *allocator;
};


MULLE_C_NONNULL_FIRST
static inline void    _mulle_pointerset_init( struct mulle_pointerset *set,
                                              struct mulle_allocator *allocator)
{
   _mulle__pointerset_init( (struct mulle__pointerset *) set);
   set->allocator = allocator;
}

static inline void    mulle_pointerset_init( struct mulle_pointerset *set,
                                             struct mulle_allocator *allocator)
{
   if( set)
      _mulle_pointerset_init( set, allocator);
}


MULLE_C_NONNULL_FIRST
static inline void    _mulle_pointerset_done( struct mulle_pointerset *set)
{
   _mulle__pointerset_done( (struct mulle__pointerset *) set, set->allocator);
}


static inline void    mulle_pointerset_done( struct mulle_pointerset *set)
{
   if( set)
      _mulle_pointerset_done( set);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline void    _mulle_pointerset_insert( struct mulle_pointerset *set, void *p)
{
   _mulle__pointerset_insert( (struct mulle__pointerset *) set, p, set->allocator);
}


static inline void    mulle_pointerset_insert( struct mulle_pointerset *set, void *p)
{
   if( set && p)
      _mulle_pointerset_insert( set, p);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline void   *_mulle_pointerset_get( struct mulle_pointerset *set, void *p)
{
   return( _mulle__pointerset_get( (struct mulle__pointerset *) set, p));
}


MULLE_C_NONNULL_SECOND
static inline void   *mulle_pointerset_get( struct mulle_pointerset *set, void *p)
{
   return( set ? _mulle_pointerset_get( set, p) : NULL);
}


#endif
