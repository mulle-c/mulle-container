/*
 *  MulleFoundation - A tiny Foundation replacement
 *
 *  _NSByteBuffer.h is a part of MulleFoundation
 *
 *  Copyright (C) 2011 Nat!, Mulle kybernetiK.
 *  All rights reserved.
 *
 *  Coded by Nat!
 *
 *  $Id$
 *
 */
#ifndef mulle__pointerqueue__h__
#define mulle__pointerqueue__h__

#include "mulle-container-callback.h"

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>

#include <mulle-allocator/mulle-allocator.h>

//
// A struct mulle__pointerqueue is a FIFO, you can't use it as a stack.
// As a queue has a pop operation. You can not store NULL into it. It can
// be faster than a mulle--pointerarray, since it doesn't realloc/memcpy.
//
// MEMO: It is not useful to put memory management into the queue, as the queue
// would need to release on pop, which would be disastrous if the release does
// a free (copied C-string for example).
//
struct mulle__pointerqueuebucket;

#define MULLE__POINTERQUEUE_BASE                         \
   struct mulle__pointerqueuebucket   *_spares;          \
   struct mulle__pointerqueuebucket   *_read;            \
   struct mulle__pointerqueuebucket   *_write;           \
   unsigned int                       count;            \
   unsigned int                       _read_index;       \
   unsigned int                       _write_index;      \
                                                         \
   unsigned short                     _bucket_size;      \
   unsigned short                     _spare_allowance


struct mulle__pointerqueue
{
   MULLE__POINTERQUEUE_BASE;
};


struct mulle__pointerqueuebucket
{
   struct mulle__pointerqueuebucket  *_next;
   void                              *storage[ 1];
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


// large bucket_size is convenient for mostly add
// spare_allowance can be to cache freed buckets, useful for many edit
// operations
MULLE_C_NONNULL_FIRST
static inline void  _mulle__pointerqueue_init( struct mulle__pointerqueue *queue,
                                               unsigned short bucket_size,
                                               unsigned short spare_allowance)
{
   queue->_spares      =
   queue->_read        =
   queue->_write       = 0;

   queue->count       = 0;

   queue->_bucket_size =
   queue->_read_index  =
   queue->_write_index = bucket_size >= 4 ? bucket_size : 4;

   queue->_spare_allowance = spare_allowance;
}


struct mulle__pointerqueue   *_mulle__pointerqueue_create( unsigned short bucket_size,
                                                           unsigned short spare_allowance,
                                                           struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST
void   _mulle__pointerqueue_destroy( struct mulle__pointerqueue *queue,
                                     struct mulle_allocator *allocator);
MULLE_C_NONNULL_FIRST
void   _mulle__pointerqueue_done( struct mulle__pointerqueue *queue,
                                  struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST
void   _mulle__pointerqueue_grow( struct mulle__pointerqueue *queue,
                                  struct mulle_allocator *allocator);
MULLE_C_NONNULL_FIRST
void   _mulle__pointerqueue_shrink( struct mulle__pointerqueue *queue,
                                    struct mulle_allocator *allocator);
MULLE_C_NONNULL_FIRST
void   _mulle__pointerqueue_reset( struct mulle__pointerqueue *queue,
                                   struct mulle_allocator *allocator);

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

   queue->count++;
   queue->_write->storage[ queue->_write_index++] = p;
}


MULLE_C_NONNULL_FIRST
void  *_mulle__pointerqueue_pop( struct mulle__pointerqueue *queue,
                                 struct mulle_allocator *allocator);


MULLE_C_NONNULL_FIRST
static inline unsigned int
   _mulle__pointerqueue_get_count( struct mulle__pointerqueue *queue)
{
   return( queue->count);
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
struct mulle__pointerqueueenumerator
{
   struct mulle__pointerqueue        *_queue;
   struct mulle__pointerqueuebucket  *_curr;
   unsigned int                       _index;
};


static inline struct mulle__pointerqueueenumerator
   mulle__pointerqueue_enumerate( struct mulle__pointerqueue *queue)
{
   struct mulle__pointerqueueenumerator   rover;

   if( queue)
   {
      rover._queue = queue;
      rover._curr  = queue->_read;
      rover._index = queue->_read_index;
   }
   else
      rover._queue = NULL;
   return( rover);
}


MULLE_C_NONNULL_FIRST
static inline void   *_mulle__pointerqueueenumerator_next( struct mulle__pointerqueueenumerator *rover)
{
   extern void   *__mulle__pointerqueueenumerator_next( struct mulle__pointerqueueenumerator *);

   struct mulle__pointerqueue  *queue;
   unsigned int                limit;

   queue = rover->_queue;
   if( ! queue)
      return( NULL);
   // if rover._curr == NULL, set rover->_index to queue->_bucket_size
   limit = rover->_curr != queue->_write ? queue->_bucket_size : queue->_write_index;
   if( rover->_index < limit)
      return( rover->_curr->storage[ rover->_index++]);

   return( __mulle__pointerqueueenumerator_next( rover));
}


static inline void   mulle__pointerqueueenumerator_done( struct mulle__pointerqueueenumerator *rover)
{
}


#endif
