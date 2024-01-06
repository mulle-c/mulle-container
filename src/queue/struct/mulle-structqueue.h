//
//  mulle-structqueue.h
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
#ifndef mulle_structqueue_h__
#define mulle_structqueue_h__

#include "mulle--structqueue.h"

//
// A struct mulle_structqueue is a FIFO, you can't use it as a stack.
// As a queue it has a pop operation. You can not store NULL into it. It can
// be faster than a mulle-structarray, since it doesn't realloc/memcpy.
// Main use is for an ever growing collection of things.
//
// MEMO: It is not useful to put memory management into the queue, as the queue
// would need to release on pop, which would be disastrous if the release does
// a free (copied C-string for example). Or pop would have to return retained.
//
struct mulle_structqueue
{
   MULLE__STRUCTQUEUE_BASE;
   struct mulle_allocator   *allocator;   // public...
};


#define MULLE_STRUCTQUEUE_INIT( type, xallocator)                     \
   ((struct mulle_structqueue)                                        \
   {                                                                  \
      ._read_index         = 64,                                      \
      ._write_index        = 64,                                      \
      ._sizeof_struct      = MULLE__STRUCTQUEUE_ALIGNED_SIZE( type),  \
      ._copy_sizeof_struct = sizeof( type),                           \
      ._bucket_size        = 64,                                      \
      .allocator           = (xallocator)                             \
   })

// does not set the allocator, init does
static inline struct mulle_structqueue *
  mulle_structqueue_alloc( struct mulle_allocator *allocator)
{
   struct mulle_structqueue   *queue;

   queue = mulle_allocator_malloc( allocator, sizeof( struct mulle_structqueue));
   return( queue);
}


static inline void
  mulle_structqueue_free( struct mulle_structqueue *queue)
{
   if( queue)
      mulle_allocator_free( queue->allocator, queue);
}


// large bucket_size is convenient for mostly add
// spare_allowance can be to cache freed buckets, useful for many edit
// operations
MULLE_C_NONNULL_FIRST
static inline void  _mulle_structqueue_init( struct mulle_structqueue *queue,
                                             size_t sizeof_struct,
                                             unsigned int alignof_struct,
                                             unsigned short bucket_size,
                                             unsigned short spare_allowance,
                                             struct mulle_allocator *allocator)
{
   _mulle__structqueue_init( (struct mulle__structqueue *) queue,
                              sizeof_struct,
                              alignof_struct,
                              bucket_size,
                              spare_allowance);
   queue->allocator = allocator;
}


static inline void  mulle_structqueue_init( struct mulle_structqueue *queue,
                                            size_t sizeof_struct,
                                            unsigned int alignof_struct,
                                            unsigned short bucket_size,
                                            unsigned short spare_allowance,
                                            struct mulle_allocator *allocator)
{
   if( queue)
      _mulle_structqueue_init( queue,
                               sizeof_struct,
                               alignof_struct,
                               bucket_size,
                               spare_allowance,
                               allocator);
}


static inline void
   mulle_structqueue_init_default( struct mulle_structqueue *queue,
                                   size_t sizeof_struct,
                                   unsigned int alignof_struct,
                                   struct mulle_allocator *allocator)
{
   if( queue)
      _mulle_structqueue_init( queue,
                               sizeof_struct,
                               alignof_struct,
                               256,
                               16,
                               allocator);
}



MULLE__CONTAINER_GLOBAL
struct mulle_structqueue *
   mulle_structqueue_create( size_t sizeof_struct,
                             unsigned int alignof_struct,
                             unsigned short bucket_size,
                             unsigned short spare_allowance,
                             struct mulle_allocator *allocator);


static inline struct mulle_structqueue *
   mulle_structqueue_create_default( size_t sizeof_struct,
                                     unsigned int alignof_struct,
                                     struct mulle_allocator *allocator)
{
   return( mulle_structqueue_create( sizeof_struct,
                                     alignof_struct,
                                     256,
                                     16,
                                     allocator));
}

MULLE_C_NONNULL_FIRST
static inline
void   _mulle_structqueue_destroy( struct mulle_structqueue *queue)
{
   _mulle__structqueue_destroy( (struct mulle__structqueue *) queue,
                                 queue->allocator);
}

static inline
void   mulle_structqueue_destroy( struct mulle_structqueue *queue)
{
   if( queue)
      _mulle_structqueue_destroy( queue);
}


MULLE_C_NONNULL_FIRST
static inline
void   _mulle_structqueue_done( struct mulle_structqueue *queue)
{
   _mulle__structqueue_done( (struct mulle__structqueue *) queue,
                              queue->allocator);
}


static inline
void   mulle_structqueue_done( struct mulle_structqueue *queue)
{
   if( queue)
      _mulle_structqueue_done( queue);
}



MULLE_C_NONNULL_FIRST
static inline
void   _mulle_structqueue_grow( struct mulle_structqueue *queue)
{
   _mulle__structqueue_grow( (struct mulle__structqueue *) queue,
                              queue->allocator);
}



static inline
void   mulle_structqueue_grow( struct mulle_structqueue *queue)
{
   if( queue)
      _mulle_structqueue_grow( queue);
}


MULLE_C_NONNULL_FIRST
static inline
void   _mulle_structqueue_shrink( struct mulle_structqueue *queue)
{
   _mulle__structqueue_shrink( (struct mulle__structqueue *) queue,
                                queue->allocator);
}


static inline
void   mulle_structqueue_shrink( struct mulle_structqueue *queue)
{
   if( queue)
      _mulle_structqueue_shrink( queue);
}


MULLE_C_NONNULL_FIRST
static inline
void   _mulle_structqueue_reset( struct mulle_structqueue *queue)
{
   _mulle__structqueue_reset( (struct mulle__structqueue *) queue,
                               queue->allocator);
}


static inline
void   mulle_structqueue_reset( struct mulle_structqueue *queue)
{
   if( queue)
      _mulle_structqueue_reset( queue);
}


MULLE_C_NONNULL_FIRST
static inline
void   _mulle_structqueue_removeall( struct mulle_structqueue *queue)
{
   _mulle__structqueue_removeall( (struct mulle__structqueue *) queue,
                                   queue->allocator);
}


static inline
void   mulle_structqueue_removeall( struct mulle_structqueue *queue)
{
   if( queue)
      _mulle_structqueue_removeall( queue);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline
void  _mulle_structqueue_push( struct mulle_structqueue *queue,
                                void *p)
{
   _mulle__structqueue_push( (struct mulle__structqueue *) queue,
                              p,
                              queue->allocator);
}


static inline void
   *_mulle_structqueue_reserve( struct mulle_structqueue *queue)
{
   return( _mulle__structqueue_reserve( (struct mulle__structqueue *) queue,
                                        queue->allocator));
}


static inline void
   *mulle_structqueue_reserve( struct mulle_structqueue *queue)
{
   if( ! queue)
      return( NULL);
   return( _mulle__structqueue_reserve( (struct mulle__structqueue *) queue,
                                        queue->allocator));
}


static inline
void   mulle_structqueue_push( struct mulle_structqueue *queue, void *p)
{
   if( queue)
      _mulle_structqueue_push( queue, p);
}


// aliases for orthogonality with mulle_structarray
MULLE_C_NONNULL_FIRST_SECOND
static inline
void  _mulle_structqueue_add( struct mulle_structqueue *queue,
                                void *p)
{
   _mulle__structqueue_push( (struct mulle__structqueue *) queue,
                              p,
                              queue->allocator);
}


static inline
void   mulle_structqueue_add( struct mulle_structqueue *queue, void *p)
{
   if( queue)
      _mulle_structqueue_push( queue, p);
}



// returns 1 on success, copies struct into 'p' (which must be large
// enough) if p is != NULL
MULLE_C_NONNULL_FIRST
static inline
int  _mulle_structqueue_pop( struct mulle_structqueue *queue, void *p)
{
   return( _mulle__structqueue_pop( (struct mulle__structqueue *) queue,
                                     queue->allocator,
                                     p));
}


static inline
int   mulle_structqueue_pop( struct mulle_structqueue *queue, void *p)
{
   return( queue ? _mulle_structqueue_pop( queue, p) : 0);
}


MULLE_C_NONNULL_FIRST
static inline unsigned int
   _mulle_structqueue_get_count( struct mulle_structqueue *queue)
{
   return( _mulle__structqueue_get_count( (struct mulle__structqueue *) queue));
}


static inline unsigned int
   mulle_structqueue_get_count( struct mulle_structqueue *queue)
{
   return( queue ? _mulle_structqueue_get_count( queue) : 0);
}


MULLE_C_NONNULL_FIRST
static inline unsigned short
   _mulle_structqueue_get_bucketsize( struct mulle_structqueue *queue)
{
   return( _mulle__structqueue_get_bucketsize( (struct mulle__structqueue *) queue));
}


static inline unsigned int
   mulle_structqueue_get_bucketsize( struct mulle_structqueue *queue)
{
   return( queue ? _mulle_structqueue_get_bucketsize( queue) : 0);
}


MULLE_C_NONNULL_FIRST
static inline unsigned int
   _mulle_structqueue_get_element_size( struct mulle_structqueue *queue)
{
   return( _mulle__structqueue_get_element_size( (struct mulle__structqueue *) queue));
}

static inline unsigned int
   mulle_structqueue_get_element_size( struct mulle_structqueue *queue)
{
   return( queue ? _mulle__structqueue_get_element_size( (struct mulle__structqueue *) queue) : 0);
}


MULLE_C_NONNULL_FIRST
static inline struct mulle_allocator *
   _mulle_structqueue_get_allocator( struct mulle_structqueue *queue)
{
   return( queue->allocator);
}


static inline struct mulle_allocator *
   mulle_structqueue_get_allocator( struct mulle_structqueue *queue)
{
   return( queue ? _mulle_structqueue_get_allocator( queue) : NULL);
}


/*
 * The enumerator interface is rarely useful, since you can NOT use
 * it to manipulate the queue.
 */
#define MULLE_STRUCTQUEUEENUMERATOR_BASE  MULLE__STRUCTQUEUEENUMERATOR_BASE

struct mulle_structqueueenumerator
{
   MULLE_STRUCTQUEUEENUMERATOR_BASE;
};


static inline struct mulle_structqueueenumerator
   mulle_structqueue_enumerate( struct mulle_structqueue *queue)
{
   struct mulle_structqueueenumerator    rover;
   struct mulle__structqueueenumerator   tmp;

   tmp = mulle__structqueue_enumerate( (struct mulle__structqueue *) queue);
   memcpy( &rover, &tmp, sizeof( struct mulle_structqueueenumerator));
   return( rover);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline int
   _mulle_structqueueenumerator_next( struct mulle_structqueueenumerator *rover,
                                       void **item)
{
   return( _mulle__structqueueenumerator_next( (struct mulle__structqueueenumerator *) rover,
                                                item));
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle_structqueueenumerator_done( struct mulle_structqueueenumerator *rover)
{
}

static inline void   mulle_structqueueenumerator_done( struct mulle_structqueueenumerator *rover)
{
}


MULLE_C_NONNULL_FIRST_SECOND
static inline void
   _mulle_structqueue_assert_pointer( struct mulle_structqueue *queue, void *p)
{
   _mulle__structqueue_assert_pointer( (struct mulle__structqueue *) queue, p);
}


#define mulle_structqueue_do( name, type)                              \
   for( struct mulle_structqueue                                       \
           name ## __container = MULLE_STRUCTQUEUE_INIT( type, NULL),  \
           *name = &name ## __container,                               \
           *name ## __i = NULL;                                        \
        ! name ## __i;                                                 \
        name ## __i =                                                  \
        (                                                              \
           _mulle_structqueue_done( &name ## __container),             \
           (void *) 0x1                                                \
        )                                                              \
      )                                                                \
      for( int  name ## __j = 0;    /* break protection */             \
           name ## __j < 1;                                            \
           name ## __j++)


// created by make-container-for.sh src/queue/struct/mulle-structqueue.c

#define mulle_structqueue_for( name, item)                                               \
   assert( sizeof( item) == sizeof( void *));                                            \
   for( struct mulle_structqueueenumerator                                               \
           rover__ ## item = mulle_structqueue_enumerate( name),                         \
           *rover___  ## item ## __i = (void *) 0;                                       \
        ! rover___  ## item ## __i;                                                      \
        rover___ ## item ## __i = (_mulle_structqueueenumerator_done( &rover__ ## item), \
                                   (void *) 1))                                          \
      while( _mulle_structqueueenumerator_next( &rover__ ## item, (void **) &item))

#endif
