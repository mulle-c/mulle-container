//
//  mulle-assoc.h
//  mulle-container
//
//  Created by Nat! on 04/11/15.
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

#ifndef mulle_assoc_h__
#define mulle_assoc_h__

#include "mulle--assoc.h"
#include "mulle-container-operation.h"

#include <assert.h>

//
// mulle-assoc is a autosorting. You add a compare operation and then
// your associative array is always sorted. You can not add duplicate values
// though. The API tries to be very similiar to mulle_map so you can
// swap between both implementations more easily. The advantage of mulle-assoc
// over mulle_map is, that it does not need "holes", therefore will use less
// memory in most cases (except when very, very few items are present)
//
// The userinfo for the compare will be the assoc itself.
//
#define MULLE_ASSOC_BASE                                  \
   MULLE__ASSOC_BASE;                                     \
   struct mulle_container_keyvaluecallback   *callback;   \
   mulle_pointerpair_compare_t               *compare;    \
   struct mulle_allocator                    *allocator;  \
   int                                       _is_sorted


struct mulle_assoc
{
   MULLE_ASSOC_BASE;
};


#define MULLE_ASSOC_INIT( xcallback, xcompare, xallocator) \
   ((struct mulle_assoc)                                   \
   {                                                       \
      .callback         = (xcallback),                     \
      .allocator        = (xallocator),                    \
      .compare          = (xcompare),                      \
   })

MULLE_C_NONNULL_THIRD
static inline void    mulle_assoc_init( struct mulle_assoc *assoc,
                                        unsigned int length,
                                        struct mulle_container_keyvaluecallback *callback,
                                        mulle_pointerpair_compare_t *compare,
                                        struct mulle_allocator *allocator)
{
   if( ! assoc)
      return;

   _mulle__assoc_init( (struct mulle__assoc *) assoc, length, allocator);

   assoc->callback   = callback;
   assoc->allocator  = allocator;
   assoc->compare    = compare ? compare : _mulle_pointerpair_compare_pointer_key;
   assoc->_is_sorted = 0;
}

static inline void   _mulle_assoc_done( struct mulle_assoc *assoc)
{
   _mulle__assoc_done( (struct mulle__assoc *) assoc,
                       assoc->callback,
                       assoc->allocator);
}


static inline void   mulle_assoc_done( struct mulle_assoc *assoc)
{
   if( ! assoc)
      return;
   _mulle__assoc_done( (struct mulle__assoc *) assoc,
                       assoc->callback,
                       assoc->allocator);
}


MULLE__CONTAINER_GLOBAL
struct mulle_assoc    *
   mulle_assoc_create( unsigned int capacity,
                       struct mulle_container_keyvaluecallback *callback,
                       mulle_pointerpair_compare_t *compare,
                       struct mulle_allocator *allocator);


static inline void   mulle_assoc_destroy( struct mulle_assoc *assoc)
{
   if( ! assoc)
      return;

   _mulle__assoc_destroy( (struct mulle__assoc *) assoc,
                          assoc->callback,
                          assoc->allocator);
}


# pragma mark - petty accessors

MULLE_C_NONNULL_FIRST
static inline unsigned int
   _mulle_assoc_get_size( struct mulle_assoc *assoc)
{
   return( _mulle__assoc_get_size( (struct mulle__assoc *) assoc));
}


static inline unsigned int
   mulle_assoc_get_size( struct mulle_assoc *assoc)
{
   return( mulle__assoc_get_size( (struct mulle__assoc *) assoc));
}


MULLE_C_NONNULL_FIRST
static inline unsigned int
	_mulle_assoc_get_guaranteed_size( struct mulle_assoc *assoc)
{
   return( _mulle__assoc_get_guaranteed_size( (struct mulle__assoc *) assoc));
}


static inline unsigned int
	mulle_assoc_get_guaranteed_size( struct mulle_assoc *assoc)
{
   return( mulle__assoc_get_guaranteed_size( (struct mulle__assoc *) assoc));
}


MULLE_C_NONNULL_FIRST
static inline unsigned int
   _mulle_assoc_get_count( struct mulle_assoc *assoc)
{
   return( _mulle__assoc_get_count( (struct mulle__assoc *) assoc));
}


static inline unsigned int
   mulle_assoc_get_count( struct mulle_assoc *assoc)
{
   return( mulle__assoc_get_count( (struct mulle__assoc *) assoc));
}


MULLE_C_NONNULL_FIRST
static inline int
   _mulle_assoc_is_full( struct mulle_assoc *assoc)
{
   return( _mulle__assoc_is_full( (struct mulle__assoc *) assoc));
}


static inline int
   mulle_assoc_is_full( struct mulle_assoc *assoc)
{
   return( mulle__assoc_is_full( (struct mulle__assoc *) assoc));
}


MULLE_C_NONNULL_FIRST
static inline int
   _mulle_assoc_is_sorted( struct mulle_assoc *assoc)
{
   return( assoc->_is_sorted || _mulle_assoc_get_count( assoc) < 2);
}


static inline int
   mulle_assoc_is_sorted( struct mulle_assoc *assoc)
{
   return( assoc ? _mulle_assoc_is_sorted( assoc) : 1); // NULL is always sorted
}


static inline void
   _mulle_assoc_set_unsorted( struct mulle_assoc *assoc)
{
   assoc->_is_sorted = 0;
}



MULLE_C_NONNULL_FIRST
static inline struct mulle_container_keyvaluecallback  *
   _mulle_assoc_get_callback( struct mulle_assoc *assoc)
{
   return( assoc->callback);
}


static inline struct mulle_container_keyvaluecallback *
   mulle_assoc_get_callback( struct mulle_assoc *assoc)
{
   return( assoc ? assoc->callback : NULL);
}


MULLE_C_NONNULL_FIRST
static inline mulle_pointerpair_compare_t *
   _mulle_assoc_get_compare( struct mulle_assoc *assoc)
{
   return( assoc->compare);
}


static inline mulle_pointerpair_compare_t *
   mulle_assoc_get_compare( struct mulle_assoc *assoc)
{
   return( assoc ? assoc->compare : 0);
}


MULLE_C_NONNULL_FIRST
static inline struct mulle_allocator  *
   _mulle_assoc_get_allocator( struct mulle_assoc *assoc)
{
   return( assoc->allocator);
}


static inline struct mulle_allocator *
   mulle_assoc_get_allocator( struct mulle_assoc *assoc)
{
   return( assoc ? assoc->allocator : NULL);
}



MULLE_C_NONNULL_FIRST
static inline void
   _mulle_assoc_set_compare( struct mulle_assoc *assoc,
                             mulle_pointerpair_compare_t *compare)
{

   assoc->compare    = compare ? compare : _mulle_pointerpair_compare_pointer_key;
   assoc->_is_sorted = 0;
}


static inline void
   mulle_assoc_set_compare( struct mulle_assoc *assoc,
                             mulle_pointerpair_compare_t *compare)
{
   if( assoc)
      _mulle_assoc_set_compare( assoc, compare);
}



MULLE_C_NONNULL_FIRST
static inline void   *_mulle_assoc_get_notakey( struct mulle_assoc *assoc)
{
   return( _mulle_container_keycallback_get_notakey( &assoc->callback->keycallback));
}


static inline void   *mulle_assoc_get_notakey( struct mulle_assoc *assoc)
{
   return( assoc ? _mulle_assoc_get_notakey( assoc) : NULL);
}

#pragma mark - sort and search


//
// you can pass NULL for callback, and it will just compare pointer equality
// TODO: make this uniform across library for find ? also for search ?
//
static inline uintptr_t
   mulle_assoc_find_in_range( struct mulle_assoc *assoc,
                              void *key,
                              struct mulle_range range)
{
   return( mulle__assoc_find_in_range( (struct mulle__assoc *) assoc,
                                       key,
                                       range,
                                       assoc->callback));
}

static inline uintptr_t
   mulle_assoc_find( struct mulle_assoc *assoc, void *key)
{
   return( mulle__assoc_find( (struct mulle__assoc *) assoc,
                              key,
                              assoc->callback));
}


static inline void   mulle_assoc_qsort( struct mulle_assoc *assoc)
{
   mulle__assoc_qsort_r( (struct mulle__assoc *) assoc,
                         assoc->compare,
                         assoc);
   assoc->_is_sorted = 1;
}

static inline void   mulle_assoc_qsort_if_needed( struct mulle_assoc *assoc)
{
   if( ! mulle_assoc_is_sorted( assoc))
      mulle_assoc_qsort( assoc);
}


MULLE__CONTAINER_GLOBAL
void   *mulle_assoc_get( struct mulle_assoc *assoc, void *key);


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle_assoc_set( struct mulle_assoc *assoc, void *key, void *value);

static inline void
   mulle_assoc_set( struct mulle_assoc *assoc, void *key, void *value)
{
   if( assoc)
      _mulle_assoc_set( assoc, key, value);
}


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle_assoc_remap_intptr_key_range( struct mulle_assoc *assoc,
                                            struct mulle_range range,
                                            intptr_t offset);

static inline void
   mulle_assoc_remap_intptr_key_range( struct mulle_assoc *assoc,
                                       struct mulle_range range,
                                       intptr_t offset)
{
   if( ! assoc)
      return;

   _mulle_assoc_remap_intptr_key_range( assoc, range, offset);
}


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle_assoc_move_intptr_key_range( struct mulle_assoc *assoc,
                                           struct mulle_range range,
                                           intptr_t index);

static inline void
   mulle_assoc_move_intptr_key_range( struct mulle_assoc *assoc,
                                      struct mulle_range range,
                                      intptr_t offset)
{
   if( ! assoc)
      return;

   _mulle_assoc_move_intptr_key_range( assoc, range, offset);
}


static inline void
   mulle_assoc_assert_no_intptr_key_dupes( struct mulle_assoc *assoc)
{
   mulle__pointerpairarray_assert_no_dupes( (struct mulle__pointerpairarray *) assoc);
}


// add assumes that pair is not already present
// in debug, mode it will assert this
// add assumes that pair is not already present
// in debug, mode it will assert this
MULLE_C_NONNULL_FIRST
static inline void   _mulle_assoc_add( struct mulle_assoc *assoc, void *key, void *value)
{
   assert( mulle_not_found_e ==
               _mulle__assoc_find_callback( (struct mulle__assoc *) assoc, key, assoc->callback));

   assoc->_is_sorted = 0;

   _mulle__assoc_add( (struct mulle__assoc *) assoc,
                      key,
                      value,
                      assoc->callback,
                      assoc->allocator);
}


static inline void
   mulle_assoc_add( struct mulle_assoc *assoc, void *key, void *value)
{
   if( assoc)
      _mulle_assoc_add( assoc, key, value);
}


# pragma mark - operations

static inline void   mulle_assoc_remove_in_range( struct mulle_assoc *assoc,
                                                  struct mulle_range range)
{
   if( assoc)
   {
      mulle_assoc_qsort_if_needed( assoc);
      _mulle__assoc_remove_in_range( (struct mulle__assoc *) assoc,
                                     range,
                                     assoc->callback,
                                     assoc->allocator);
   }
}


// removal can change sort order
static inline void   _mulle_assoc_remove( struct mulle_assoc *assoc,
                                          void *key)
{
   _mulle__assoc_remove( (struct mulle__assoc *) assoc,
                         key,
                         assoc->callback,
                         assoc->allocator);
}


static inline void   mulle_assoc_remove( struct mulle_assoc *assoc,
                                         void *key)
{
   if( assoc)
   {
      _mulle__assoc_remove( (struct mulle__assoc *) assoc,
                            key,
                            assoc->callback,
                            assoc->allocator);
   }
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle_assoc_reset( struct mulle_assoc *assoc)
{
   _mulle__assoc_reset( (struct mulle__assoc *) assoc,
                        assoc->callback,
                        assoc->allocator);
   assoc->_is_sorted = 0;
}


static inline void   mulle_assoc_reset( struct mulle_assoc *assoc)
{
   if( assoc)
      _mulle_assoc_reset( assoc);
}


// will use callbacks of assoc to determine equality
static inline int   mulle_assoc_is_equal( struct mulle_assoc *assoc,
                                          struct mulle_assoc *other)
{
   if( ! assoc || ! other)
      return( assoc == other);

   assert( assoc->callback->keycallback.is_equal == other->callback->keycallback.is_equal);
   return( _mulle__assoc_is_equal( (struct mulle__assoc *) assoc,
                                   (struct mulle__assoc *) other,
                                   assoc->callback));
}


static inline
struct mulle_pointerpair  mulle_assoc_get_at_index( struct mulle_assoc *assoc,
                                                    unsigned int index)
{
   if( ! assoc)
      return( mulle_pointerpair_make_invalid());

   mulle_assoc_qsort_if_needed( assoc);
   return( _mulle__assoc_get_at_index( (struct mulle__assoc *) assoc, index));
}


MULLE_C_NONNULL_FIRST_THIRD
static inline unsigned int
   _mulle_assoc_get_in_range( struct mulle_assoc *assoc,
                              struct mulle_range range,
                              struct mulle_pointerpair *buf)
{
   mulle_assoc_qsort_if_needed( assoc);
   return( _mulle__assoc_get_in_range( (struct mulle__assoc *) assoc,
                                       range,
                                       buf));
}


static inline unsigned int
   mulle_assoc_get_in_range( struct mulle_assoc *assoc,
                             struct mulle_range range,
                             struct mulle_pointerpair *buf)
{
   mulle_assoc_qsort_if_needed( assoc);
   return( mulle__assoc_get_in_range( (struct mulle__assoc *) assoc,
                                      range,
                                      buf));
}


static inline struct mulle_pointerpair
   mulle_assoc_get_last( struct mulle_assoc *assoc)
{
   if( ! assoc)
      return( mulle_pointerpair_make_invalid());

   mulle_assoc_qsort_if_needed( assoc);
   return( _mulle__assoc_get_last( (struct mulle__assoc *) assoc));
}


static inline void   mulle_assoc_remove_last( struct mulle_assoc *assoc)
{
   if( assoc)
   {
      mulle_assoc_qsort_if_needed( assoc);
      _mulle__assoc_remove_last( (struct mulle__assoc *) assoc,
                                 assoc->callback,
                                 assoc->allocator);
   }
}

static inline void
   mulle_assoc_add_assoc_range( struct mulle_assoc *assoc,
                                struct mulle_assoc *other,
                                struct mulle_range range)
{
   assert( ! assoc || ! other || assoc->callback == other->callback);
   assoc->_is_sorted = 0;
   mulle__assoc_add_assoc_range( (struct mulle__assoc *) assoc,
                                 (struct mulle__assoc *) other,
                                 range,
                                 assoc->callback,
                                 assoc->allocator);
}

static inline void
   mulle_assoc_add_assoc( struct mulle_assoc *assoc,
                          struct mulle_assoc *other)
{
   mulle_assoc_add_assoc_range( assoc,
                                other,
                                mulle_range_make_all());
}


static inline struct mulle_assoc   *mulle_assoc_copy( struct mulle_assoc *assoc)
{
   struct mulle_assoc   *other;

   if( ! assoc)
      return( NULL);

   // can't allow creation to be done by struct mulle__assoc
   other = mulle_assoc_create( _mulle_assoc_get_count( assoc),
                               assoc->callback,
                               assoc->compare,
                               assoc->allocator);
   _mulle__assoc_copy_items( (struct mulle__assoc *) other,
                             (struct mulle__assoc *) assoc,
                             assoc->callback,
                             assoc->allocator);
   return( other);
}



# pragma mark - debugging

// use default allocator for returned string
MULLE_C_NONNULL_FIRST
static inline char   *_mulle_assoc_describe( struct mulle_assoc *assoc)
{
   mulle_assoc_qsort_if_needed( assoc);
   return( _mulle__assoc_describe( (struct mulle__assoc *) assoc,
                                    assoc->callback,
                                    NULL));
}


static inline char
   *mulle_assoc_describe( struct mulle_assoc *assoc)
{
   mulle_assoc_qsort_if_needed( assoc);
   return( mulle__assoc_describe( (struct mulle__assoc *) assoc,
                                   assoc->callback,
                                   NULL));
}


static inline int   mulle_assoc_member( struct mulle_assoc *assoc,
                                        void *key)
{
   return( mulle__assoc_member( (struct mulle__assoc *) assoc, key, assoc->callback));
}


#pragma mark - enumeration

/*
 *  If the assoc gets manipulated in the loop, enumeration will likely fail.
 *
 *  struct mulle_assocenumerator   rover;
 *  void                           *item;
 *
 *  rover = mulle_assoc_enumerate( assoc);
 *  while( mulle_assocenumerator_next( &rover, &item))
 *     printf( "%s\n", (char *) item);
 *  mulle_assocenumerator_done( &rover);
 *
 * or use
 *
 *  char   *item;
 *
 *  mulle_assoc_for( assoc, item)
 *     printf( "%s\n", item);
 */
#define MULLE_ASSOCENUMERATOR_BASE   MULLE__ASSOCENUMERATOR_BASE

struct mulle_assocenumerator
{
   MULLE_ASSOCENUMERATOR_BASE;
};

#define mulle_assocenumerator_empty   ((struct mulle_assocenumerator) { 0})


//
// lots of shit code, because C compilers are broken and we want to
// have one '_' less in our type, yet can't alias struct x = struct y...
// (without typedef), but we also want to avoid base.base.base.base..
//
MULLE_C_NONNULL_FIRST
static inline struct mulle_assocenumerator
   _mulle_assoc_enumerate( struct mulle_assoc *assoc)
{
   struct mulle_assocenumerator    rval;
   struct mulle__assocenumerator   tmp;

   mulle_assoc_qsort_if_needed( assoc);

   tmp = _mulle__assoc_enumerate( (struct mulle__assoc *) assoc, assoc->callback);
   memcpy( &rval, &tmp, sizeof( struct mulle__assocenumerator));
   return( rval);
}


static inline struct mulle_assocenumerator
   mulle_assoc_enumerate( struct mulle_assoc *assoc)
{
   return( assoc
           ? _mulle_assoc_enumerate( assoc)
           : mulle_assocenumerator_empty);
}


// item is unchanged if rval is 0 (!)
MULLE_C_NONNULL_FIRST
static inline int
   _mulle_assocenumerator_next( struct mulle_assocenumerator *rover, void **key, void **value)
{
   return( _mulle__assocenumerator_next( (struct mulle__assocenumerator *) rover, key, value));
}


// item is unchanged if rval is 0 (!)
static inline int
   mulle_assocenumerator_next( struct mulle_assocenumerator *rover, void **key, void **value)
{
   return( mulle__assocenumerator_next( (struct mulle__assocenumerator *) rover, key, value));
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle_assocenumerator_done( struct mulle_assocenumerator *rover)
{
}


static inline void   mulle_assocenumerator_done( struct mulle_assocenumerator *rover)
{
}

// created by make-container-do.sh -ls --compare --type struct mulle_pointerpair    mulle-assoc.c

#define mulle_assoc_do( name, callback, compare)                             \
   for( struct mulle_assoc                                                   \
           name ## __container = MULLE_ASSOC_INIT( callback, compare, NULL), \
           *name = &name ## __container,                                     \
           *name ## __i = NULL;                                              \
        ! name ## __i;                                                       \
        name ## __i =                                                        \
        (                                                                    \
           _mulle_assoc_done( &name ## __container),                         \
           (void *) 0x1                                                      \
        )                                                                    \
      )                                                                      \
      for( int  name ## __j = 0;    /* break protection */                   \
           name ## __j < 1;                                                  \
           name ## __j++)



// created by make-container-for.sh src/assoc/mulle-assoc.c

#define mulle_assoc_for( name, key, value)                                                                     \
   assert( sizeof( key) == sizeof( void *));                                                                   \
   assert( sizeof( value) == sizeof( void *));                                                                 \
   for( struct mulle_assocenumerator                                                                           \
           rover__ ## key ## __ ## value = mulle_assoc_enumerate( name),                                       \
           *rover___  ## key ## __ ## value ## __i = (void *) 0;                                               \
        ! rover___  ## key ## __ ## value ## __i;                                                              \
        rover___ ## key ## __ ## value ## __i = (_mulle_assocenumerator_done( &rover__ ## key ## __ ## value), \
                                              (void *) 1))                                                     \
      while( _mulle_assocenumerator_next( &rover__ ## key ## __ ## value,                                      \
                                      (void **) &key,                                                          \
                                      (void **) &value))




#endif /* mulle_assoc_h */


