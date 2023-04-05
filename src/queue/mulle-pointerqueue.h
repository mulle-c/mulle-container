//
//  mulle-pointerqueue.h
//  mulle-container
//
//  Created by Nat! on 13.02.2021
//  Copyright (c) 2021 Mulle kybernetiK. All rights reserved.
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
#ifndef mulle_pointerqueue__h__
#define mulle_pointerqueue__h__

#include "mulle--pointerqueue.h"

//
// A struct mulle_pointerqueue is a FIFO, you can't use it as a stack.
// As a queue it has a pop operation. You can not store NULL into it. It can
// be faster than a mulle-pointerarray, since it doesn't realloc/memcpy.
// Main use is for an ever growing collection of things.
//
// MEMO: It is not useful to put memory management into the queue, as the queue
// would need to release on pop, which would be disastrous if the release does
// a free (copied C-string for example). Or pop would have to return retained.
//
struct mulle_pointerqueue
{
   MULLE__POINTERQUEUE_BASE;
   struct mulle_allocator   *allocator;   // public...
};


// does not set the allocator, init does
static inline struct mulle_pointerqueue *
  mulle_pointerqueue_alloc( struct mulle_allocator *allocator)
{
   struct mulle_pointerqueue   *queue;

   queue = mulle_allocator_malloc( allocator, sizeof( struct mulle_pointerqueue));
   return( queue);
}


static inline void
  mulle_pointerqueue_free( struct mulle_pointerqueue *queue)
{
   if( queue)
      mulle_allocator_free( queue->allocator, queue);
}


// large bucket_size is convenient for mostly add
// spare_allowance can be to cache freed buckets, useful for many edit
// operations
MULLE_C_NONNULL_FIRST
static inline void  _mulle_pointerqueue_init( struct mulle_pointerqueue *queue,
                                              unsigned short bucket_size,
                                              unsigned short spare_allowance,
                                              struct mulle_allocator *allocator)
{
   _mulle__pointerqueue_init( (struct mulle__pointerqueue *) queue,
                              bucket_size,
                              spare_allowance);
   queue->allocator = allocator;
}


static inline void  mulle_pointerqueue_init( struct mulle_pointerqueue *queue,
                                             unsigned short bucket_size,
                                             unsigned short spare_allowance,
                                              struct mulle_allocator *allocator)
{
   if( queue)
      _mulle_pointerqueue_init( queue,
                                bucket_size,
                                spare_allowance,
                                allocator);
}

static inline void  mulle_pointerqueue_init_default( struct mulle_pointerqueue *queue,
                                                     struct mulle_allocator *allocator)
{
   if( queue)
      _mulle_pointerqueue_init( queue, 256, 16, allocator);
}



MULLE_CONTAINER_GLOBAL
struct mulle_pointerqueue *
   mulle_pointerqueue_create( unsigned short bucket_size,
                              unsigned short spare_allowance,
                              struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST
static inline
void   _mulle_pointerqueue_destroy( struct mulle_pointerqueue *queue)
{
   _mulle__pointerqueue_destroy( (struct mulle__pointerqueue *) queue,
                                 queue->allocator);
}

static inline
void   mulle_pointerqueue_destroy( struct mulle_pointerqueue *queue)
{
   if( queue)
      _mulle_pointerqueue_destroy( queue);
}


MULLE_C_NONNULL_FIRST
static inline
void   _mulle_pointerqueue_done( struct mulle_pointerqueue *queue)
{
   _mulle__pointerqueue_done( (struct mulle__pointerqueue *) queue,
                              queue->allocator);
}


static inline
void   mulle_pointerqueue_done( struct mulle_pointerqueue *queue)
{
   if( queue)
      _mulle_pointerqueue_done( queue);
}



MULLE_C_NONNULL_FIRST
static inline
void   _mulle_pointerqueue_grow( struct mulle_pointerqueue *queue)
{
   _mulle__pointerqueue_grow( (struct mulle__pointerqueue *) queue,
                              queue->allocator);
}



static inline
void   mulle_pointerqueue_grow( struct mulle_pointerqueue *queue)
{
   if( queue)
      _mulle_pointerqueue_grow( queue);
}


MULLE_C_NONNULL_FIRST
static inline
void   _mulle_pointerqueue_shrink( struct mulle_pointerqueue *queue)
{
   _mulle__pointerqueue_shrink( (struct mulle__pointerqueue *) queue,
                                queue->allocator);
}


static inline
void   mulle_pointerqueue_shrink( struct mulle_pointerqueue *queue)
{
   if( queue)
      _mulle_pointerqueue_shrink( queue);
}


MULLE_C_NONNULL_FIRST
static inline
void   _mulle_pointerqueue_reset( struct mulle_pointerqueue *queue)
{
   _mulle__pointerqueue_reset( (struct mulle__pointerqueue *) queue,
                               queue->allocator);
}


static inline
void   mulle_pointerqueue_reset( struct mulle_pointerqueue *queue)
{
   if( queue)
      _mulle_pointerqueue_reset( queue);
}


MULLE_C_NONNULL_FIRST
static inline
void   _mulle_pointerqueue_removeall( struct mulle_pointerqueue *queue)
{
   _mulle__pointerqueue_removeall( (struct mulle__pointerqueue *) queue,
                                   queue->allocator);
}


static inline
void   mulle_pointerqueue_removeall( struct mulle_pointerqueue *queue)
{
   if( queue)
      _mulle_pointerqueue_removeall( queue);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline
void  _mulle_pointerqueue_push( struct mulle_pointerqueue *queue,
                                void *p)
{
   _mulle__pointerqueue_push( (struct mulle__pointerqueue *) queue,
                              p,
                              queue->allocator);
}


static inline
void   mulle_pointerqueue_push( struct mulle_pointerqueue *queue, void *p)
{
   if( queue)
      _mulle_pointerqueue_push( queue, p);
}


// aliases for orthogonality with mulle_pointerarray
MULLE_C_NONNULL_FIRST_SECOND
static inline
void  _mulle_pointerqueue_add( struct mulle_pointerqueue *queue,
                                void *p)
{
   _mulle__pointerqueue_push( (struct mulle__pointerqueue *) queue,
                              p,
                              queue->allocator);
}


static inline
void   mulle_pointerqueue_add( struct mulle_pointerqueue *queue, void *p)
{
   if( queue)
      _mulle_pointerqueue_push( queue, p);
}



MULLE_C_NONNULL_FIRST
static inline
void  *_mulle_pointerqueue_pop( struct mulle_pointerqueue *queue)
{
   return( _mulle__pointerqueue_pop( (struct mulle__pointerqueue *) queue,
                                     queue->allocator));
}


static inline
void   *mulle_pointerqueue_pop( struct mulle_pointerqueue *queue)
{
   return( queue ? _mulle_pointerqueue_pop( queue) : NULL);
}


MULLE_C_NONNULL_FIRST
static inline unsigned int
   _mulle_pointerqueue_get_count( struct mulle_pointerqueue *queue)
{
   return( _mulle__pointerqueue_get_count( (struct mulle__pointerqueue *) queue));
}


static inline unsigned int
   mulle_pointerqueue_get_count( struct mulle_pointerqueue *queue)
{
   return( queue ? _mulle_pointerqueue_get_count( queue) : 0);
}


MULLE_C_NONNULL_FIRST
static inline unsigned short
   _mulle_pointerqueue_get_bucketsize( struct mulle_pointerqueue *queue)
{
   return( _mulle__pointerqueue_get_bucketsize( (struct mulle__pointerqueue *) queue));
}


static inline unsigned int
   mulle_pointerqueue_get_bucketsize( struct mulle_pointerqueue *queue)
{
   return( queue ? _mulle_pointerqueue_get_bucketsize( queue) : 0);
}


MULLE_C_NONNULL_FIRST
static inline struct mulle_allocator *
   _mulle_pointerqueue_get_allocator( struct mulle_pointerqueue *queue)
{
   return( queue->allocator);
}


static inline struct mulle_allocator *
   mulle_pointerqueue_get_allocator( struct mulle_pointerqueue *queue)
{
   return( queue ? _mulle_pointerqueue_get_allocator( queue) : NULL);
}


/*
 * The enumerator interface is rarely useful, since you can NOT use
 * it to manipulate the queue.
 */
#define MULLE_POINTERQUEUEENUMERATOR_BASE  MULLE__POINTERQUEUEENUMERATOR_BASE

struct mulle_pointerqueueenumerator
{
   MULLE_POINTERQUEUEENUMERATOR_BASE;
};


static inline struct mulle_pointerqueueenumerator
   mulle_pointerqueue_enumerate( struct mulle_pointerqueue *queue)
{
   struct mulle_pointerqueueenumerator    rover;
   struct mulle__pointerqueueenumerator   tmp;

   tmp = mulle__pointerqueue_enumerate( (struct mulle__pointerqueue *) queue);
   memcpy( &rover, &tmp, sizeof( struct mulle_pointerqueueenumerator));
   return( rover);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline int
   _mulle_pointerqueueenumerator_next( struct mulle_pointerqueueenumerator *rover,
                                       void **item)
{
   return( _mulle__pointerqueueenumerator_next( (struct mulle__pointerqueueenumerator *) rover,
                                                item));
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle_pointerqueueenumerator_done( struct mulle_pointerqueueenumerator *rover)
{
}

static inline void   mulle_pointerqueueenumerator_done( struct mulle_pointerqueueenumerator *rover)
{
}


#define mulle_pointerqueue_for( queue, item)                                                       \
   for( struct mulle_pointerqueueenumerator rover__ ## item = mulle_pointerqueue_enumerate( queue); \
        _mulle_pointerqueueenumerator_next( &rover__ ## item, (void **) &item);)


#endif
