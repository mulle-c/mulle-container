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
#include "mulle--structqueue.h"

#include "include-private.h"


int   __mulle__structqueueenumerator_next( struct mulle__structqueueenumerator *rover,
                                          void **item);


struct mulle__structqueue   *
   mulle__structqueue_create( size_t sizeof_struct,
                              size_t alignof_struct,
                              unsigned short bucket_size,
                              unsigned short spare_allowance,
                              struct mulle_allocator *allocator)
{
   struct mulle__structqueue  *queue;

   queue = mulle__structqueue_alloc( allocator);
   _mulle__structqueue_init( queue,
                             sizeof_struct,
                             alignof_struct,
                             bucket_size,
                             spare_allowance);
   return( queue);
}


struct mulle__structqueuebucket *
   _mulle__structqueue_new_bucket( struct mulle__structqueue *queue,
                                   struct mulle_allocator *allocator)
{
   struct mulle__structqueuebucket   *p;
   size_t                             space;

   p = queue->_spares;
   if( p)
   {
      queue->_spares = p->_next;
      queue->_spare_allowance++;  // room again
   }
   else
   {
      space = (sizeof( struct mulle__structqueuebucket) -
               sizeof( struct mulle__structqueuebucket *)) +
               (size_t) queue->_bucket_size * (size_t) queue->_sizeof_struct;

      p = mulle_allocator_malloc( allocator, space);
   }
   // _storage may be garbage, but _next struct must be clear
   p->_next = NULL;
   return( p);
}


void
   mulle__structqueue_free_bucket( struct mulle__structqueue *queue,
                                   struct mulle__structqueuebucket  *q,
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


void   _mulle__structqueue_grow( struct mulle__structqueue *queue,
                                 struct mulle_allocator *allocator)
{
   struct mulle__structqueuebucket   *p;
   struct mulle__structqueuebucket   *q;

   p = _mulle__structqueue_new_bucket( queue, allocator);
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


void   _mulle__structqueue_shrink( struct mulle__structqueue *queue,
                                   struct mulle_allocator *allocator)
{
   struct mulle__structqueuebucket   *q;

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

   mulle__structqueue_free_bucket( queue, q, allocator);
}



void  *_mulle__structqueue_reserve( struct mulle__structqueue *queue,
                                    struct mulle_allocator *allocator)
{
   size_t   offset;

   if( queue->_write_index >= queue->_bucket_size)
      _mulle__structqueue_grow( queue, allocator);

   assert( queue->_write_index < queue->_bucket_size);

   queue->_count++;
   offset = queue->_write_index++ * queue->_sizeof_struct;
   return( &queue->_write->_storage[ offset]);
}


static void   free_chained_buckets( struct mulle__structqueuebucket *p,
                                    struct mulle_allocator *allocator)
{
   struct mulle__structqueuebucket   *q;

   for(; p; p = q)
   {
      q = p->_next;
      mulle_allocator_free( allocator, p);
   }
}


int   __mulle__structqueueenumerator_next( struct mulle__structqueueenumerator *rover,
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
   return( _mulle__structqueueenumerator_next( rover, item));
}


static inline int
  _mulle__structqueue_pop_noshrink( struct mulle__structqueue *queue,
                                    void *dst)
{
   size_t   offset;

   // if nothing to read, read struct points at write
   if( queue->_read == queue->_write)
      if( queue->_read_index == queue->_write_index)
         return( 0);

   assert( queue->_count);
   queue->_count--;

   //
   // now if "release" truely releases, p is pointing to a zombie.
   // not so, if release is autorelease or nop
   // check one case with an assert
   //
   assert( queue->_read_index < queue->_bucket_size);
   offset = queue->_read_index * queue->_sizeof_struct;
   if( dst)
   {
      memcpy( dst, &queue->_read->_storage[ offset], queue->_copy_sizeof_struct);
   }
   queue->_read_index++;

   return( 1);
}



int   _mulle__structqueue_pop( struct mulle__structqueue *queue,
                               struct mulle_allocator *allocator,
                               void *dst)
{
   if( queue->_read_index >= queue->_bucket_size)
      _mulle__structqueue_shrink( queue, allocator);

   return( _mulle__structqueue_pop_noshrink( queue, dst));
}


void   _mulle__structqueue_reset( struct mulle__structqueue *queue,
                                  struct mulle_allocator *allocator)
{
   struct mulle__structqueuebucket  *p;


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



void   _mulle__structqueue_done( struct mulle__structqueue *queue,
                                 struct mulle_allocator *allocator)
{
   free_chained_buckets( queue->_read, allocator);
   free_chained_buckets( queue->_spares, allocator);
#ifdef DEBUG
   mulle_memset_uint32( queue, 0xDEADDEAD,sizeof( struct mulle__structqueue));
#endif
}


void   _mulle__structqueue_destroy( struct mulle__structqueue *queue,
                                    struct mulle_allocator *allocator)
{
   _mulle__structqueue_done( queue, allocator);
   mulle__structqueue_free( queue, allocator);
}

