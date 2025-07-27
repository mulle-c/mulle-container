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
#ifndef mulle__rangemap_h__
#define mulle__rangemap_h__

#include "include.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

struct mulle_range;


//
// mulle__rangemap stores ranges with associated values
//
// 1. you can not add a range that overlaps with another range
// 2. you can remove a range, but only if the range **equals** a stored range
// 3. you can get a list of ranges (if non NULL) and a list of values
//    (if non NULL), for a given range.
//    Each range in the list intersects with the given range. The return value
//    is the number of ranges that intersect the given range.
// 4. you can lookup a range by its location (not by its range)
// 5. If you want to split or move ranges, its the API user responsibility
//    to split this into multiple insert and remove actions. Reason being
//    that its too complicated to properly handle "value" in these cases
//    struct mulle__rangemap has no API for that.
// 6. adjacent ranges are not merged, this different from mulle__rangeset
//
struct mulle__rangemap
{
   void        *_storage;    // the malloc'd area
   size_t      _length;
   size_t      _size;        // size of each half
#ifdef MULLE__CONTAINER_HAVE_MUTATION_COUNT
   uintptr_t   _n_mutations;
#endif
};



void   _mulle__rangemap_init( struct mulle__rangemap *map,
                              size_t capacity,
                              struct mulle_allocator *allocator);

void   _mulle__rangemap_done( struct mulle__rangemap *map,
                              struct mulle_allocator *allocator);



// TODO: AI liked it, i am not so sure :)
static inline struct mulle_range   *_mulle__rangemap_get_ranges( struct mulle__rangemap *map)
{
   return( (struct mulle_range *) map->_storage);
}


static inline void   **_mulle__rangemap_get_values( struct mulle__rangemap *map)
{
   return( (void **) &((struct mulle_range *) map->_storage)[ map->_size]);
}



MULLE_C_NONNULL_FIRST
static inline size_t  _mulle__rangemap_get_count( struct mulle__rangemap *map)
{
   return( map->_length);
}

MULLE_C_NONNULL_FIRST
static inline int  _mulle__rangemap_is_empty( struct mulle__rangemap *map)
{
   return( map->_length == 0);
}




MULLE_C_NONNULL_FIRST
static inline struct mulle_range   _mulle__rangemap_get_range( struct mulle__rangemap *map,
                                                               uintptr_t index)
{
   struct mulle_range  *ranges;

   if( index >= map->_length)
      return( mulle_range_make_invalid());

   ranges = _mulle__rangemap_get_ranges( map);
   return( ranges[ index]);
}


MULLE_C_NONNULL_FIRST
static inline void   *_mulle__rangemap_get_value( struct mulle__rangemap *map,
                                                  uintptr_t index)
{
   void  **values;

   if( index >= map->_length)
      return( NULL);

   values = _mulle__rangemap_get_values( map);
   return( values[ index]);
}

//
// returns 0 on success
MULLE_C_NONNULL_FIRST
int   _mulle__rangemap_insert( struct mulle__rangemap *map,
                               struct mulle_range range,
                               void *value,
                               struct mulle_allocator *allocator);

// the range must match exactly
// returns 0 on success
//
MULLE_C_NONNULL_FIRST
int    _mulle__rangemap_remove( struct mulle__rangemap *map,
                                struct mulle_range range,
                                struct mulle_allocator *allocator);


// returns index to be used with _mulle__rangemap_get_range
// or _mulle__rangemap_get_value

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
uintptr_t  _mulle__rangemap_search( struct mulle__rangemap *map,
                                    uintptr_t location);



MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__rangemap_reset( struct mulle__rangemap *map,
                               struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__rangemap_print( struct mulle__rangemap *map,
                               FILE *fp);




struct mulle__rangemapenumerator
{
   struct mulle_range   *_curr;
   struct mulle_range   *_sentinel;
   void                 **_values;
};


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


#endif
