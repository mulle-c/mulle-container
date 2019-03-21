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


void   _mulle_pointerqueue_grow( struct _mulle_pointerqueue *queue,
                                 struct mulle_allocator *allocator)
{
   struct _mulle_pointerqueuebucket   *p;
   struct _mulle_pointerqueuebucket   *q;
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

      p = mulle_allocator_calloc( allocator, 1, space);
   }

   q = queue->_write;
   if( q)
      q->_next = p;

   p->_next = NULL;

   queue->_write       = p;
   queue->_write_index = 0;
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

   queue->_read       = q->_next;
   queue->_read_index = queue->_read ? 0 : queue->_write_index;

   if( queue->_spare_allowance)
   {
      q->_next       = queue->_spares;
      queue->_spares = q;
      --queue->_spare_allowance;
      return;
   }

   mulle_allocator_free( allocator, q);
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


void  *_mulle_pointerqueue_pop( struct _mulle_pointerqueue *queue,
                                struct mulle_allocator *allocator)
{
   void   *p;

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



