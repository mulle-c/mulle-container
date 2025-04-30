//  mulle--array.h
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
#ifndef mulle__array_h__
#define mulle__array_h__

#include "mulle--pointerarray.h"
#include "mulle-container-callback.h"

#include <stddef.h>
#include <string.h>
#include <assert.h>



//
// mulle--array accepts mulle_container_keycallback. Elements in the array are
// retained on addition and released on removal. Furthermore equality is no
// longer determined by pointer equality but by an is_equal callback.
// mulle--array ignores the hash function.
//

#define MULLE__ARRAY_BASE   MULLE__POINTERARRAY_BASE


struct mulle__array
{
   MULLE__ARRAY_BASE;
};


#define MULLE__ARRAY_DATA( storage, count)  \
   MULLE__POINTERARRAY_DATA( storage, count)



MULLE_C_NONNULL_FIRST
static inline void    _mulle__array_init( struct mulle__array *array,
                                          size_t capacity,
                                          struct mulle_allocator *allocator)
{
   _mulle__pointerarray_init( (struct mulle__pointerarray *) array,
                              capacity,
                              allocator);
}


static inline struct mulle__array   *
   mulle__array_create( struct mulle_allocator *allocator)
{
   return( (struct mulle__array *) mulle__pointerarray_create( allocator));
}

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__array_destroy( struct mulle__array *array,
                              struct mulle_container_keycallback *callback,
                              struct mulle_allocator *allocator);

// function should work for not properly initialized with mulle__array_init
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST // MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__array_done( struct mulle__array *array,
                           struct mulle_container_keycallback *callback,
                           struct mulle_allocator *allocator);



# pragma mark - petty accessors

MULLE_C_NONNULL_FIRST
static inline void **
   _mulle__array_get_storage( struct mulle__array *array)
{
   return( _mulle__pointerarray_get_storage( (struct mulle__pointerarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle__array_get_size( struct mulle__array *array)
{
   return( _mulle__pointerarray_get_size( (struct mulle__pointerarray *) array));
}


static inline size_t
   mulle__array_get_size( struct mulle__array *array)
{
   return( mulle__pointerarray_get_size( (struct mulle__pointerarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle__array_get_guaranteed_size( struct mulle__array *array)
{
   return( _mulle__pointerarray_get_guaranteed_size( (struct mulle__pointerarray *) array));
}


static inline size_t
   mulle__array_get_guaranteed_size( struct mulle__array *array)
{
   return( mulle__pointerarray_get_guaranteed_size( (struct mulle__pointerarray *) array));
}



MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle__array_get_count( struct mulle__array *array)
{
   return( _mulle__pointerarray_get_count( (struct mulle__pointerarray *) array));
}


static inline size_t
   mulle__array_get_count( struct mulle__array *array)
{
   return( mulle__pointerarray_get_count( (struct mulle__pointerarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline int
   _mulle__array_is_full( struct mulle__array *array)
{
   return( _mulle__pointerarray_is_full( (struct mulle__pointerarray *) array));
}


static inline int
   mulle__array_is_full( struct mulle__array *array)
{
   return( mulle__pointerarray_is_full( (struct mulle__pointerarray *) array));
}


# pragma mark - callback access with mutation check


#if MULLE__CONTAINER_HAVE_MUTATION_COUNT

static inline int   _mulle__array_callback_equal( struct mulle__array *array,
                                                  struct mulle_container_keycallback *callback,
                                                  void *a,
                                                  void *b)
{
   uintptr_t   memo_array;
   int         is_equal;

   memo_array = array->_n_mutations;
   is_equal   = (*callback->is_equal)( callback, a, b);
   assert( array->_n_mutations == memo_array && "array was modified during is_equal callback");

   return( is_equal);
}


static inline void   *_mulle__array_callback_retain( struct mulle__array *array,
                                                     struct mulle_container_keycallback *callback,
                                                     void *p,
                                                     struct mulle_allocator *allocator)
{
   uintptr_t   memo_array;

   memo_array = array->_n_mutations;
   p          = (*callback->retain)( callback, p, allocator);
   assert( array->_n_mutations == memo_array && "array was modified during retain callback");
   return( p);
}


static inline void   _mulle__array_callback_release( struct mulle__array *array,
                                                     struct mulle_container_keycallback *callback,
                                                     void *p,
                                                     struct mulle_allocator *allocator)
{
   uintptr_t   memo_array;

   memo_array = array->_n_mutations;
   (*callback->release)( callback, p, allocator);
   assert( array->_n_mutations == memo_array && "array was modified during release callback");

}

static inline char   *_mulle__array_callback_describe( struct mulle__array *array,
                                                       struct mulle_container_keycallback *callback,
                                                       void *p,
                                                       struct mulle_allocator **allocator)
{
   uintptr_t   memo_array;
   char        *s;

   memo_array = array->_n_mutations;
   s          = (*callback->describe)( callback, p, allocator);
   assert( array->_n_mutations == memo_array && "array was modified during release callback");
   return( s);
}


#else

static inline int   _mulle__array_callback_equal( struct mulle__array *array,
                                                  struct mulle_container_keycallback *callback,
                                                  void *a,
                                                  void *b)
{
   int   is_equal;

   MULLE_C_UNUSED( array);

   is_equal = (*callback->is_equal)( callback, a, b);
   return( is_equal);
}


static inline void   *_mulle__array_callback_retain( struct mulle__array *array,
                                                     struct mulle_container_keycallback *callback,
                                                     void *p,
                                                     struct mulle_allocator *allocator)
{
   MULLE_C_UNUSED( array);

   p = (*callback->retain)( callback, p, allocator);
   return( p);
}


static inline void   _mulle__array_callback_release( struct mulle__array *array,
                                                     struct mulle_container_keycallback *callback,
                                                     void *p,
                                                     struct mulle_allocator *allocator)
{
   MULLE_C_UNUSED( array);

   (*callback->release)( callback, p, allocator);
}


static inline char   *_mulle__array_callback_describe( struct mulle__array *array,
                                                       struct mulle_container_keycallback *callback,
                                                       void *p,
                                                       struct mulle_allocator **allocator)
{
   MULLE_C_UNUSED( array);

   return( (*callback->describe)( callback, p, allocator));
}

#endif



# pragma mark - memory operations

MULLE_C_NONNULL_FIRST
static inline void **
   _mulle__array_guarantee( struct mulle__array *array,
                            size_t length,
                            struct mulle_allocator *allocator)
{
   return( _mulle__pointerarray_guarantee( (struct mulle__pointerarray *) array,
                                           length,
                                           allocator));
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle__array_grow( struct mulle__array *array,
                       struct mulle_allocator *allocator)
{
   _mulle__pointerarray_grow( (struct mulle__pointerarray *) array,
                              allocator);
}


# pragma mark - operations

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_THIRD
void   _mulle__array_remove_in_range( struct mulle__array *array,
                                      struct mulle_range location,
                                      struct mulle_container_keycallback *callback,
                                      struct mulle_allocator *allocator);

// convenience, that uses find in range and then remove in range
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_THIRD
void   _mulle__array_remove( struct mulle__array *array,
                             void *item,
                             struct mulle_container_keycallback *callback,
                             struct mulle_allocator *allocator);

// convenience, that uses find in range and then remove in range
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_THIRD
void   _mulle__array_remove_unique( struct mulle__array *array,
                                  void *item,
                                  struct mulle_container_keycallback *callback,
                                  struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__array_reset( struct mulle__array *array,
                            struct mulle_container_keycallback *callback,
                            struct mulle_allocator *allocator);



// other may be NULL
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_THIRD
int    _mulle__array_is_equal( struct mulle__array *array,
                               struct mulle__array *other,
                               struct mulle_container_keycallback *callback);


// here the caller guarantees that there is enough space left to add
// also he has to guarantee that p is not callback->notakey, which will only
// be asserted
MULLE_C_NONNULL_FIRST_THIRD
static inline
void    _mulle__array_add_guaranteed( struct mulle__array *array,
                                      void  *p,
                                      struct mulle_container_keycallback *callback,
                                      struct mulle_allocator *allocator)
{
   assert( p != callback->notakey);

   p = _mulle__array_callback_retain( array, callback, p, allocator);
   _mulle__pointerarray_add_guaranteed( (struct mulle__pointerarray *)  array,
                                       p);
}

// if you add notakey, it leads to abort / assert
MULLE_C_NONNULL_FIRST_THIRD
void    _mulle__array_add( struct mulle__array *array,
                           void  *p,
                           struct mulle_container_keycallback *callback,
                           struct mulle_allocator *allocator);


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_THIRD
void    _mulle__array_set( struct mulle__array *array,
                           size_t i,
                           void  *p,
                           struct mulle_container_keycallback *callback,
                           struct mulle_allocator *allocator);

MULLE_C_NONNULL_THIRD
static inline void
   mulle__array_set( struct mulle__array *array,
                     size_t i,
                     void  *p,
                     struct mulle_container_keycallback *callback,
                     struct mulle_allocator *allocator)
{
   if( array)
      return( _mulle__array_set( array, i, p, callback, allocator));
}


MULLE_C_NONNULL_FIRST
static inline void   *
   _mulle__array_get( struct mulle__array *array,
                      size_t i)
{
   return( _mulle__pointerarray_get( (struct mulle__pointerarray *) array, i));
}


/*
 *  Loop over all items. This works as long as you don't remove
 *  anything from the array. It will not retrieve newly added elements.
 *
 *  size_t  i, n;
 *  size_t  *item;
 *
 *  for( i = 0, n = mulle__array_get_count( array); i < n; i++)
 *  {
 *     item = mulle__array_get( array, i);
 *     printf( "%s", (char *) item);
 *  }
 *
 *  Reverse loop over all items. This works as long as you don't remove
 *  anything but the last element from the array. It will not retrieve newly
 *  added elements.
 *
 *  size_t  i;
 *  size_t  *item;
 *
 *  for( i = mulle__array_get_count( array); i;)
 *  {
 *     item = mulle__array_get( array, --i);
 *     printf( "%s", (char *) item);
 *  }
 *
 */
static inline void   *
   mulle__array_get( struct mulle__array *array, size_t i)
{
   return( mulle__pointerarray_get( (struct mulle__pointerarray *) array, i));
}


MULLE_C_NONNULL_FIRST_THIRD
static inline size_t
   _mulle__array_get_in_range( struct mulle__array *array,
                               struct mulle_range range,
                               void *buf)
{
   return( _mulle__pointerarray_get_in_range( (struct mulle__pointerarray *) array,
                                              range,
                                              buf));
}


// TODO: maybe rename to get_pointers_in_range
static inline size_t
   mulle__array_get_in_range( struct mulle__array *array,
                              struct mulle_range range,
                              void *buf)
{
   return( mulle__pointerarray_get_in_range( (struct mulle__pointerarray *) array,
                                              range,
                                              buf));
}


MULLE_C_NONNULL_FIRST
static inline
void   *_mulle__array_get_last( struct mulle__array *array)
{
   return( _mulle__pointerarray_get_last( (struct mulle__pointerarray *) array));
}


MULLE_C_NONNULL_FIRST_SECOND
static inline
void  _mulle__array_remove_last( struct mulle__array *array,
                                 struct mulle_container_keycallback *callback,
                                 struct mulle_allocator *allocator)
{
   void   *p;

   p = _mulle__pointerarray_pop( (struct mulle__pointerarray *) array);
   _mulle__array_callback_release( array, callback, p, allocator);
}


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_FOURTH
void   _mulle__array_add_array( struct mulle__array *array,
                                struct mulle__array *other,
                                struct mulle_range range,
                                struct mulle_container_keycallback *callback,
                                struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_SECOND_THIRD
void    _mulle__array_copy_items( struct mulle__array *dst,
                                 struct mulle__array *src,
                                 struct mulle_container_keycallback *callback,
                                 struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__array_reset( struct mulle__array *array,
                            struct mulle_container_keycallback *callback,
                            struct mulle_allocator *allocator);


MULLE_C_NONNULL_FIRST
static inline uintptr_t
   _mulle__array_find_in_range_identical( struct mulle__array *array,
                                          void *obj,
                                          struct mulle_range range)
{
   return( _mulle__pointerarray_find_in_range( (struct mulle__pointerarray *) array,
                                                obj,
                                                range));
}


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
uintptr_t  _mulle__array_find_in_range( struct mulle__array *array,
                                        void *obj,
                                        struct mulle_range range,
                                        struct mulle_container_keycallback *callback);



MULLE_C_NONNULL_FIRST
static inline void
   _mulle__array_qsort_r_inline( struct mulle__array *array,
                                 mulle_pointerarray_cmp_t *compare,
                                 void *userinfo)
{
   _mulle__pointerarray_qsort_r_inline( (struct mulle__pointerarray *) array,
                                        compare,
                                        userinfo);
}


static inline void
   mulle__array_qsort_r_inline( struct mulle__array *array,
                                mulle_pointerarray_cmp_t *compare,
                                void *userinfo)
{
   if( array)
      _mulle__array_qsort_r_inline( array, compare, userinfo);
}


# pragma mark - debugging

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_SECOND
char   *_mulle__array_describe( struct mulle__array *array,
                                struct mulle_container_keycallback *callback,
                                struct mulle_allocator *allocator);


MULLE_C_NONNULL_SECOND
static inline
char   *mulle__array_describe( struct mulle__array *array,
                               struct mulle_container_keycallback *callback,
                               struct mulle_allocator *allocator)
{
   if( ! array)
      return( mulle_allocator_strdup( allocator, "NULL"));
   return( _mulle__array_describe( array, callback, allocator));
}


#pragma mark - enumeration

/*
 *  Enumeration is likely to fail, if the array gets manipulated in the
 *  loop.
 *
 *  struct mulle__arrayenumerator   rover;
 *  void                           *item;
 *
 *  rover = mulle__array_enumerate( array, callback);
 *  while( mulle__arrayenumerator_next( &rover, &item))
 *     printf( "%s", (char *) item);
 *  mulle__arrayenumerator_done( &rover);
 *
 */

#define MULLE__ARRAYENUMERATOR_BASE                 \
   struct mulle__pointerarrayenumerator   base;     \
   void                                   *notakey

struct mulle__arrayenumerator
{
   MULLE__ARRAYENUMERATOR_BASE;
};

#define mulle__arrayenumerator_empty   ((struct mulle__arrayenumerator) { 0})


MULLE_C_NONNULL_FIRST
static inline struct mulle__arrayenumerator
   _mulle__array_enumerate( struct mulle__array *array,
                            struct mulle_container_keycallback *callback)
{
   struct mulle__arrayenumerator   rover;

   rover.base    = _mulle__pointerarray_enumerate( (struct mulle__pointerarray *) array);
   rover.notakey = callback->notakey;
   return( *(struct mulle__arrayenumerator *) &rover);
}


static inline struct mulle__arrayenumerator
   mulle__array_enumerate( struct mulle__array *array,
                           struct mulle_container_keycallback *callback)
{
   if( ! array || ! callback)
      return( mulle__arrayenumerator_empty);
   return( _mulle__array_enumerate( array, callback));
}


MULLE_C_NONNULL_FIRST_SECOND
static inline int
   _mulle__arrayenumerator_next( struct mulle__arrayenumerator *rover, void **item)
{
   if( rover->base._curr < rover->base._sentinel)
   {
      *item = *rover->base._curr++;
      return( 1);
   }
   *item = rover->notakey;
   return( 0);
}


static inline int
   mulle__arrayenumerator_next( struct mulle__arrayenumerator *rover, void **item)
{
   void   *dummy;

   if( ! item)
      item = &dummy;
   if( rover)
      return( _mulle__arrayenumerator_next( rover, item));
   *item = NULL;
   return( 0);
}


MULLE_C_NONNULL_FIRST
static inline void
  _mulle__arrayenumerator_done( struct mulle__arrayenumerator *rover)
{
   MULLE_C_UNUSED( rover);
}


static inline void
  mulle__arrayenumerator_done( struct mulle__arrayenumerator *rover)
{
   MULLE_C_UNUSED( rover);
}


#pragma mark - reverse enumeration

/*
 *  Enumeration is likely to fail, if the array gets manipulated in the
 *  loop.
 *
 *  struct mulle__arrayreverseenumerator   rover;
 *  void                                  *item;
 *
 *  rover = mulle__array_reverseenumerate( array, callback);
 *  while( mulle__arrayreverseenumerator_next( &rover, &item))
 *     printf( "%s", item);
 *  mulle__arrayreverseenumerator_done( &rover);
 *  printf( "\n");
 *
 */

#define MULLE__ARRAYREVERSEENUMERATOR_BASE                 \
   struct mulle__pointerarrayreverseenumerator   base;     \
   void                                          *notakey

struct mulle__arrayreverseenumerator
{
   MULLE__ARRAYREVERSEENUMERATOR_BASE;
};

#define mulle__arrayreverseenumerator_empty \
   ((struct mulle__arrayreverseenumerator) { 0 })


MULLE_C_NONNULL_FIRST
static inline struct  mulle__arrayreverseenumerator
   _mulle__array_reverseenumerate( struct mulle__array *array,
                                   struct mulle_container_keycallback *callback)
{
   struct mulle__arrayreverseenumerator   rover;

   rover.base    = _mulle__pointerarray_reverseenumerate( (struct mulle__pointerarray *) array);
   rover.notakey = callback->notakey;
   return( rover);
}


static inline struct  mulle__arrayreverseenumerator
   mulle__array_reverseenumerate( struct mulle__array *array,
                                  struct mulle_container_keycallback *callback)
{
   if( ! array || ! callback)
      return( mulle__arrayreverseenumerator_empty);
   return( _mulle__array_reverseenumerate( array, callback));
}


MULLE_C_NONNULL_FIRST_SECOND
static inline int
   _mulle__arrayreverseenumerator_next( struct mulle__arrayreverseenumerator *rover,
                                        void **item)
{
   if( rover->base._curr == rover->base._sentinel)
   {
      *item = rover->notakey;
      return( 0);
   }

   *item = *--rover->base._curr;
   return( 1);
}


static inline int
   mulle__arrayreverseenumerator_next( struct mulle__arrayreverseenumerator *rover,
                                       void **item)
{
   void   *dummy;

   if( ! item)
      item = &dummy;
   if( rover)
      return( _mulle__arrayreverseenumerator_next( rover, item));
   if( item)
      *item = NULL;
   return( 0);
}


MULLE_C_NONNULL_FIRST
static inline void
  _mulle__arrayreverseenumerator_done( struct mulle__arrayreverseenumerator *rover)
{
   MULLE_C_UNUSED( rover);
}


static inline void
  mulle__arrayreverseenumerator_done( struct mulle__arrayreverseenumerator *rover)
{
   MULLE_C_UNUSED( rover);
}

/*
 *
 */
int   mulle__array_member( struct mulle__array *array,
                           void *p,
                           struct mulle_container_keycallback *callback);


// created by make-container-do.sh --flexible mulle--array.c

#define mulle__array_do( name, callback)                              \
   for( struct mulle__array                                           \
           name ## __container = { 0 },                               \
           *name = &name ## __container,                              \
           *name ## __i = NULL;                                       \
        ! name ## __i;                                                \
        name ## __i =                                                 \
        (                                                             \
           _mulle__array_done( &name ## __container, callback, NULL), \
           (void *) 0x1                                               \
        )                                                             \
      )                                                               \
      for( int  name ## __j = 0;    /* break protection */            \
           name ## __j < 1;                                           \
           name ## __j++)

#define mulle__array_do_flexible( name, stackcount, callback)         \
   void   *name ## __storage[ stackcount];                            \
   for( struct mulle__array                                           \
           name ## __container =                                      \
              MULLE__ARRAY_DATA( name ## __storage, stackcount),      \
           *name = &name ## __container,                              \
           *name ## __i = NULL;                                       \
        ! name ## __i;                                                \
        name ## __i =                                                 \
        (                                                             \
           _mulle__array_done( &name ## __container, callback, NULL), \
           (void *) 0x1                                               \
        )                                                             \
      )                                                               \
      for( int  name ## __j = 0;    /* break protection */            \
           name ## __j < 1;                                           \
           name ## __j++)


// created by make-container-for.sh src/array/mulle--array.c

#define mulle__array_for( name, callback, item)                                     \
   assert( sizeof( item) == sizeof( void *));                                       \
   for( struct mulle__arrayenumerator                                               \
           rover__ ## item = mulle__array_enumerate( name, callback),               \
           *rover__  ## item ## __i = (void *) 0;                                  \
        ! rover__  ## item ## __i;                                                 \
        rover__ ## item ## __i = (_mulle__arrayenumerator_done( &rover__ ## item), \
                                   (void *) 1))                                     \
      while( _mulle__arrayenumerator_next( &rover__ ## item, (void **) &item))


// created by make-container-for.sh --reverse src/array/mulle--array.c

#define mulle__array_for_reverse( name, callback, item)                                    \
   assert( sizeof( item) == sizeof( void *));                                              \
   for( struct mulle__arrayreverseenumerator                                               \
           rover__ ## item = mulle__array_reverseenumerate( name, callback),               \
           *rover__  ## item ## __i = (void *) 0;                                         \
        ! rover__  ## item ## __i;                                                        \
        rover__ ## item ## __i = (_mulle__arrayreverseenumerator_done( &rover__ ## item), \
                                   (void *) 1))                                            \
      while( _mulle__arrayreverseenumerator_next( &rover__ ## item, (void **) &item))

#endif

