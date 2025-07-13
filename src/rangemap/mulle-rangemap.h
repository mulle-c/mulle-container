//
//  mulle-rangemap.h
//  mulle-container
//
//  Created by Nat! on 10.07.25.
//  Copyright (C) 2023 Nat!, Mulle kybernetiK.
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
#ifndef mulle_rangemap_h__
#define mulle_rangemap_h__

#include "mulle-container-callback.h"
#include "include.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

//
// mulle__rangemap stores ranges with associated values
// When ranges are merged, the leftmost (first) value is kept
//
struct mulle__rangemap
{
   void                *_storage;    // the malloc'd area
   size_t              _length;
   size_t              _size;        // size of each half
#ifdef MULLE__CONTAINER_HAVE_MUTATION_COUNT
   uintptr_t          _n_mutations;
#endif
};

struct mulle__rangemapenumerator
{
   struct mulle_range   *_curr;
   struct mulle_range   *_sentinel;
   void                **_values;
};


static inline struct mulle_range *_mulle__rangemap_get_ranges( struct mulle__rangemap *map)
{
   return( (struct mulle_range *) map->_storage);
}

static inline void **_mulle__rangemap_get_values( struct mulle__rangemap *map)
{
   return( (void **) &((struct mulle_range *) map->_storage)[ map->_size]);
}

void   _mulle__rangemap_init( struct mulle__rangemap *map,
                             size_t capacity,
                             struct mulle_allocator *allocator);

void   _mulle__rangemap_done( struct mulle__rangemap *map,
                             struct mulle_allocator *allocator);

void   __mulle__rangemap_insert( struct mulle__rangemap *map,
                                struct mulle_range range,
                                void *value,
                                void **merged,
                                struct mulle_allocator *allocator);

void   __mulle__rangemap_remove( struct mulle__rangemap *map,
                                struct mulle_range range,
                                void **removed_values,
                                size_t *n_removed,
                                struct mulle_allocator *allocator);

struct mulle_range   _mulle__rangemap_search_exact( struct mulle__rangemap *map,
                                                   uintptr_t location,
                                                   void **value);

struct mulle_range   _mulle__rangemap_search_nearest( struct mulle__rangemap *map,
                                                     uintptr_t location,
                                                     void **value);

static inline size_t  _mulle__rangemap_get_count( struct mulle__rangemap *map)
{
   return( map->_length);
}

static inline int  _mulle__rangemap_is_empty( struct mulle__rangemap *map)
{
   return( map->_length == 0);
}

void   _mulle__rangemap_reset( struct mulle__rangemap *map,
                              struct mulle_allocator *allocator);

void   _mulle__rangemap_print( struct mulle__rangemap *map,
                              FILE *fp);


// created by make-container-do.sh --no-callback --type struct mulle_range --flexible mulle-rangemap
#define mulle__rangemap_do( name)                              \
   for( struct mulle__rangemap                                 \
           name ## __container = { 0 },                        \
           *name = &name ## __container,                       \
           *name ## __i = NULL;                                \
        ! name ## __i;                                         \
        name ## __i =                                          \
        (                                                      \
           _mulle__rangemap_done( &name ## __container, NULL), \
           (void *) 0x1                                        \
        )                                                      \
      )                                                        \
      for( int  name ## __j = 0;    /* break protection */     \
           name ## __j < 1;                                    \
           name ## __j++)

// Enumeration
static inline struct mulle__rangemapenumerator
   _mulle__rangemap_enumerate( struct mulle__rangemap *map)
{
   struct mulle__rangemapenumerator   rover;

   rover._curr = _mulle__rangemap_get_ranges( map);
   rover._sentinel = rover._curr + map->_length;
   rover._values = _mulle__rangemap_get_values( map);
   return( rover);
}

static inline int
   _mulle__rangemapenumerator_next( struct mulle__rangemapenumerator *rover,
                                   struct mulle_range *range,
                                   void **value)
{
   if( rover->_curr < rover->_sentinel)
   {
      *range = *rover->_curr++;
      *value = *rover->_values++;
      return( 1);
   }

   *range = mulle_range_make_invalid();
   *value = NULL;
   return( 0);
}

//
// Public API layer - follows the pattern of mulle_set
//

// Public rangemap structure with embedded allocator
struct mulle_rangemap
{
   struct mulle__rangemap   _base;
   struct mulle_allocator   *allocator;
};

#define MULLE_RANGEMAP_DATA( xallocator) \
   ((struct mulle_rangemap)              \
   {                                     \
      .allocator = (xallocator)          \
   })

MULLE__CONTAINER_GLOBAL
struct mulle_rangemap   *mulle_rangemap_create( size_t capacity,
                                                struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
void   mulle_rangemap_init( struct mulle_rangemap *map,
                           size_t capacity,
                           struct mulle_allocator *allocator);

static inline void   _mulle_rangemap_done( struct mulle_rangemap *map)
{
   _mulle__rangemap_done( &map->_base, map->allocator);
}

static inline void   mulle_rangemap_done( struct mulle_rangemap *map)
{
   if( map)
      _mulle__rangemap_done( &map->_base, map->allocator);
}

static inline void   mulle_rangemap_destroy( struct mulle_rangemap *map)
{
   if( map)
   {
      _mulle__rangemap_done( &map->_base, map->allocator);
      mulle_allocator_free( map->allocator, map);
   }
}

static inline void   mulle_rangemap_reset( struct mulle_rangemap *map)
{
   if( map)
      _mulle__rangemap_reset( &map->_base, map->allocator);
}

// Accessors
static inline struct mulle_allocator   *mulle_rangemap_get_allocator( struct mulle_rangemap *map)
{
   return( map ? map->allocator : NULL);
}

static inline size_t   mulle_rangemap_get_count( struct mulle_rangemap *map)
{
   return( map ? _mulle__rangemap_get_count( &map->_base) : 0);
}

static inline int   mulle_rangemap_is_empty( struct mulle_rangemap *map)
{
   return( map ? _mulle__rangemap_is_empty( &map->_base) : 1);
}

// Operations
static inline void   _mulle_rangemap_insert( struct mulle_rangemap *map,
                                             struct mulle_range range,
                                             void *value,
                                             void **merged)
{
   __mulle__rangemap_insert( &map->_base, range, value, merged, map->allocator);
}

static inline void   mulle_rangemap_insert( struct mulle_rangemap *map,
                                            struct mulle_range range,
                                            void *value,
                                            void **merged)
{
   if( map)
      _mulle_rangemap_insert( map, range, value, merged);
}

static inline void   _mulle_rangemap_remove( struct mulle_rangemap *map,
                                             struct mulle_range range,
                                             void **removed_values,
                                             size_t *n_removed)
{
   __mulle__rangemap_remove( &map->_base, range, removed_values, n_removed, map->allocator);
}

static inline void   mulle_rangemap_remove( struct mulle_rangemap *map,
                                            struct mulle_range range,
                                            void **removed_values,
                                            size_t *n_removed)
{
   if( map)
      _mulle_rangemap_remove( map, range, removed_values, n_removed);
}

static inline struct mulle_range   mulle_rangemap_search_exact( struct mulle_rangemap *map,
                                                                uintptr_t location,
                                                                void **value)
{
   return( map ? _mulle__rangemap_search_exact( &map->_base, location, value) : mulle_range_make_invalid());
}

static inline struct mulle_range   mulle_rangemap_search_nearest( struct mulle_rangemap *map,
                                                                  uintptr_t location,
                                                                  void **value)
{
   return( map ? _mulle__rangemap_search_nearest( &map->_base, location, value) : mulle_range_make_invalid());
}

static inline void   mulle_rangemap_print( struct mulle_rangemap *map, FILE *fp)
{
   if( map)
      _mulle__rangemap_print( &map->_base, fp);
}

// Do macro for public API
#define mulle_rangemap_do( name)                                \
   for( struct mulle_rangemap                                   \
           name ## __container = MULLE_RANGEMAP_DATA( NULL),    \
           *name = &name ## __container,                        \
           *name ## __i = NULL;                                 \
        ! name ## __i;                                          \
        name ## __i =                                           \
        (                                                       \
           mulle_rangemap_init( &name ## __container, 0, NULL), \
           _mulle_rangemap_done( &name ## __container),         \
           (void *) 0x1                                         \
        )                                                       \
      )                                                         \
      for( int  name ## __j = 0;    /* break protection */      \
           name ## __j < 1;                                     \
           name ## __j++)

#endif
