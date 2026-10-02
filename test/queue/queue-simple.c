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


static void  no_leak_empty_queue( void)
{
   struct mulle__pointerqueue   *queue;

   queue = mulle__pointerqueue_create( 0, 0, NULL);
   _mulle__pointerqueue_destroy( queue, NULL);
}


static void  no_leak_one_queue( void)
{
   struct mulle__pointerqueue   *queue;

   queue = mulle__pointerqueue_create( 0, 0, NULL);
   _mulle__pointerqueue_push( queue, "1", NULL);
   _mulle__pointerqueue_destroy( queue, NULL);
}



static void  no_leak_filled_queue( void)
{
   struct mulle__pointerqueue   *queue;

   queue = mulle__pointerqueue_create( 0, 0, NULL);
   _mulle__pointerqueue_push( queue, "1", NULL);
   _mulle__pointerqueue_push( queue, "2", NULL);
   _mulle__pointerqueue_push( queue, "3", NULL);
   _mulle__pointerqueue_push( queue, "4", NULL);
   _mulle__pointerqueue_push( queue, "5", NULL);
   _mulle__pointerqueue_destroy( queue, NULL);
}


static void  simple_queue( void)
{
   mulle__pointerqueue_do( queue)
   {
      printf( "%s\n", _mulle__pointerqueue_pop( queue, NULL) ? "???" : "NULL");
      _mulle__pointerqueue_push( queue, "1", NULL);
      _mulle__pointerqueue_push( queue, "2", NULL);
      _mulle__pointerqueue_push( queue, "3", NULL);
      _mulle__pointerqueue_push( queue, "4", NULL);
      _mulle__pointerqueue_push( queue, "5", NULL);
      printf( "%s\n", (char *) _mulle__pointerqueue_pop( queue, NULL));
      printf( "%s\n", (char *) _mulle__pointerqueue_pop( queue, NULL));
      printf( "%s\n", (char *) _mulle__pointerqueue_pop( queue, NULL));
      printf( "%s\n", (char *) _mulle__pointerqueue_pop( queue, NULL));
      printf( "%s\n", (char *) _mulle__pointerqueue_pop( queue, NULL));
      printf( "%s\n", _mulle__pointerqueue_pop( queue, NULL) ? "???" : "NULL");
   }
}




// the mulle_allocator_default detects and aborts on leaks
static void  run_test( void (*f)( void))
{
   (f)();
}


int main(int argc, const char * argv[])
{
   run_test( no_leak_empty_queue);
   run_test( no_leak_one_queue);
   run_test( no_leak_filled_queue);
   run_test( simple_queue);

   return( 0);
}
