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
#ifndef _mulle_queue__h__
#define _mulle_queue__h__

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>
#include "mulle_container_callback.h"

struct mulle_allocator;

//
// a struct _mulle_queue is LIFO, you can't use it as a stack
//

#define _MULLE_QUEUE_BASE                       \
   struct _mulle_queuebucket   *_spares;        \
   struct _mulle_queuebucket   *_read;          \
   struct _mulle_queuebucket   *_write;         \
   size_t                     _count;           \
   unsigned short             _bucket_size;     \
                                                \
   unsigned short             _read_index;      \
   unsigned short             _write_index;     \
   unsigned short             _spare_allowance


struct _mulle_queue
{
   _MULLE_QUEUE_BASE;
};


struct _mulle_queuebucket
{
   struct _mulle_queuebucket  *_next;
   void                       *_storage[ 1];
};


static inline void  _mulle_queue_init( struct _mulle_queue *queue,
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


struct _mulle_queue   *_mulle_queue_create( unsigned short bucket_size,
                                            unsigned short spare_allowance,
                                            struct mulle_allocator *allocator);

void   _mulle_queue_free( struct _mulle_queue *queue,  struct mulle_allocator *allocator);
void   _mulle_queue_done( struct _mulle_queue *queue,  struct mulle_allocator *allocator);

void   _mulle_queue_grow( struct _mulle_queue *queue, struct mulle_allocator *allocator);
void   _mulle_queue_shrink( struct _mulle_queue *queue, struct mulle_allocator *allocator);
void   _mulle_queue_reset( struct _mulle_queue *queue, struct mulle_allocator *allocator);

void   _mulle_queue_removeall( struct _mulle_queue *queue, struct mulle_allocator *allocator);  // keeps spares


struct _mulle_queueenumerator
{  
   struct _mulle_queuebucket  *_curr;
   unsigned short             _index;
};


static inline struct _mulle_queueenumerator   _mulle_queue_enumerate( struct _mulle_queue *queue)
{
   struct _mulle_queueenumerator   rover;
   
   rover._curr  = queue->_read;
   rover._index = queue->_read_index;

   return( rover);
}


static inline void   _mulle_queueenumerator_done( struct _mulle_queueenumerator *rover)
{
}


static inline void   *_mulle_queueenumerator_next( struct _mulle_queue *queue, struct _mulle_queueenumerator *rover)
{
   extern void   *__mulle_queueenumerator_next( struct _mulle_queue *,
                                               struct _mulle_queueenumerator *);
   unsigned short   limit;
   
   // if rover._curr == NULL, set rover->_index to queue->_bucket_size
   limit = rover->_curr != queue->_write ? queue->_bucket_size : queue->_write_index;
   if( rover->_index < limit)
      return( rover->_curr->_storage[ rover->_index++]);
      
   return( __mulle_queueenumerator_next( queue, rover));
}


static inline void  _mulle_queue_push( struct _mulle_queue *queue,
                                      void *obj,
                                      struct mulle_container_keycallback *callback,
                                      struct mulle_allocator *allocator)
{
   assert( obj);
   
   if( queue->_write_index >= queue->_bucket_size)
      _mulle_queue_grow( queue, allocator);

   obj = (*callback->retain)( callback, obj, allocator);

   queue->_count++;
   queue->_write->_storage[ queue->_write_index++] = obj;
}


void  *_mulle_queue_pop( struct _mulle_queue *queue,
                        struct mulle_container_keycallback *callback,
                        struct mulle_allocator *allocator);


static inline size_t   _mulle_queue_get_count( struct _mulle_queue *queue)
{
   return( queue->_count);
}

static inline unsigned short    _mulle_queue_get_bucketsize( struct _mulle_queue *queue)
{
   return( queue->_bucket_size);
}



#endif
