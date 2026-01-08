//
//  Copyright (C) 2011-2020 Nat!, Mulle kybernetiK.
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
#ifndef mulle_pointerset_h__
#define mulle_pointerset_h__

#include "mulle--pointerset.h"

#pragma mark - operations built on generic and struct operations

struct mulle_pointerset
{
   MULLE__POINTERSET_BASE;
   struct mulle_allocator   *allocator;   // public...
};


#define MULLE_POINTERSET_DATA( xallocator) \
   ((struct mulle_pointerset)              \
   {                                       \
      .allocator = xallocator              \
   })

// MEMO: not possible because its a hashtable, duh!
/* #define MULLE_POINTERSET_FLEXIBLE_DATA( pointers, count, xallocator)
 *   ((struct mulle_pointerset)
 *   {
 *      ._storage         = pointers,
 *      ._initial_storage = pointers,
 *      ._size            = count,
 *      ._allocator       = .xallocator
 *   })
 */

static inline struct mulle_pointerset  *
   mulle_pointerset_alloc( struct mulle_allocator *allocator)
{
   struct mulle_pointerset   *set;

   set = mulle_allocator_malloc( allocator, sizeof( struct mulle_pointerset));
   return( set);
}


// if capacity is 0, this just does a memset 0
MULLE_C_NONNULL_FIRST
static inline void    _mulle_pointerset_init( struct mulle_pointerset *set,
                                              size_t capacity,
                                              struct mulle_allocator *allocator)
{
   _mulle__pointerset_init( (struct mulle__pointerset *) set, capacity, allocator);

   set->allocator = allocator;
}


static inline void    mulle_pointerset_init( struct mulle_pointerset *set,
                                             size_t capacity,
                                             struct mulle_allocator *allocator)
{
    if( set)
       _mulle_pointerset_init( set, capacity, allocator);
}


static inline void    
   mulle_pointerset_init_with_static_pointers( struct mulle_pointerset *set,
                                               void **storage,
                                               size_t capacity,
                                               struct mulle_allocator *allocator)
{
    if( set)
    {
       _mulle__pointerset_init_with_static_pointers( (struct mulle__pointerset *) set,
                                                     storage,
                                                     capacity);
      set->allocator = allocator;
    }
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


static inline struct mulle_pointerset *
   mulle_pointerset_create( struct mulle_allocator *allocator)
{
   struct mulle_pointerset   *set;

   set = mulle_pointerset_alloc( allocator);
   _mulle_pointerset_init( set, 0, allocator);
   return( set);
}


static inline void   mulle_pointerset_destroy( struct mulle_pointerset *set)
{
   if( set)
      _mulle__pointerset_destroy( (struct mulle__pointerset *) set,
                                 set->allocator);
}


static inline void   mulle__pointerset_reset( struct mulle_pointerset *set)
{
   if( set)
      _mulle__pointerset_reset( (struct mulle__pointerset *) set);
}

//
//
// count is the number of pointers in the set
static inline size_t   mulle_pointerset_get_count( struct mulle_pointerset *set)
{
   if( ! set)
      return( 0);
   return( _mulle__pointerset_get_count( (struct mulle__pointerset *) set));
}

static inline size_t   mulle_pointerset_get_size( struct mulle_pointerset *set)
{
   if( ! set)
      return( 0);
   return( _mulle__pointerset_get_size( (struct mulle__pointerset *) set));
}


static inline int   mulle_pointerset_is_full( struct mulle_pointerset *set)
{
   if( ! set)
      return( 1);
   return( _mulle__pointerset_is_full( (struct mulle__pointerset *) set));
}


static inline int   mulle_pointerset_is_sparse( struct mulle_pointerset *set)
{
   if( ! set)
      return( 0);
   return( _mulle__pointerset_is_sparse( (struct mulle__pointerset *) set));
}


//

static inline
void *    mulle_pointerset_get( struct mulle_pointerset *set, void *p)
{
   if( ! set)
      return( NULL);
   return( _mulle__pointerset_get( (struct mulle__pointerset *) set, p));
}


static inline
void    mulle_pointerset_set( struct mulle_pointerset *set, void *p)
{
   if( set)
      _mulle__pointerset_set( (struct mulle__pointerset *) set, p, set->allocator);
}


// returns 1 if insert succeeded
static inline int
   mulle_pointerset_insert( struct mulle_pointerset *set, void *p)
{
   if( ! set)
      return( 0);
   return( _mulle__pointerset_insert( (struct mulle__pointerset *) set, p, set->allocator));
}


static inline void *
   mulle_pointerset_register( struct mulle_pointerset *set, void *p)
{
   if( ! set)
      return( NULL);
   return( _mulle__pointerset_register( (struct mulle__pointerset *) set, p, set->allocator));
}



static inline
void   mulle_pointerset_shrink( struct mulle_pointerset *set)
{
   if( set)
      _mulle__pointerset_shrink( (struct mulle__pointerset *) set, set->allocator);
}


static inline void
   mulle_pointerset_shrink_if_needed( struct mulle_pointerset *set)
{
   if( set)
      _mulle__pointerset_shrink_if_needed( (struct mulle__pointerset *) set, set->allocator);
}


static inline
int  mulle_pointerset_remove( struct mulle_pointerset *set, void *p)
{
   if( ! set)
      return( 0);
   return( _mulle__pointerset_remove( (struct mulle__pointerset *) set, p, set->allocator));
}


static inline void
   mulle_pointerset_copy_items( struct mulle_pointerset *dst,
                                struct mulle_pointerset *src)
{
   if( dst)
      _mulle__pointerset_copy_items( (struct mulle__pointerset *) dst,
                                     (struct mulle__pointerset *) src,
                                     dst->allocator);
}


MULLE__CONTAINER_GLOBAL
struct mulle_pointerset  *mulle_pointerset_copy( struct mulle_pointerset *set);


// created by make-container-do.sh mulle--pointerset.c

#define mulle_pointerset_do( name)                          \
   for( struct mulle_pointerset                             \
           name ## __container = { 0 },                     \
           *name = &name ## __container,                    \
           *name ## __i = NULL;                             \
        ! name ## __i;                                      \
        name ## __i =                                       \
        (                                                   \
           mulle_pointerset_done( &name ## __container),    \
           (void *) 0x1                                     \
        )                                                   \
      )                                                     \
      for( int  name ## __j = 0;    /* break protection */  \
           name ## __j < 1;                                 \
           name ## __j++)


//
// API still incomplete, borrow from inferior data structure.
//     In this for loop, pretend that _mulle__pointersetenumerator_done is
//     very important, and needs to be called. Could be
//     good in a different data structure, where an enumerator must actually
//     allocate and free something.
//
#define mulle_pointerset_for( set, item)                                                                                     \
   assert( sizeof( item) == sizeof( void *));                                                                                \
   for( struct mulle__pointersetenumerator rover__ ## item = mulle__pointerset_enumerate( (struct mulle__pointerset *) set); \
        *rover__  ## item ## __i = (void *) 0;                                                                              \
        ! rover__  ## item ## __i;                                                                                          \
        rover__ ## item ## __i   = (_mulle__pointersetenumerator_done( &rover__ ## item), (void *) 1))                      \
                                                                                                                             \
      while(_mulle__pointersetenumerator_next( &rover__ ## item, (void **) &item))

#endif
