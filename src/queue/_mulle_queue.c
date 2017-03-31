/*
 *  MulleFoundation - A tiny Foundation replacement
 *
 *  struct _mulle_queue.c is a part of MulleFoundation
 *
 *  Copyright (C) 2011 Nat!, Mulle kybernetiK.
 *  All rights reserved.
 *
 *  Coded by Nat!
 *
 *  $Id$
 *
 */
#include "_mulle_queue.h"

#include <mulle_allocator/mulle_allocator.h>


void  *__mulle_queueenumerator_next( struct _mulle_queue *queue, struct _mulle_queueenumerator *rover);


void   _mulle_queue_grow( struct _mulle_queue *queue, struct mulle_allocator *allocator)
{
   struct _mulle_queuebucket   *p;
   struct _mulle_queuebucket   *q;
   size_t                      space;

   p = queue->_spares;
   if( p)
   {
      queue->_spares = p->_next;
      queue->_spare_allowance++;  // room again
   }
   else
   {
      space = (sizeof( struct _mulle_queuebucket) - sizeof( struct _mulle_queuebucket *)) +
                            queue->_bucket_size * sizeof( struct _mulle_queuebucket *);

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


void   _mulle_queue_shrink( struct _mulle_queue *queue, struct mulle_allocator *allocator)
{
   struct _mulle_queuebucket   *q;

   q = queue->_read;

   queue->_read       = q->_next;
   queue->_read_index = queue->_read ? 0 : queue->_write_index;

   if( queue->_spare_allowance)
   {
      q->_next       = queue->_spares;
      queue->_spares = q;
      --queue->_spare_allowance;
   }
   else
   {
      mulle_allocator_free( allocator, q);
   }
}


static void   free_chained_buckets( struct _mulle_queuebucket *p,
                                    struct mulle_allocator *allocator)
{
   struct _mulle_queuebucket   *q;

   for(; p; p = q)
   {
      q = p->_next;
      mulle_allocator_free( allocator, p);
   }
}


void   _mulle_queue_done( struct _mulle_queue *queue, struct mulle_allocator *allocator)
{
   free_chained_buckets( queue->_spares, allocator);
   free_chained_buckets( queue->_read, allocator);
}


void   _mulle_queue_remove_all( struct _mulle_queue *queue,
                               struct mulle_allocator *allocator)
{
   struct _mulle_queuebucket  *p;

   // transfer buffer over to spares if possible

   while( queue->_spare_allowance)
   {
      p = queue->_read;
      if( ! p)
         break;
      queue->_read = p->_next;

      p->_next       = queue->_spares;
      queue->_spares = p;

      --queue->_spare_allowance;
   }

   free_chained_buckets( queue->_read, allocator);

   queue->_read        =
   queue->_write       = NULL;
   queue->_read_index  =
   queue->_write_index = queue->_bucket_size;

   queue->_count      = 0;
}


void  *__mulle_queueenumerator_next( struct _mulle_queue *queue, struct _mulle_queueenumerator *rover)
{
   if( rover->_index != queue->_bucket_size)
      return( NULL);
   if( ! rover->_curr)
      return( NULL);

   rover->_curr = rover->_curr->_next;
   if( ! rover->_curr)
      return( NULL);

   rover->_index = 0;
   return( _mulle_queueenumerator_next( queue, rover));
}


struct _mulle_queue   *_mulle_queue_create( unsigned short bucket_size,
                                          unsigned short spare_allowance,
                                          struct mulle_allocator *allocator)
{
   struct _mulle_queue  *queue;

   queue = mulle_allocator_malloc( allocator, sizeof( struct _mulle_queue));
   _mulle_queue_init( queue, bucket_size, spare_allowance);
   return( queue);
}



void  *_mulle_queue_pop( struct _mulle_queue *queue,
                        struct mulle_container_keycallback *callback,
                        struct mulle_allocator *allocator)
{
   void   *p;

   if( queue->_read_index >= queue->_bucket_size)
      _mulle_queue_shrink( queue, allocator);

   // if nothing to read, read pointer points at write
   if( queue->_read == queue->_write)
      if( queue->_read_index == queue->_write_index)
         return( NULL);

   queue->_count--;

   // now if "release" truely release, p is pointing to a zombie
   // not so, if release is autorelease or nop
   // check one case with an assert
   assert( callback->release != mulle_container_keycallback_pointer_free);

   p = queue->_read->_storage[ queue->_read_index++];
   (*callback->release)( callback, p, allocator);
   return( p);
}

void   _mulle_queue_destroy( struct _mulle_queue *queue,
                             struct mulle_allocator *allocator)
{
   _mulle_queue_done( queue, allocator);
   mulle_allocator_free( allocator, queue);
}


void   _mulle_queue_reset( struct _mulle_queue *queue,
                           struct mulle_allocator *allocator)
{
   _mulle_queue_done( queue, allocator);
   _mulle_queue_init( queue, queue->_bucket_size, queue->_spare_allowance);
}
