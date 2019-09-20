/*
 *  MulleFoundation - A tiny Foundation replacement
 *
 *  struct _mulle_pointerqueue.c is a part of MulleFoundation
 *
 *  Copyright (C) 2011 Nat!, Mulle kybernetiK.
 *  All rights reserved.
 *
 *  Coded by Nat!
 *
 *  $Id$
 *
 */
#include "_mulle-pointerqueue.h"

#include "include-private.h"


void  *__mulle_pointerqueueenumerator_next( struct _mulle_pointerqueueenumerator *rover);



struct _mulle_pointerqueuebucket *
   _mulle_pointerqueue_new_bucket( struct _mulle_pointerqueue *queue,
                                   struct mulle_allocator *allocator)
{
   struct _mulle_pointerqueuebucket   *p;
   size_t                             space;

   p = queue->_spares;
   if( p)
   {
      queue->_spares = p->_next;
      queue->_spare_allowance++;  // room again
   }
   else
   {
      space = (sizeof( struct _mulle_pointerqueuebucket) -
               sizeof( struct _mulle_pointerqueuebucket *)) +
               queue->_bucket_size * sizeof( struct _mulle_pointerqueuebucket *);

      p = mulle_allocator_malloc( allocator, space);
   }
   // storage may be garbage, but _next pointer must be clear
   p->_next = NULL;
   return( p);
}


void
   _mulle_pointerqueue_free_bucket( struct _mulle_pointerqueue *queue,
                                    struct _mulle_pointerqueuebucket  *q,
                                    struct mulle_allocator *allocator)
{
   struct _mulle_pointerqueuebucket   *p;
   size_t                             space;

   if( queue->_spare_allowance)
   {
      q->_next       = queue->_spares;
      queue->_spares = q;
      --queue->_spare_allowance;
      return;
   }

   mulle_allocator_free( allocator, q);
}


void   _mulle_pointerqueue_grow( struct _mulle_pointerqueue *queue,
                                 struct mulle_allocator *allocator)
{
   struct _mulle_pointerqueuebucket   *p;
   struct _mulle_pointerqueuebucket   *q;
   size_t                             space;

   p = _mulle_pointerqueue_new_bucket( queue, allocator);
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


void   _mulle_pointerqueue_shrink( struct _mulle_pointerqueue *queue,
                                   struct mulle_allocator *allocator)
{
   struct _mulle_pointerqueuebucket   *q;

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

   _mulle_pointerqueue_free_bucket( queue, q, allocator);
}


static void   free_chained_buckets( struct _mulle_pointerqueuebucket *p,
                                    struct mulle_allocator *allocator)
{
   struct _mulle_pointerqueuebucket   *q;

   for(; p; p = q)
   {
      q = p->_next;
      mulle_allocator_free( allocator, p);
   }
}


void   *__mulle_pointerqueueenumerator_next( struct _mulle_pointerqueueenumerator *rover)
{
   if( rover->_index != rover->_queue->_bucket_size)
      return( NULL);

   if( ! rover->_curr)
      return( NULL);

   rover->_curr = rover->_curr->_next;
   if( ! rover->_curr)
      return( NULL);

   rover->_index = 0;
   return( _mulle_pointerqueueenumerator_next( rover));
}


struct _mulle_pointerqueue   *
   _mulle_pointerqueue_create( unsigned short bucket_size,
                               unsigned short spare_allowance,
                               struct mulle_allocator *allocator)
{
   struct _mulle_pointerqueue  *queue;

   queue = _mulle_pointerqueue_alloc( allocator);
   _mulle_pointerqueue_init( queue, bucket_size, spare_allowance);
   return( queue);
}


static inline void  *
  _mulle_pointerqueue_pop_noshrink( struct _mulle_pointerqueue *queue)
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



void  *_mulle_pointerqueue_pop( struct _mulle_pointerqueue *queue,
                                struct mulle_allocator *allocator)
{
   if( queue->_read_index >= queue->_bucket_size)
      _mulle_pointerqueue_shrink( queue, allocator);

   return( _mulle_pointerqueue_pop_noshrink( queue));
}


void   _mulle_pointerqueue_reset( struct _mulle_pointerqueue *queue,
                                  struct mulle_allocator *allocator)
{
   struct _mulle_pointerqueuebucket  *p;


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



void   _mulle_pointerqueue_done( struct _mulle_pointerqueue *queue,
                          struct mulle_allocator *allocator)
{
   free_chained_buckets( queue->_read, allocator);
   free_chained_buckets( queue->_spares, allocator);
}


void   _mulle_pointerqueue_destroy( struct _mulle_pointerqueue *queue,
                                    struct mulle_allocator *allocator)
{
   _mulle_pointerqueue_done( queue, allocator);
   _mulle_pointerqueue_free( queue, allocator);
}



