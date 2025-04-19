//  mulle--assoc.h
//  mulle-container
//
//  Copyright (C) 2011 Nat!, Mulle kybernetiK.
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
#ifndef mulle__assoc_h__
#define mulle__assoc_h__

#include "mulle--pointerpairarray.h"
#include "mulle-container-callback.h"

#include <stddef.h>
#include <string.h>
#include <assert.h>

// MEMO: the mulle--assoc API is awkward to use, because you have to push the
// callbacks all the time, even to the getter for on-demand autosorting.
// It just serves as a basis for mulle-assoc, where the convenience lives.
//
// mulle--assoc is an autosorting associative array.
// That the underlying storage is currently a "pointerpair" array is an
// implementation detail, that may/will change in the future.
//
// The disadvantage is that hashed lookup is usually faster and has better
// memory access characteristica. TODO: Redesign
// according to https://github.com/patmorin/arraylayout for faster bsearches ?
//
// mulle--assoc accepts mulle_container_keycallback. Elements in the assoc are
// retained on addition and released on removal. Furthermore equality is no
// longer determined by pointer equality but by an is_equal callback.
// mulle--assoc ignores the hash function.
//
// TODO: make this also autosorting so mulle_assoc embeds callbacks and 
// allocator like usual...

#define MULLE__ASSOC_BASE                                  \
   MULLE__POINTERPAIRARRAY_BASE;                           \
   int    _is_sorted


struct mulle__assoc
{
   MULLE__ASSOC_BASE;
};


MULLE_C_NONNULL_FIRST
static inline void    _mulle__assoc_init( struct mulle__assoc *assoc,
                                          size_t capacity,
                                          struct mulle_allocator *allocator)
{
   _mulle__pointerpairarray_init( (struct mulle__pointerpairarray *) assoc,
                                  capacity,
                                  allocator);
   assoc->_is_sorted = 0;
}


static inline struct mulle__assoc   *
   mulle__assoc_create( struct mulle_allocator *allocator)
{
   return( (struct mulle__assoc *) mulle__pointerpairarray_create( allocator));
}



MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__assoc_destroy( struct mulle__assoc *assoc,
                              struct mulle_container_keyvaluecallback *callback,
                              struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST  // _SECOND
void   _mulle__assoc_done( struct mulle__assoc *assoc,
                           struct mulle_container_keyvaluecallback *callback,
                           struct mulle_allocator *allocator);



# pragma mark - petty accessors


MULLE_C_NONNULL_FIRST
static inline int
_mulle__assoc_is_sorted( struct mulle__assoc *assoc)
{
   return( assoc->_is_sorted);
}


static inline int
mulle__assoc_is_sorted( struct mulle__assoc *assoc)
{
   return( assoc ? _mulle__assoc_is_sorted( assoc) : 1); // NULL is always sorted
}


static inline void
_mulle__assoc_set_unsorted( struct mulle__assoc *assoc)
{
   assoc->_is_sorted = 0;
}


MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle__assoc_get_size( struct mulle__assoc *assoc)
{
   return( _mulle__pointerpairarray_get_size( (struct mulle__pointerpairarray *) assoc));
}


static inline size_t
   mulle__assoc_get_size( struct mulle__assoc *assoc)
{
   return( mulle__pointerpairarray_get_size( (struct mulle__pointerpairarray *) assoc));
}


MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle__assoc_get_guaranteed_size( struct mulle__assoc *assoc)
{
   return( _mulle__pointerpairarray_get_guaranteed_size( (struct mulle__pointerpairarray *) assoc));
}


static inline size_t
   mulle__assoc_get_guaranteed_size( struct mulle__assoc *assoc)
{
   return( mulle__pointerpairarray_get_guaranteed_size( (struct mulle__pointerpairarray *) assoc));
}



MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle__assoc_get_count( struct mulle__assoc *assoc)
{
   return( _mulle__pointerpairarray_get_count( (struct mulle__pointerpairarray *) assoc));
}


static inline size_t
   mulle__assoc_get_count( struct mulle__assoc *assoc)
{
   return( mulle__pointerpairarray_get_count( (struct mulle__pointerpairarray *) assoc));
}


MULLE_C_NONNULL_FIRST
static inline int
   _mulle__assoc_is_full( struct mulle__assoc *assoc)
{
   return( _mulle__pointerpairarray_is_full( (struct mulle__pointerpairarray *) assoc));
}


static inline int
   mulle__assoc_is_full( struct mulle__assoc *assoc)
{
   return( mulle__pointerpairarray_is_full( (struct mulle__pointerpairarray *) assoc));
}


# pragma mark - sorting / autosorting support

MULLE_C_NONNULL_FIRST
static inline void   _mulle__assoc_qsort_r( struct mulle__assoc *assoc,
                                            mulle_pointerpair_compare_t *compare,
                                            void *userinfo)
{
   mulle__pointerpairarray_qsort_r( (struct mulle__pointerpairarray *) assoc,
                                    compare,
                                    userinfo);
   assoc->_is_sorted = 1;
}



static inline void   mulle__assoc_qsort_r( struct mulle__assoc *assoc,
                                           mulle_pointerpair_compare_t *compare,
                                           void *userinfo)
{
   if( assoc)
      _mulle__assoc_qsort_r( assoc, compare, userinfo);
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle__assoc_qsort_r_if_needed( struct mulle__assoc *assoc,
                                                      mulle_pointerpair_compare_t *compare,
                                                      void *userinfo)
{
   if( ! _mulle__assoc_is_sorted( assoc))
      _mulle__assoc_qsort_r( assoc, compare, userinfo);
}


static inline void   mulle__assoc_qsort_if_needed( struct mulle__assoc *assoc,
                                                   mulle_pointerpair_compare_t *compare,
                                                   void *userinfo)
{
   if( assoc)
      _mulle__assoc_qsort_r_if_needed( assoc, compare, userinfo);
}


# pragma mark - operations

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_THIRD
void   _mulle__assoc_remove_in_range( struct mulle__assoc *assoc,
                                      struct mulle_range range,
                                      struct mulle_container_keyvaluecallback *callback,
                                      struct mulle_allocator *allocator);

// convenience, that uses find in range and then remove in range
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_THIRD
void   _mulle__assoc_remove( struct mulle__assoc *assoc,
                             void *key,
                             struct mulle_container_keyvaluecallback *callback,
                             mulle_pointerpair_compare_t *compare,
                             void *userinfo,
                             struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__assoc_reset( struct mulle__assoc *assoc,
                            struct mulle_container_keyvaluecallback *callback,
                            struct mulle_allocator *allocator);



// other may be NULL, Values will be pointer compared, keys according to callback
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_THIRD
int    _mulle__assoc_is_equal( struct mulle__assoc *assoc,
                               struct mulle__assoc *other,
                               struct mulle_container_keyvaluecallback *callback,
                               mulle_pointerpair_compare_t *compare,
                               void *userinfo);

// if you add notakey, it leads to abort / assert
MULLE_C_NONNULL_FIRST_THIRD
void    _mulle__assoc_add( struct mulle__assoc *assoc,
                           void *key,
                           void *value,
                           struct mulle_container_keyvaluecallback *callback,
                           struct mulle_allocator *allocator);

static inline void
   mulle__assoc_add( struct mulle__assoc *assoc,
                     void *key,
                     void *value,
                     struct mulle_container_keyvaluecallback *callback,
                     struct mulle_allocator *allocator)
{
   if( assoc)
      _mulle__assoc_add( assoc, key, value, callback, allocator);
}


MULLE_C_NONNULL_FIRST_THIRD
static inline void
   _mulle__assoc_add_guaranteed( struct mulle__assoc *assoc,
                                 void *key,
                                 void *value,
                                 struct mulle_container_keyvaluecallback *callback,
                                 struct mulle_allocator *allocator)
{
   struct mulle_pointerpair   retained;
   struct mulle_pointerpair   pair;

   pair     = mulle_pointerpair_make( key, value);
   retained = mulle_pointerpair_retain( pair, callback, allocator);
   _mulle__pointerpairarray_add_guaranteed( (struct mulle__pointerpairarray *) assoc, retained);
   assoc->_is_sorted = 0;
}


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_FIFTH
void    _mulle__assoc_set_at_index( struct mulle__assoc *assoc,
                                    size_t i,
                                    void *key,
                                    void *value,
                                    struct mulle_container_keyvaluecallback *callback,
                                    mulle_pointerpair_compare_t *compare,
                                    void *userinfo,
                                    struct mulle_allocator *allocator);




/*
 *  Loop over all items. This works as long as you don't remove
 *  anything from the assoc. It will not retrieve newly added elements.
 *
 *  size_t  i, n;
 *  size_t  *item;
 *
 *  for( i = 0, n = mulle__assoc_get_count( assoc); i < n; i++)
 *  {
 *     item = mulle__assoc_get( assoc, i);
 *     printf( "%s", (char *) item);
 *  }
 *
 *  Reverse loop over all items. This works as long as you don't remove
 *  anything but the last element from the assoc. It will not retrieve newly
 *  added elements.
 *
 *  size_t  i;
 *  size_t  *item;
 *
 *  for( i = mulle__assoc_get_count( assoc); i;)
 *  {
 *     item = mulle__assoc_get( assoc, --i);
 *     printf( "%s", (char *) item);
 *  }
 *
 */
MULLE_C_NONNULL_FIRST_THIRD
static inline size_t
   _mulle__assoc_get_in_range( struct mulle__assoc *assoc,
                               struct mulle_range range,
                               struct mulle_pointerpair *buf)
{
   return( _mulle__pointerpairarray_get_in_range( (struct mulle__pointerpairarray *) assoc,
                                                   range,
                                                   buf));
}


// TODO: maybe rename to get_pointers_in_range
static inline size_t
   mulle__assoc_get_in_range( struct mulle__assoc *assoc,
                              struct mulle_range range,
                              struct mulle_pointerpair *buf)
{
   return( mulle__pointerpairarray_get_in_range( (struct mulle__pointerpairarray *) assoc,
                                                 range,
                                                 buf));
}


MULLE_C_NONNULL_FIRST
static inline
struct mulle_pointerpair  _mulle__assoc_get_last( struct mulle__assoc *assoc)
{
   return( _mulle__pointerpairarray_get_last( (struct mulle__pointerpairarray *) assoc));
}


MULLE_C_NONNULL_FIRST_SECOND
static inline
void  _mulle__assoc_remove_last( struct mulle__assoc *assoc,
                                 struct mulle_container_keyvaluecallback *callback,
                                 struct mulle_allocator *allocator)
{
   struct mulle_pointerpair   p;

   p = _mulle__pointerpairarray_pop( (struct mulle__pointerpairarray *) assoc);
   mulle_pointerpair_release( p, callback, allocator);
}


MULLE__CONTAINER_GLOBAL
void   _mulle__assoc_add_assoc_range( struct mulle__assoc *assoc,
                                      struct mulle__assoc *other,
                                      struct mulle_range range,
                                      struct mulle_container_keyvaluecallback *callback,
                                      struct mulle_allocator *allocator);

static inline
void   mulle__assoc_add_assoc( struct mulle__assoc *assoc,
                               struct mulle__assoc *other,
                               struct mulle_container_keyvaluecallback *callback,
                               struct mulle_allocator *allocator)
{
   if( assoc && other)
      _mulle__assoc_add_assoc_range( assoc,
                                     other,
                                     mulle_range_make_all(),
                                     callback,
                                     allocator);
}


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_SECOND_THIRD
void _mulle__assoc_copy_items( struct mulle__assoc *dst,
                               struct mulle__assoc *src,
                               struct mulle_container_keyvaluecallback *callback,
                               struct mulle_allocator *allocator);


#pragma mark - sort and search


//
// you can pass NULL for callback, and it will just compare pointer equality
// TODO: make this uniform across library for find ? also for search ?
//
MULLE__CONTAINER_GLOBAL
uintptr_t   _mulle__assoc_find_in_range( struct mulle__assoc *assoc,
                                         void *key,
                                         struct mulle_range range,
                                         struct mulle_container_keyvaluecallback *callback,
                                         mulle_pointerpair_compare_t *compare,
                                         void *userinfo);


static inline uintptr_t
   mulle__assoc_find_in_range( struct mulle__assoc *assoc,
                               void *key,
                               struct mulle_range range,
                               struct mulle_container_keyvaluecallback *callback,
                               mulle_pointerpair_compare_t compare,
                               void *userinfo)
{
   if( ! assoc)
      return( mulle_not_found_e);
   return( _mulle__assoc_find_in_range( assoc, key, range, callback, compare, userinfo));
}

MULLE_C_NONNULL_FIRST_THIRD
static inline uintptr_t
    _mulle__assoc_find( struct mulle__assoc *assoc,
                        void *key,
                        struct mulle_container_keyvaluecallback *callback,
                        mulle_pointerpair_compare_t compare,
                        void *userinfo)
{
   return( _mulle__assoc_find_in_range( assoc,
                                        key,
                                        mulle_range_make_all(),
                                        callback,
                                        compare,
                                        userinfo));
}


static inline uintptr_t
   mulle__assoc_find( struct mulle__assoc *assoc,
                      void *key,
                      struct mulle_container_keyvaluecallback *callback,
                      mulle_pointerpair_compare_t compare,
                      void *userinfo)
{
   if ( ! assoc)
      return( mulle_not_found_e);

   return( _mulle__assoc_find_in_range( assoc,
                                        key,
                                        mulle_range_make_all(),
                                        callback,
                                        compare,
                                        userinfo));

}




MULLE_C_NONNULL_FIRST
static inline
struct mulle_pointerpair  _mulle__assoc_get_at_index( struct mulle__assoc *assoc,
                                                      size_t index,
                                                      mulle_pointerpair_compare_t *compare,
                                                      void *userinfo)
{
   _mulle__assoc_qsort_r_if_needed( assoc, compare, userinfo);
   return( _mulle__pointerpairarray_get( (struct mulle__pointerpairarray *) assoc, index));
}



static inline struct mulle_pointerpair
   mulle__assoc_get_at_index( struct mulle__assoc *assoc,
                              size_t i,
                              mulle_pointerpair_compare_t *compare,
                              void *userinfo)
{
   if( ! assoc)
      return( mulle_pointerpair_make_invalid());
   _mulle__assoc_qsort_r_if_needed( assoc, compare, userinfo);
   return( _mulle__pointerpairarray_get( (struct mulle__pointerpairarray *) assoc, i));
}



MULLE_C_NONNULL_FIRST
static inline uintptr_t
   _mulle__assoc_find_callback( struct mulle__assoc *assoc,
                                void *key,
                                struct mulle_container_keyvaluecallback *callback,
                                mulle_pointerpair_compare_t *compare,
                                void *userinfo)
{
   struct mulle_pointerpair   search;

   _mulle__assoc_qsort_r_if_needed( assoc, compare, userinfo);
   search = mulle_pointerpair_make( key, NULL);
   return( _mulle__pointerpairarray_find_callback( (struct mulle__pointerpairarray *) assoc,
                                                   search,
                                                   callback));
}


static inline uintptr_t
   mulle__assoc_find_callback( struct mulle__assoc *assoc,
                               void *key,
                               struct mulle_container_keyvaluecallback *callback,
                               mulle_pointerpair_compare_t *compare,
                               void *userinfo)
{
   if( ! assoc)
      return( mulle_not_found_e);

   return( _mulle__assoc_find_callback( assoc, key, callback, compare, userinfo));
}


MULLE_C_NONNULL_FIRST
static inline uintptr_t
   _mulle__assoc_find_compare( struct mulle__assoc *assoc,
                               void *key,
                               mulle_pointerpair_compare_t *compare,
                               void *userinfo)
{
   struct mulle_pointerpair   search;

   search = mulle_pointerpair_make( key, NULL);
   return( _mulle__pointerpairarray_find_compare( (struct mulle__pointerpairarray *) assoc,
                                                  search,
                                                  compare,
                                                  userinfo));
}


static inline uintptr_t
   mulle__assoc_find_compare( struct mulle__assoc *assoc,
                              void *key,
                              mulle_pointerpair_compare_t *compare,
                              void *userinfo)
{
   if( ! assoc)
      return( mulle_not_found_e);

   return( _mulle__assoc_find_compare( assoc, key, compare, userinfo));
}

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   *_mulle__assoc_get( struct mulle__assoc *assoc,
                           void *key,
                           mulle_pointerpair_compare_t *compare,
                           void *userinfo);

static inline
void   *mulle__assoc_get( struct mulle__assoc *assoc,
                          void *key,
                          mulle_pointerpair_compare_t *compare,
                          void *userinfo)
{
   if( ! assoc)
      return( NULL);
   return( _mulle__assoc_get( assoc, key, compare, userinfo));
}


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__assoc_set( struct mulle__assoc *assoc,
                          void *key,
                          void *value,
                          struct mulle_container_keyvaluecallback *callback,
                          mulle_pointerpair_compare_t *compare,
                          void *userinfo,
                          struct mulle_allocator *allocator);

// And a non-underscore version
static inline void
mulle__assoc_set( struct mulle__assoc *assoc,
                  void *key,
                  void *value,
                  mulle_pointerpair_compare_t *compare,
                  void *userinfo,
                  struct mulle_container_keyvaluecallback *callback,
                  struct mulle_allocator *allocator)
{
   if (assoc)
      _mulle__assoc_set( assoc, key, value, callback, compare, userinfo, allocator);
}


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__assoc_remap_intptr_key_range(struct mulle__assoc *assoc,
                                            struct mulle_range range,
                                            intptr_t offset,
                                            struct mulle_container_keyvaluecallback *callback,
                                            mulle_pointerpair_compare_t *compare,
                                            void *userinfo);

// And a non-underscore version
static inline void
   mulle__assoc_remap_intptr_key_range( struct mulle__assoc *assoc,
                                        struct mulle_range range,
                                        intptr_t offset,
                                        struct mulle_container_keyvaluecallback *callback,
                                        mulle_pointerpair_compare_t *compare,
                                        void *userinfo)
{
   if (assoc)
      _mulle__assoc_remap_intptr_key_range(assoc, range, offset, callback, compare, userinfo);
}

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__assoc_move_intptr_key_range( struct mulle__assoc *assoc,
                                            struct mulle_range range,
                                            intptr_t index,
                                            struct mulle_container_keyvaluecallback *callback,
                                            mulle_pointerpair_compare_t *compare,
                                            void *userinfo);

// And a non-underscore version
static inline void
mulle__assoc_move_intptr_key_range( struct mulle__assoc *assoc,
                                    struct mulle_range range,
                                    intptr_t index,
                                    struct mulle_container_keyvaluecallback *callback,
                                    mulle_pointerpair_compare_t *compare,
                                    void *userinfo)
{
   if (assoc)
      _mulle__assoc_move_intptr_key_range(assoc, range, index, callback, compare, userinfo);
}


# pragma mark - debugging

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_SECOND
char   *_mulle__assoc_describe( struct mulle__assoc *assoc,
                                struct mulle_container_keyvaluecallback *callback,
                                mulle_pointerpair_compare_t *compare,
                                void *userinfo,
                                struct mulle_allocator *allocator);


MULLE_C_NONNULL_SECOND
static inline
char   *mulle__assoc_describe( struct mulle__assoc *assoc,
                               struct mulle_container_keyvaluecallback *callback,
                               mulle_pointerpair_compare_t *compare,
                               void *userinfo,
                               struct mulle_allocator *allocator)
{
   if( ! assoc)
      return( mulle_allocator_strdup( allocator, "NULL"));
   return( _mulle__assoc_describe( assoc, callback, compare, userinfo, allocator));
}


#pragma mark - enumeration

/*
 *  Enumeration is likely to fail, if the assoc gets manipulated in the
 *  loop.
 *
 *  struct mulle__assocenumerator   rover;
 *  void                           *item;
 *
 *  rover = mulle__assoc_enumerate( assoc, callback);
 *  while( mulle__assocenumerator_next( &rover, &item))
 *     printf( "%s", (char *) item);
 *  mulle__assocenumerator_done( &rover);
 *
 */

#define MULLE__ASSOCENUMERATOR_BASE                     \
   struct mulle__pointerpairarrayenumerator   base;     \
   void                                       *notakey

struct mulle__assocenumerator
{
   MULLE__ASSOCENUMERATOR_BASE;
};

#define mulle__assocenumerator_empty   ((struct mulle__assocenumerator) { 0})


MULLE_C_NONNULL_FIRST
static inline struct mulle__assocenumerator
   _mulle__assoc_enumerate( struct mulle__assoc *assoc,
                            struct mulle_container_keyvaluecallback *callback,
                            mulle_pointerpair_compare_t *compare,
                            void *userinfo)
{
   struct mulle__assocenumerator   rover;

   _mulle__assoc_qsort_r_if_needed( assoc, compare, userinfo);
   rover.base    = _mulle__pointerpairarray_enumerate( (struct mulle__pointerpairarray *) assoc);
   rover.notakey = callback->keycallback.notakey;
   return( *(struct mulle__assocenumerator *) &rover);
}


static inline struct mulle__assocenumerator
   mulle__assoc_enumerate( struct mulle__assoc *assoc,
                           struct mulle_container_keyvaluecallback *callback,
                           mulle_pointerpair_compare_t *compare,
                           void *userinfo)
{
   if( ! assoc || ! callback)
      return( mulle__assocenumerator_empty);
   return( _mulle__assoc_enumerate( assoc, callback, compare, userinfo));
}


MULLE_C_NONNULL_FIRST_SECOND
static inline int
   _mulle__assocenumerator_next( struct mulle__assocenumerator *rover,
                                 void **key,
                                 void **value)
{
   struct mulle_pointerpair  item;
   int                       rval;

   rval = rover->base._curr < rover->base._sentinel;
   item = rval ? *rover->base._curr++ : mulle_pointerpair_make( rover->notakey, NULL);

   *key   = item.key;
   *value = item.value;

   return( rval);
}


static inline int
   mulle__assocenumerator_next( struct mulle__assocenumerator *rover,
                                void **key,
                                void **value)
{
   void   *dummy;

   key   = key   ? key   : &dummy;
   value = value ? value : &dummy;
   if( rover)
      return( _mulle__assocenumerator_next( rover, key, value));

   *key   = NULL;
   *value = NULL;
   return( 0);
}


MULLE_C_NONNULL_FIRST
static inline void
  _mulle__assocenumerator_done( struct mulle__assocenumerator *rover)
{
}


static inline void
  mulle__assocenumerator_done( struct mulle__assocenumerator *rover)
{
}


/*
 *
 */
MULLE__CONTAINER_GLOBAL
int   _mulle__assoc_member( struct mulle__assoc *assoc,
                            void *key,
                            struct mulle_container_keyvaluecallback *callback);

// created by make-container-do.sh -ls --compare --type struct mulle_pointerpair    mulle--assoc.c

#define mulle__assoc_do( name, callback)                              \
   for( struct mulle__assoc                                           \
           name ## __container = { 0 },                               \
           *name = &name ## __container,                              \
           *name ## __i = NULL;                                       \
        ! name ## __i;                                                \
        name ## __i =                                                 \
        (                                                             \
           _mulle__assoc_done( &name ## __container, callback, NULL), \
           (void *) 0x1                                               \
        )                                                             \
      )                                                               \
      for( int  name ## __j = 0;    /* break protection */            \
           name ## __j < 1;                                           \
           name ## __j++)



// created by make-container-for.sh src/assoc/mulle--assoc.c

#define mulle__assoc_for( name, callback, key, value)                                                           \
   assert( sizeof( key) == sizeof( void *));                                                                    \
   assert( sizeof( value) == sizeof( void *));                                                                  \
   for( struct mulle__assocenumerator                                                                           \
           rover__ ## key ## __ ## value = mulle__assoc_enumerate( name, callback),                             \
           *rover__  ## key ## __ ## value ## __i = (void *) 0;                                                \
        ! rover__  ## key ## __ ## value ## __i;                                                               \
        rover__ ## key ## __ ## value ## __i = (_mulle__assocenumerator_done( &rover__ ## key ## __ ## value), \
                                              (void *) 1))                                                      \
      while( _mulle__assocenumerator_next( &rover__ ## key ## __ ## value,                                      \
                                           (void **) &key,                                                           \
                                           (void **) &value))


#endif

