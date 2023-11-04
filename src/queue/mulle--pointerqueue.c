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
#include "mulle--pointerqueue.h"

#include "include-private.h"


int   __mulle__pointerqueueenumerator_next( struct mulle__pointerqueueenumerator *rover,
                                            void **item);



struct mulle__pointerqueuebucket *
   _mulle__pointerqueue_new_bucket( struct mulle__pointerqueue *queue,
                                    struct mulle_allocator *allocator)
{
   struct mulle__pointerqueuebucket   *p;
   size_t                             space;

   p = queue->_spares;
   if( p)
   {
      queue->_spares = p->_next;
      queue->_spare_allowance++;  // room again
   }
   else
   {
      space = (sizeof( struct mulle__pointerqueuebucket) -
               sizeof( struct mulle__pointerqueuebucket *)) +
               queue->_bucket_size * sizeof( void *);

      p = mulle_allocator_malloc( allocator, space);
   }
   // _storage may be garbage, but _next pointer must be clear
   p->_next = NULL;
   return( p);
}


void
   mulle__pointerqueue_free_bucket( struct mulle__pointerqueue *queue,
                                    struct mulle__pointerqueuebucket  *q,
                                    struct mulle_allocator *allocator)
{
   if( queue->_spare_allowance)
   {
      q->_next       = queue->_spares;
      queue->_spares = q;
      --queue->_spare_allowance;
      return;
   }

   mulle_allocator_free( allocator, q);
}


void   _mulle__pointerqueue_grow( struct mulle__pointerqueue *queue,
                                  struct mulle_allocator *allocator)
{
   struct mulle__pointerqueuebucket   *p;
   struct mulle__pointerqueuebucket   *q;

   p = _mulle__pointerqueue_new_bucket( queue, allocator);
   q = queue->_write;
   if( q)
      q->_next = p;

   queue->_write       = p;
   queue->_write_index = 0;

   // can happen initially
   if( ! queue->_read)
   {
      queue->_read       = queue->_write;
      queue->_read_index = 0;
   }
}


void   _mulle__pointerqueue_shrink( struct mulle__pointerqueue *queue,
                                    struct mulle_allocator *allocator)
{
   struct mulle__pointerqueuebucket   *q;

   q = queue->_read;
   if( ! q)
      return;

   // read should eventually read queue->_write
   if( ! q->_next)
   {
      assert( queue->_write == queue->_read);
      assert( queue->_write_index == queue->_read_index);
      assert( queue->_count == 0);

      // shrink completely to zero
      queue->_read        =
      queue->_write       = NULL;
      queue->_read_index  =
      queue->_write_index = queue->_bucket_size;
   }
   else
   {
      queue->_read       = q->_next;
      queue->_read_index = 0;
   }

   mulle__pointerqueue_free_bucket( queue, q, allocator);
}


static void   free_chained_buckets( struct mulle__pointerqueuebucket *p,
                                    struct mulle_allocator *allocator)
{
   struct mulle__pointerqueuebucket   *q;

   for(; p; p = q)
   {
      q = p->_next;
      mulle_allocator_free( allocator, p);
   }
}


int   __mulle__pointerqueueenumerator_next( struct mulle__pointerqueueenumerator *rover,
                                            void **item)
{
   if( rover->_index != rover->_queue->_bucket_size)
   {
      *item = NULL;
      return( 0);
   }
   if( ! rover->_curr)
   {
      *item = NULL;
      return( 0);
   }

   rover->_curr = rover->_curr->_next;
   if( ! rover->_curr)
   {
      *item = NULL;
      return( 0);
   }

   rover->_index = 0;
   return( _mulle__pointerqueueenumerator_next( rover, item));
}


struct mulle__pointerqueue   *
   mulle__pointerqueue_create( unsigned short bucket_size,
                               unsigned short spare_allowance,
                               struct mulle_allocator *allocator)
{
   struct mulle__pointerqueue  *queue;

   queue = mulle__pointerqueue_alloc( allocator);
   _mulle__pointerqueue_init( queue, bucket_size, spare_allowance);
   return( queue);
}


static inline void  *
  _mulle__pointerqueue_pop_noshrink( struct mulle__pointerqueue *queue)
{
   void   *p;

   // if nothing to read, read pointer points at write
   if( queue->_read == queue->_write)
      if( queue->_read_index == queue->_write_index)
         return( NULL);

   assert( queue->_count);
   queue->_count--;
   //
   // now if "release" truely releases, p is pointing to a zombie.
   // not so, if release is autorelease or nop
   // check one case with an assert
   //
   assert( queue->_read_index < queue->_bucket_size);
   p = queue->_read->_storage[ queue->_read_index++];
   return( p);
}



void  *_mulle__pointerqueue_pop( struct mulle__pointerqueue *queue,
                                 struct mulle_allocator *allocator)
{
   if( queue->_read_index >= queue->_bucket_size)
      _mulle__pointerqueue_shrink( queue, allocator);

   return( _mulle__pointerqueue_pop_noshrink( queue));
}


void   _mulle__pointerqueue_reset( struct mulle__pointerqueue *queue,
                                   struct mulle_allocator *allocator)
{ 
   struct mulle__pointerqueuebucket  *p;

   // transfer buffer over to spares if possible
   while( queue->_spare_allowance)
   {
      p = queue->_read;
      if( ! p)
         break;

      queue->_read   = p->_next;
      p->_next       = queue->_spares;
      queue->_spares = p;

      --queue->_spare_allowance;
   }

   free_chained_buckets( queue->_read, allocator);

   queue->_read        =
   queue->_write       = NULL;
   queue->_read_index  =
   queue->_write_index = queue->_bucket_size;

   queue->_count       = 0;
}



void   _mulle__pointerqueue_done( struct mulle__pointerqueue *queue,
                                  struct mulle_allocator *allocator)
{
   free_chained_buckets( queue->_read, allocator);
   free_chained_buckets( queue->_spares, allocator);
#ifdef DEBUG
   memset( queue, 0xFD, sizeof( struct mulle__pointerqueue));
#endif
}


void   _mulle__pointerqueue_destroy( struct mulle__pointerqueue *queue,
                                     struct mulle_allocator *allocator)
{
   _mulle__pointerqueue_done( queue, allocator);
   mulle__pointerqueue_free( queue, allocator);
}



