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



static void  enumerate( struct mulle__pointerqueue *queue)
{
   struct mulle__pointerqueueenumerator   rover;
   char   *s;

   rover = mulle__pointerqueue_enumerate( queue);
   while( _mulle__pointerqueueenumerator_next( &rover, (void **) &s))
      printf( "%s\n", s);
   mulle__pointerqueueenumerator_done(&rover);
}


static void  simple_queue( void)
{
   struct mulle__pointerqueue   *queue;

   queue = mulle__pointerqueue_create( 0, 0, NULL);
   enumerate( queue);
   _mulle__pointerqueue_push( queue, "1", NULL);
   enumerate( queue);
   _mulle__pointerqueue_push( queue, "2", NULL);
   enumerate( queue);
   _mulle__pointerqueue_push( queue, "3", NULL);
   enumerate( queue);
   _mulle__pointerqueue_push( queue, "4", NULL);
   enumerate( queue);
   _mulle__pointerqueue_push( queue, "5", NULL);
   enumerate( queue);
   _mulle__pointerqueue_pop( queue, NULL);
   enumerate( queue);
   _mulle__pointerqueue_pop( queue, NULL);
   enumerate( queue);
   _mulle__pointerqueue_pop( queue, NULL);
   enumerate( queue);
   _mulle__pointerqueue_pop( queue, NULL);
   enumerate( queue);
   _mulle__pointerqueue_pop( queue, NULL);
   enumerate( queue);
   _mulle__pointerqueue_pop( queue, NULL);
   enumerate( queue);
   _mulle__pointerqueue_destroy( queue, NULL);
}


int main(int argc, const char * argv[])
{
   simple_queue();

   return( 0);
}
