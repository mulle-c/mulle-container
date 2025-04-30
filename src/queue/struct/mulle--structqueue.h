//
//  mulle-structqueue.h
//  mulle-container
//
//  Copyright (c) 2011 Mulle kybernetiK. All rights reserved.
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
#ifndef mulle__structqueue_h__
#define mulle__structqueue_h__

#include "mulle-container-callback.h"

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>

#include <mulle-allocator/mulle-allocator.h>

struct mulle__structqueuebucket;

#define MULLE__STRUCTQUEUE_BASE                            \
   struct mulle__structqueuebucket    *_spares;            \
   struct mulle__structqueuebucket    *_read;              \
   struct mulle__structqueuebucket    *_write;             \
   size_t                       _count;              \
   size_t                       _read_index;         \
   size_t                       _write_index;        \
                                                           \
   size_t                       _sizeof_struct;      \
   size_t                       _copy_sizeof_struct; \
   unsigned short                     _bucket_size;        \
   unsigned short                     _spare_allowance


#define MULLE__STRUCTQUEUE_ALIGNED_SIZE( type)             \
   (size_t) (sizeof( type) + (sizeof( type) % alignof( type)))


struct mulle__structqueue
{
   MULLE__STRUCTQUEUE_BASE;
};

#define MULLE__STRUCTQUEUE_DATA( type)                              \
   ((struct mulle__structqueue)                                     \
   {                                                                \
      ._bucket_size        = 64,                                    \
      ._read_index         = 64,                                    \
      ._write_index        = 64,                                    \
      ._copy_sizeof_struct = (size_t) sizeof( type),          \
      ._sizeof_struct      = MULLE__STRUCTQUEUE_ALIGNED_SIZE( type) \
   })



struct mulle__structqueuebucket
{
   union
   {
      struct mulle__structqueuebucket  *_next;
      double                           _alignment;
   };
   char   _storage[ 1];
};


static inline struct mulle__structqueue *
  mulle__structqueue_alloc( struct mulle_allocator *allocator)
{
   struct mulle__structqueue   *queue;

   queue = mulle_allocator_malloc( allocator, sizeof( struct mulle__structqueue));
   return( queue);
}


static inline void
  mulle__structqueue_free( struct mulle__structqueue *queue,
                           struct mulle_allocator *allocator)
{
   mulle_allocator_free( allocator, queue);
}

//
// Large bucket_size is convenient for mostly add queues, but also a bit
// wasteful.
//
// spare_allowance can be used to cache freed buckets. That's useful in
// scenarios where you reset the queue often
//
MULLE_C_NONNULL_FIRST
static inline void
   _mulle__structqueue_init( struct mulle__structqueue *queue,
                             size_t sizeof_struct,
                             size_t alignof_struct,
                             unsigned short bucket_size,
                             unsigned short spare_allowance)
{
   if( ! bucket_size)
      bucket_size = 64;

   queue->_spares      =
   queue->_read        =
   queue->_write       = 0;

   queue->_count       = 0;

   queue->_bucket_size =
   queue->_read_index  =
   queue->_write_index = bucket_size >= 4 ? bucket_size : 4;

   assert( sizeof_struct == (size_t) sizeof_struct);
   assert( alignof_struct <= alignof( double));

   queue->_sizeof_struct       = (size_t) (sizeof_struct + (sizeof_struct % alignof_struct));
   queue->_copy_sizeof_struct  = (size_t) sizeof_struct;
   queue->_spare_allowance     = spare_allowance;
}


MULLE__CONTAINER_GLOBAL
struct mulle__structqueue   *
   mulle__structqueue_create( size_t sizeof_struct,
                              size_t alignof_struct,
                              unsigned short bucket_size,
                              unsigned short spare_allowance,
                              struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__structqueue_destroy( struct mulle__structqueue *queue,
                                    struct mulle_allocator *allocator);
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__structqueue_done( struct mulle__structqueue *queue,
                                 struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__structqueue_grow( struct mulle__structqueue *queue,
                                 struct mulle_allocator *allocator);
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__structqueue_shrink( struct mulle__structqueue *queue,
                                   struct mulle_allocator *allocator);
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__structqueue_reset( struct mulle__structqueue *queue,
                                  struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__structqueue_removeall( struct mulle__structqueue *queue,
                                      struct mulle_allocator *allocator);  // keeps spares


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void  *_mulle__structqueue_reserve( struct mulle__structqueue *queue,
                                    struct mulle_allocator *allocator);


MULLE_C_NONNULL_FIRST_SECOND
static inline void  _mulle__structqueue_push( struct mulle__structqueue *queue,
                                              void *p,
                                              struct mulle_allocator *allocator)
{
   void   *q;

   q = _mulle__structqueue_reserve( queue, allocator);
   memcpy( q, p, queue->_copy_sizeof_struct);
}


// copies last element into (if successfull 1) and p != NULL
//
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
int   _mulle__structqueue_pop( struct mulle__structqueue *queue,
                               struct mulle_allocator *allocator,
                               void *p);


MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle__structqueue_get_count( struct mulle__structqueue *queue)
{
   return( queue->_count);
}


MULLE_C_NONNULL_FIRST
static inline unsigned short
   _mulle__structqueue_get_bucketsize( struct mulle__structqueue *queue)
{
   return( queue->_bucket_size);
}


MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle__structqueue_get_element_size( struct mulle__structqueue *queue)
{
   return( queue->_sizeof_struct);
}


/*
 * The enumerator interface is rarely useful, since you can NOT use
 * it to manipulate the queue.
 */
#define MULLE__STRUCTQUEUEENUMERATOR_BASE      \
   struct mulle__structqueue        *_queue;   \
   struct mulle__structqueuebucket  *_curr;    \
   size_t                      _index


struct mulle__structqueueenumerator
{
   MULLE__STRUCTQUEUEENUMERATOR_BASE;
};


#define mulle__structqueueenumerator_empty  \
   ((struct mulle__structqueueenumerator) { 0 })


static inline struct mulle__structqueueenumerator
   _mulle__structqueue_enumerate( struct mulle__structqueue *queue)
{
   struct mulle__structqueueenumerator   rover;

   rover._queue = queue;
   rover._curr  = queue->_read;
   rover._index = queue->_read_index;
   return( rover);
}


static inline struct mulle__structqueueenumerator
   mulle__structqueue_enumerate( struct mulle__structqueue *queue)
{
   if( queue)
      return( _mulle__structqueue_enumerate( queue));
   return( mulle__structqueueenumerator_empty);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline int
   _mulle__structqueueenumerator_next( struct mulle__structqueueenumerator *rover,
                                       void **item)
{
   extern int   __mulle__structqueueenumerator_next( struct mulle__structqueueenumerator *,
                                                     void **item);
   struct mulle__structqueue  *queue;
   size_t                limit;
   size_t                offset;

   queue = rover->_queue;
   if( ! queue)
   {
      *item = NULL;
      return( 0);
   }

   // if rover._curr == NULL, set rover->_index to queue->_bucket_size
   limit = rover->_curr != queue->_write ? queue->_bucket_size : queue->_write_index;
   if( rover->_index < limit)
   {
      offset = rover->_index++ * queue->_sizeof_struct;
      *item  = &rover->_curr->_storage[ offset];
      return( 1);
   }

   return( __mulle__structqueueenumerator_next( rover, item));
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle__structqueueenumerator_done( struct mulle__structqueueenumerator *rover)
{
   MULLE_C_UNUSED( rover);
}


static inline void
   mulle__structqueueenumerator_done( struct mulle__structqueueenumerator *rover)
{
   MULLE_C_UNUSED( rover);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline void
   _mulle__structqueue_assert_pointer( struct mulle__structqueue *queue, void *p)
{
#ifndef NDEBUG
   struct mulle__structqueueenumerator   rover;
   void                                  *item;

   rover = _mulle__structqueue_enumerate( queue);
   while( _mulle__structqueueenumerator_next( &rover, &item))
   {
      if( item == p)
         break;
   }
   _mulle__structqueueenumerator_done( &rover);
   assert( item == p);
#else
   MULLE_C_UNUSED( queue);
   MULLE_C_UNUSED( p);
#endif
}


// created by make-container-do.sh src/queue/struct/mulle--structqueue.c
// but hand edited signature and the MULLE__STRUCTQUEUE_DATA

#define mulle__structqueue_do( name, type)                        \
   for( struct mulle__structqueue                                 \
           name ## __container = MULLE__STRUCTQUEUE_DATA( type),  \
           *name = &name ## __container,                          \
           *name ## __i = NULL;                                   \
        ! name ## __i;                                            \
        name ## __i =                                             \
        (                                                         \
           _mulle__structqueue_done( &name ## __container, NULL), \
           (void *) 0x1                                           \
        )                                                         \
      )                                                           \
      for( int  name ## __j = 0;    /* break protection */        \
           name ## __j < 1;                                       \
           name ## __j++)


// created by make-container-for.sh src/queue/struct/mulle--structqueue.c

#define mulle__structqueue_for( name, item)                                               \
   assert( sizeof( item) == sizeof( void *));                                             \
   for( struct mulle__structqueueenumerator                                               \
           rover__ ## item = mulle__structqueue_enumerate( name),                         \
           *rover__  ## item ## __i = (void *) 0;                                        \
        ! rover__  ## item ## __i;                                                       \
        rover__ ## item ## __i = (_mulle__structqueueenumerator_done( &rover__ ## item), \
                                   (void *) 1))                                           \
      while( _mulle__structqueueenumerator_next( &rover__ ## item, (void **) &item))


#endif
