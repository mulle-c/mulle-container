//
//  mulle-bigmap.h
//  mulle-container
//
//  Created by Nat! on 03/11/15.
//  Copyright (c) 2015 Nat! - Mulle kybernetiK.
//  Copyright (c) 2015 Codeon GmbH.
//  All rights reserved.
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
#ifndef mulle_bigmap__h__
#define mulle_bigmap__h__

#include "_mulle-bigmap.h"

//
// just like struct _mulle_bigmap but has the callback embedded in
// its structure
//
#define MULLE_BIGMAP_BASE                                 \
   _MULLE_BIGMAP_BASE;                                    \
   struct mulle_container_keyvaluecallback    *_callback; \
   struct mulle_allocator                     *_allocator


struct mulle_bigmap
{
   MULLE_BIGMAP_BASE;
};

#define MULLE_BIGMAPENUMERATOR_BASE    _MULLE_BIGMAPENUMERATOR_BASE


struct mulle_bigmapenumerator
{
   MULLE_BIGMAPENUMERATOR_BASE;
};


struct mulle_bigmap   *mulle_bigmap_create( size_t capacity,
                                           struct mulle_container_keyvaluecallback *callback,
                                           struct mulle_allocator *allocator)
mulle_nonnull_second;

void   mulle_bigmap_init( struct mulle_bigmap *map,
                         size_t capacity,
                         struct mulle_container_keyvaluecallback *callback,
                         struct mulle_allocator *allocator)
mulle_nonnull_first_third;


static inline void    mulle_bigmap_done( struct mulle_bigmap *map)
{
   if( map)
      _mulle_bigmap_done( (struct _mulle_bigmap *) map, map->_callback, map->_allocator);
}


static inline void   mulle_bigmap_destroy( struct mulle_bigmap *map)
{
   if( map)
      _mulle_bigmap_destroy( (struct _mulle_bigmap *) map, map->_callback, map->_allocator);
}


#pragma mark -
#pragma mark operations


static inline struct mulle_allocator   *mulle_bigmap_get_allocator( struct mulle_bigmap *map)
{
   return( map->_allocator);
}


static inline void   *mulle_bigmap_get( struct mulle_bigmap *map, void *key)
{
   return( _mulle_bigmap_get( (struct _mulle_bigmap *) map, key, map->_callback));
}


static inline void   mulle_bigmap_remove( struct mulle_bigmap *map, void *key)
{
   _mulle_bigmap_remove( (struct _mulle_bigmap *) map, key, map->_callback, map->_allocator);
}


static inline size_t   mulle_bigmap_get_count( struct mulle_bigmap *map)
{
   return( _mulle_bigmap_get_count( (struct _mulle_bigmap *) map));
}


static inline void   mulle_bigmap_insert_values_for_keys_v( struct mulle_bigmap *map, void *firstvalue, void *firstkey, va_list args)
{
   _mulle_bigmap_insert_values_for_keysv( (struct _mulle_bigmap *) map, firstvalue, firstkey, args, map->_callback, map->_allocator);
}


static inline void   mulle_bigmap_insert_values_for_keys( struct mulle_bigmap *map, void *firstvalue, void *firstkey, ...)
{
   va_list   args;

   va_start( args, firstkey);
   _mulle_bigmap_insert_values_for_keysv( (struct _mulle_bigmap *) map, firstvalue, firstkey, args, map->_callback, map->_allocator);
   va_end( args);
}


static inline void   mulle_bigmap_set( struct mulle_bigmap *map, void *key, void *value)
{
   _mulle_bigmap_set( (struct _mulle_bigmap *) map, key, value, map->_callback, map->_allocator);
}


static inline void   *mulle_bigmap_insert( struct mulle_bigmap *map, void *key, void *value)
{
   return( _mulle_bigmap_insert( (struct _mulle_bigmap *) map, key, value, map->_callback, map->_allocator));
}


static inline struct mulle_bigmap   *mulle_bigmap_copy( struct mulle_bigmap *map)
{
   struct mulle_bigmap   *other;
   extern void   _mulle_bigmap_copy_items( struct _mulle_bigmap *, struct _mulle_bigmap *, struct mulle_container_keyvaluecallback *, struct mulle_allocator *);

   // can't allow creation to be done by struct _mulle_bigmap
   other = mulle_bigmap_create( mulle_bigmap_get_count( map), map->_callback, map->_allocator);
   _mulle_bigmap_copy_items( (struct _mulle_bigmap *) other, (struct _mulle_bigmap *) map, map->_callback, map->_allocator);
   return( other);
}



static inline struct mulle_bigmapenumerator   mulle_bigmap_enumerate( struct mulle_bigmap *map)
{
   struct _mulle_bigmapenumerator   rover;

   rover = _mulle_bigmap_enumerate( (struct _mulle_bigmap *) map, map->_callback);
   return( *(struct mulle_bigmapenumerator *) &rover);
}


static inline int    mulle_bigmapenumerator_next( struct mulle_bigmapenumerator *rover, void **key, void **value)
{
   return( _mulle_bigmapenumerator_next( (struct _mulle_bigmapenumerator *) rover, key, value));
}


static inline void    mulle_bigmapenumerator_done( struct mulle_bigmapenumerator *rover)
{
   _mulle_bigmapenumerator_done( (struct _mulle_bigmapenumerator *) rover);
}


static inline void    mulle_bigmap_reset( struct mulle_bigmap *map)
{
   mulle_bigmap_done( map);
   mulle_bigmap_init( map, 0, map->_callback, map->_allocator);
}


#endif
