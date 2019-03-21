//
//  queue-simple.c
//  test-container
//
//  Created by Nat! on 05.11.15.
//  Copyright (c) 2015 Nat! - Mulle kybernetiK.
//  Copyright (c) 2015 Codeon GmbH.
//  All rights reserved.
//
#include <stdio.h>
#include <mulle-container/mulle-container.h>
#include <mulle-testallocator/mulle-testallocator.h>



static void  enumerate( struct _mulle_pointerqueue *queue)
{
   struct _mulle_pointerqueueenumerator   rover;
   char   *s;

   rover = _mulle_pointerqueue_enumerate( queue);
   while( s = _mulle_pointerqueueenumerator_next( &rover))
      printf( "%s\n", s);
   _mulle_pointerqueueenumerator_done(&rover);
}


static void  simple_queue( void)
{
   struct _mulle_pointerqueue   *queue;

   queue = _mulle_pointerqueue_create( 0, 0, NULL);
   enumerate( queue);
   _mulle_pointerqueue_push( queue, "1", NULL);
   enumerate( queue);
   _mulle_pointerqueue_push( queue, "2", NULL);
   enumerate( queue);
   _mulle_pointerqueue_push( queue, "3", NULL);
   enumerate( queue);
   _mulle_pointerqueue_push( queue, "4", NULL);
   enumerate( queue);
   _mulle_pointerqueue_push( queue, "5", NULL);
   enumerate( queue);
   _mulle_pointerqueue_pop( queue, NULL);
   enumerate( queue);
   _mulle_pointerqueue_pop( queue, NULL);
   enumerate( queue);
   _mulle_pointerqueue_pop( queue, NULL);
   enumerate( queue);
   _mulle_pointerqueue_pop( queue, NULL);
   enumerate( queue);
   _mulle_pointerqueue_pop( queue, NULL);
   enumerate( queue);
   _mulle_pointerqueue_pop( queue, NULL);
   enumerate( queue);
   _mulle_pointerqueue_destroy( queue, NULL);
}


int main(int argc, const char * argv[])
{
   simple_queue();

   return( 0);
}
