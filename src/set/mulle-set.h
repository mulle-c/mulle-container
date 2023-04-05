//
//  mulle-set.h
//  mulle-container
//
//  Created by Nat! on 17.04.16.
//  Copyright © 2016 Mulle kybernetiK. All rights reserved.
//

//
//  Copyright (C) 2011 Nat!, Mulle kybernetiK.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//  Redistributions of source code must retain the above copyright notice, this
//  list of conditions and the following disclaimer.
//
//  Redistributions in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.
//
//  Neither the name of Mulle kybernetiK nor the names of its contributors
//  may be used to endorse or promote products derived from this software
//  without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
#ifndef mulle_set__h__
#define mulle_set__h__

#include "mulle--set.h"

#include "include.h"

#include <string.h>

//
// a mulle_set is a mulle_hashtable, with callbacks added
// this makes a mulle_set more convenient.
//

// callback and allocator can be public
#define MULLE_SET_BASE                             \
   MULLE__SET_BASE;                                \
   struct mulle_container_keycallback   *callback; \
   struct mulle_allocator               *allocator


struct mulle_set
{
   MULLE_SET_BASE;
};

MULLE_CONTAINER_GLOBAL
struct mulle_set   *mulle_set_create( unsigned int capacity,
                                      struct mulle_container_keycallback *callback,
                                      struct mulle_allocator *allocator);

MULLE_CONTAINER_GLOBAL
void   mulle_set_init( struct mulle_set *set,
                       unsigned int capacity,
                       struct mulle_container_keycallback *callback,
                       struct mulle_allocator *allocator);


static inline void   mulle_set_done( struct mulle_set *set)
{
   if( set)
      _mulle__set_done( (struct mulle__set *) set, set->callback, set->allocator);
}


static inline void   mulle_set_destroy( struct mulle_set *set)
{
   if( set)
      _mulle__set_destroy( (struct mulle__set *) set, set->callback, set->allocator);
}


static inline void   mulle_set_reset( struct mulle_set *set)
{
   if( set)
      _mulle__set_reset( (struct mulle__set *) set, set->callback, set->allocator);
}

#pragma mark - petty accessors

static inline struct mulle_allocator   *mulle_set_get_allocator( struct mulle_set *set)
{
   return( set ? set->allocator : NULL);
}


static inline struct mulle_container_keycallback   *
   mulle_set_get_keycallback( struct mulle_set *set)
{
   return( set ? set->callback : NULL);
}


static inline unsigned int   mulle_set_get_count( struct mulle_set *set)
{
   return( set ? _mulle__set_get_count( (struct mulle__set *) set) : 0);
}


static inline unsigned int   mulle_set_get_size( struct mulle__set *set)
{
   return( set ? _mulle__set_get_size( (struct mulle__set *) set) : 0);
}



#pragma mark - operations

static inline void   *mulle_set_get( struct mulle_set *set, void *p)
{
   return( set ? _mulle__set_get( (struct mulle__set *) set, p, set->callback) : NULL);
}


MULLE_C_NONNULL_FIRST
static inline int
   _mulle_set_member( struct mulle_set *set, void *p)
{
   return( _mulle__set_member( (struct mulle__set *) set, p, set->callback));
}


static inline int
   mulle_set_member( struct mulle_set *set, void *p)
{
   return( mulle__set_member( (struct mulle__set *) set, p, set->callback));
}


static inline void   mulle_set_remove( struct mulle_set *set, void *p)
{
   if( set)
      _mulle__set_remove( (struct mulle__set *) set, p, set->callback, set->allocator);
}



static inline void   *mulle_set_insert( struct mulle_set *set, void *p)
{
   if( ! set)
      return( NULL);
   return( _mulle__set_insert( (struct mulle__set *) set, p, set->callback, set->allocator));
}


static inline void   _mulle_set_set( struct mulle_set *set, void *p)
{
   _mulle__set_set( (struct mulle__set *) set, p, set->callback, set->allocator);
}


static inline void   mulle_set_set( struct mulle_set *set, void *p)
{
   if( set)
      _mulle__set_set( (struct mulle__set *) set, p, set->callback, set->allocator);
}


#pragma mark - copy


static inline int   mulle_set_copy_items( struct mulle_set *dst, struct mulle_set *src)
{
   if( dst)
      return( _mulle__set_copy_items( (struct mulle__set *) dst,
                                     (struct mulle__set *) src,
                                      dst->callback,
                                      dst->allocator));
   return( -1);
}


static inline struct mulle_set   *mulle_set_copy( struct mulle_set *set)
{
   struct mulle_set   *other;

   if( ! set)
      return( set);

   // can't allow creation to be done by struct mulle__set
   other = mulle_set_create( mulle_set_get_count( set), set->callback, set->allocator);
   _mulle__set_copy_items( (struct mulle__set *) other,
                           (struct mulle__set *) set,
                           set->callback,
                           set->allocator);
   return( other);
}


void   mulle_set_add_set( struct mulle_set *set, struct mulle_set *other);



#pragma mark - debugging

// in C, expect a strdup()ed string, in ObjC an autorelease NSString *
static inline void   *mulle_set_describe( struct mulle_set *set,
                                          struct mulle_container_keycallback *callback,
                                          struct mulle_allocator *allocator)
{
   if( ! set)
      return( NULL);
   return( _mulle__set_describe( (struct mulle__set *) set, set->callback, set->allocator));
}


#pragma mark - enumerator

/*
 * struct mulle_setenumerator    rover;
 * void                          *key;
 *
 * rover = mulle_set_enumerate( set);
 * while( mulle_setenumerator_next( &rover, &key))
 *    printf( "%s\n", key);
 * mulle_setenumerator_done( &rover);
 *
 */
#define MULLE_SETENUMERATOR_BASE   \
   MULLE__SETENUMERATOR_BASE


struct  mulle_setenumerator
{
   MULLE_SETENUMERATOR_BASE;
};


#define mulle_setenumerator_empty  \
   ((struct mulle_setenumerator) { 0 })


MULLE_C_NONNULL_FIRST
static inline struct mulle_setenumerator
   _mulle_set_enumerate( struct mulle_set *set)
{
   struct mulle_setenumerator    rover;
   struct mulle__setenumerator   tmp;

   tmp = _mulle__set_enumerate( (struct mulle__set *) set, set->callback);
   memcpy( &rover, &tmp, sizeof( struct mulle__setenumerator));
   return( rover);
}


static inline struct mulle_setenumerator
   mulle_set_enumerate( struct mulle_set *set)
{
   if( ! set)
      return( mulle_setenumerator_empty);
   return( _mulle_set_enumerate( set));
}


MULLE_C_NONNULL_FIRST_SECOND
static inline int
   _mulle_setenumerator_next( struct mulle_setenumerator *rover, void **item)
{
   return( _mulle__setenumerator_next( (struct mulle__setenumerator *) rover, item));
}


static inline int
   mulle_setenumerator_next( struct mulle_setenumerator *rover, void **item)
{
   return( mulle__setenumerator_next( (struct mulle__setenumerator *) rover, item));
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle_setenumerator_done( struct mulle_setenumerator *rover)
{
}


static inline void   mulle_setenumerator_done( struct mulle_setenumerator *rover)
{
}


#define mulle_set_for( set, item)                                               \
   for( struct mulle_setenumerator rover__ ## item = mulle_set_enumerate( set); \
        _mulle_setenumerator_next( &rover__ ## item, (void **) &item);)

#endif

