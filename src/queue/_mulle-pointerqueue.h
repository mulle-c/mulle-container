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
#ifndef _mulle_pointerqueue__h__
#define _mulle_pointerqueue__h__

#include "mulle-container-callback.h"

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>

struct mulle_allocator;

//
// a struct _mulle_pointerqueue is FIFO, you can't use it as a stack.
// As a queue has a pop operation, it is not useful to put
// memory management into the queue, as the queue would need to;
// release on pop, which would be disastrous if the release does
// a free (copied C-string for example)
//
#define _MULLE_POINTERQUEUE_BASE                         \
   struct _mulle_pointerqueuebucket   *_spares;          \
   struct _mulle_pointerqueuebucket   *_read;            \
   struct _mulle_pointerqueuebucket   *_write;           \
   unsigned int                       _count;            \
   unsigned int                       _read_index;       \
   unsigned int                       _write_index;      \
                                                         \
   unsigned short                      _bucket_size;     \
   unsigned short                      _spare_allowance


struct _mulle_pointerqueue
{
   _MULLE_POINTERQUEUE_BASE;
};


struct _mulle_pointerqueuebucket
{
   struct _mulle_pointerqueuebucket  *_next;
   void                              *_storage[ 1];
};


static inline struct _mulle_pointerqueue *
  _mulle_pointerqueue_alloc( struct mulle_allocator *allocator)
{
   struct _mulle_pointerqueue   *queue;

   queue = mulle_allocator_malloc( allocator, sizeof( struct _mulle_pointerqueue));
   return( queue);
}


static inline void
  _mulle_pointerqueue_free( struct _mulle_pointerqueue *queue,
                             struct mulle_allocator *allocator)
{
   mulle_allocator_free( allocator, queue);
}


static inline void  _mulle_pointerqueue_init( struct _mulle_pointerqueue *queue,
                                              unsigned short bucket_size,
                                              unsigned short spare_allowance)
{
   queue->_spares      =
   queue->_read        =
   queue->_write       = 0;

   queue->_count       = 0;

   queue->_bucket_size =
   queue->_read_index  =
   queue->_write_index = bucket_size >= 4 ? bucket_size : 4;

   queue->_spare_allowance = spare_allowance;
}


struct _mulle_pointerqueue   *_mulle_pointerqueue_create( unsigned short bucket_size,
                                                          unsigned short spare_allowance,
                                                          struct mulle_allocator *allocator);

void   _mulle_pointerqueue_destroy( struct _mulle_pointerqueue *queue,
                                    struct mulle_allocator *allocator);
void   _mulle_pointerqueue_done( struct _mulle_pointerqueue *queue,
                                 struct mulle_allocator *allocator);

void   _mulle_pointerqueue_grow( struct _mulle_pointerqueue *queue,
                                 struct mulle_allocator *allocator);
void   _mulle_pointerqueue_shrink( struct _mulle_pointerqueue *queue,
                                   struct mulle_allocator *allocator);
void   _mulle_pointerqueue_reset( struct _mulle_pointerqueue *queue,
                                  struct mulle_allocator *allocator);

void   _mulle_pointerqueue_removeall( struct _mulle_pointerqueue *queue,
                                      struct mulle_allocator *allocator);  // keeps spares


static inline void  _mulle_pointerqueue_push( struct _mulle_pointerqueue *queue,
                                              void *p,
                                              struct mulle_allocator *allocator)
{
   assert( p);

   if( queue->_write_index >= queue->_bucket_size)
      _mulle_pointerqueue_grow( queue, allocator);

   queue->_count++;
   queue->_write->_storage[ queue->_write_index++] = p;
}


static inline void  *
  _mulle_pointerqueue_pop_noshrink( struct _mulle_pointerqueue *queue)
{
   void   *p;

   // if nothing to read, read pointer points at write
   if( queue->_read == queue->_write)
      if( queue->_read_index == queue->_write_index)
         return( NULL);

   queue->_count--;
   //
   // now if "release" truely releases, p is pointing to a zombie.
   // not so, if release is autorelease or nop
   // check one case with an assert
   //
   p = queue->_read->_storage[ queue->_read_index++];
   return( p);
}


void  *_mulle_pointerqueue_pop( struct _mulle_pointerqueue *queue,
                                struct mulle_allocator *allocator);


static inline unsigned int
   _mulle_pointerqueue_get_count( struct _mulle_pointerqueue *queue)
{
   return( queue->_count);
}


static inline unsigned short
   _mulle_pointerqueue_get_bucketsize( struct _mulle_pointerqueue *queue)
{
   return( queue->_bucket_size);
}


/*
 * The enumerator interface is rarely useful, since you can NOT use
 * it to manipulate the queue.
 */
struct _mulle_pointerqueueenumerator
{
   struct _mulle_pointerqueue        *_queue;
   struct _mulle_pointerqueuebucket  *_curr;
   unsigned int                       _index;
};


static inline struct _mulle_pointerqueueenumerator
   _mulle_pointerqueue_enumerate( struct _mulle_pointerqueue *queue)
{
   struct _mulle_pointerqueueenumerator   rover;

   rover._queue = queue;
   rover._curr  = queue->_read;
   rover._index = queue->_read_index;

   return( rover);
}


static inline void   _mulle_pointerqueueenumerator_done( struct _mulle_pointerqueueenumerator *rover)
{
}


static inline void   *_mulle_pointerqueueenumerator_next( struct _mulle_pointerqueueenumerator *rover)
{
   extern void   *__mulle_pointerqueueenumerator_next( struct _mulle_pointerqueueenumerator *);
   struct _mulle_pointerqueue  *queue;
   unsigned int                limit;

   queue = rover->_queue;
   // if rover._curr == NULL, set rover->_index to queue->_bucket_size
   limit = rover->_curr != queue->_write ? queue->_bucket_size : queue->_write_index;
   if( rover->_index < limit)
      return( rover->_curr->_storage[ rover->_index++]);

   return( __mulle_pointerqueueenumerator_next( rover));
}


#endif
