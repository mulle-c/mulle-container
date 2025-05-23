//
//  mulle-pointerqueue.h
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
#ifndef mulle__pointerqueue_h__
#define mulle__pointerqueue_h__

#include "mulle-container-callback.h"

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>

#include <mulle-allocator/mulle-allocator.h>

struct mulle__pointerqueuebucket;

#define MULLE__POINTERQUEUE_BASE                         \
   struct mulle__pointerqueuebucket   *_spares;          \
   struct mulle__pointerqueuebucket   *_read;            \
   struct mulle__pointerqueuebucket   *_write;           \
   size_t                       _count;            \
   size_t                       _read_index;       \
   size_t                       _write_index;      \
                                                         \
   unsigned short                     _bucket_size;      \
   unsigned short                     _spare_allowance


struct mulle__pointerqueue
{
   MULLE__POINTERQUEUE_BASE;
};


#define MULLE__POINTERQUEUE_DATA( xbucket_size, xspare_allowance) \
   ((struct mulle__pointerqueue)                                  \
   {                                                              \
      ._bucket_size     = xbucket_size,                           \
      ._read_index      = xbucket_size,                           \
      ._write_index     = xbucket_size,                           \
      ._spare_allowance = xspare_allowance                        \
   })

struct mulle__pointerqueuebucket
{
   struct mulle__pointerqueuebucket  *_next;
   void                              *_storage[ 1];
};


static inline struct mulle__pointerqueue *
  mulle__pointerqueue_alloc( struct mulle_allocator *allocator)
{
   struct mulle__pointerqueue   *queue;

   queue = mulle_allocator_malloc( allocator, sizeof( struct mulle__pointerqueue));
   return( queue);
}


static inline void
  mulle__pointerqueue_free( struct mulle__pointerqueue *queue,
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
   _mulle__pointerqueue_init( struct mulle__pointerqueue *queue,
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

   queue->_spare_allowance = spare_allowance;
}


MULLE__CONTAINER_GLOBAL
struct mulle__pointerqueue   *mulle__pointerqueue_create( unsigned short bucket_size,
                                                          unsigned short spare_allowance,
                                                          struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__pointerqueue_destroy( struct mulle__pointerqueue *queue,
                                     struct mulle_allocator *allocator);
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__pointerqueue_done( struct mulle__pointerqueue *queue,
                                  struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__pointerqueue_grow( struct mulle__pointerqueue *queue,
                                  struct mulle_allocator *allocator);
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__pointerqueue_shrink( struct mulle__pointerqueue *queue,
                                    struct mulle_allocator *allocator);
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__pointerqueue_reset( struct mulle__pointerqueue *queue,
                                   struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__pointerqueue_removeall( struct mulle__pointerqueue *queue,
                                       struct mulle_allocator *allocator);  // keeps spares


MULLE_C_NONNULL_FIRST_SECOND
static inline void  _mulle__pointerqueue_push( struct mulle__pointerqueue *queue,
                                               void *p,
                                               struct mulle_allocator *allocator)
{
   assert( p);

   if( queue->_write_index >= queue->_bucket_size)
      _mulle__pointerqueue_grow( queue, allocator);

   assert( queue->_write_index < queue->_bucket_size);

   queue->_count++;
   queue->_write->_storage[ queue->_write_index++] = p;
}


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void  *_mulle__pointerqueue_pop( struct mulle__pointerqueue *queue,
                                 struct mulle_allocator *allocator);


MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle__pointerqueue_get_count( struct mulle__pointerqueue *queue)
{
   return( queue->_count);
}


MULLE_C_NONNULL_FIRST
static inline unsigned short
   _mulle__pointerqueue_get_bucketsize( struct mulle__pointerqueue *queue)
{
   return( queue->_bucket_size);
}


/*
 * The enumerator interface is rarely useful, since you can NOT use
 * it to manipulate the queue.
 */
#define MULLE__POINTERQUEUEENUMERATOR_BASE      \
   struct mulle__pointerqueue        *_queue;   \
   struct mulle__pointerqueuebucket  *_curr;    \
   size_t                       _index


struct mulle__pointerqueueenumerator
{
   MULLE__POINTERQUEUEENUMERATOR_BASE;
};


#define mulle__pointerqueueenumerator_empty  \
   ((struct mulle__pointerqueueenumerator) { 0 })


static inline struct mulle__pointerqueueenumerator
   _mulle__pointerqueue_enumerate( struct mulle__pointerqueue *queue)
{
   struct mulle__pointerqueueenumerator   rover;

   rover._queue = queue;
   rover._curr  = queue->_read;
   rover._index = queue->_read_index;
   return( rover);
}


static inline struct mulle__pointerqueueenumerator
   mulle__pointerqueue_enumerate( struct mulle__pointerqueue *queue)
{
   if( queue)
      return( _mulle__pointerqueue_enumerate( queue));
   return( mulle__pointerqueueenumerator_empty);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline int
   _mulle__pointerqueueenumerator_next( struct mulle__pointerqueueenumerator *rover,
                                        void **item)
{
   extern int   __mulle__pointerqueueenumerator_next( struct mulle__pointerqueueenumerator *,
                                                      void **item);
   struct mulle__pointerqueue  *queue;
   size_t                limit;

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
      *item = rover->_curr->_storage[ rover->_index++];
      return( 1);
   }

   return( __mulle__pointerqueueenumerator_next( rover, item));
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle__pointerqueueenumerator_done( struct mulle__pointerqueueenumerator *rover)
{
   MULLE_C_UNUSED( rover);
}

static inline void   mulle__pointerqueueenumerator_done( struct mulle__pointerqueueenumerator *rover)
{
   MULLE_C_UNUSED( rover);
}


// created by make-container-do.sh mulle--pointerqueue.c
// and then hand edited
#define mulle__pointerqueue_do( name)                              \
   for( struct mulle__pointerqueue                                 \
           name ## __container = MULLE__POINTERQUEUE_DATA( 64, 0), \
           *name = &name ## __container,                           \
           *name ## __i = NULL;                                    \
        ! name ## __i;                                             \
        name ## __i =                                              \
        (                                                          \
           _mulle__pointerqueue_done( &name ## __container, NULL), \
           (void *) 0x1                                            \
        )                                                          \
      )                                                            \
      for( int  name ## __j = 0;    /* break protection */         \
           name ## __j < 1;                                        \
           name ## __j++)



// created by make-container-for.sh src/set/pointer/mulle--pointerqueue.c

#define mulle__pointerqueue_for( name, item)                                               \
   assert( sizeof( item) == sizeof( void *));                                              \
   for( struct mulle__pointerqueueenumerator                                               \
           rover__ ## item = mulle__pointerqueue_enumerate( name),                         \
           *rover__  ## item ## __i = (void *) 0;                                         \
        ! rover__  ## item ## __i;                                                        \
        rover__ ## item ## __i = (_mulle__pointerqueueenumerator_done( &rover__ ## item), \
                                   (void *) 1))                                            \
      while( _mulle__pointerqueueenumerator_next( &rover__ ## item, (void **) &item))


#endif
