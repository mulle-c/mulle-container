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


static void  no_leak_empty_queue( void)
{
   struct _mulle_pointerqueue   *queue;

   queue = _mulle_pointerqueue_create( 0, 0, NULL);
   _mulle_pointerqueue_destroy( queue, NULL);
}


static void  no_leak_one_queue( void)
{
   struct _mulle_pointerqueue   *queue;

   queue = _mulle_pointerqueue_create( 0, 0, NULL);
   _mulle_pointerqueue_push( queue, "1", NULL);
   _mulle_pointerqueue_destroy( queue, NULL);
}



static void  no_leak_filled_queue( void)
{
   struct _mulle_pointerqueue   *queue;

   queue = _mulle_pointerqueue_create( 0, 0, NULL);
   _mulle_pointerqueue_push( queue, "1", NULL);
   _mulle_pointerqueue_push( queue, "2", NULL);
   _mulle_pointerqueue_push( queue, "3", NULL);
   _mulle_pointerqueue_push( queue, "4", NULL);
   _mulle_pointerqueue_push( queue, "5", NULL);
   _mulle_pointerqueue_destroy( queue, NULL);
}


static void  simple_queue( void)
{
   struct _mulle_pointerqueue   *queue;

   queue = _mulle_pointerqueue_create( 0, 0, NULL);
   printf( "%s\n", _mulle_pointerqueue_pop( queue, NULL) ? "???" : "NULL");
   _mulle_pointerqueue_push( queue, "1", NULL);
   _mulle_pointerqueue_push( queue, "2", NULL);
   _mulle_pointerqueue_push( queue, "3", NULL);
   _mulle_pointerqueue_push( queue, "4", NULL);
   _mulle_pointerqueue_push( queue, "5", NULL);
   printf( "%s\n", (char *) _mulle_pointerqueue_pop( queue, NULL));
   printf( "%s\n", (char *) _mulle_pointerqueue_pop( queue, NULL));
   printf( "%s\n", (char *) _mulle_pointerqueue_pop( queue, NULL));
   printf( "%s\n", (char *) _mulle_pointerqueue_pop( queue, NULL));
   printf( "%s\n", (char *) _mulle_pointerqueue_pop( queue, NULL));
   printf( "%s\n", _mulle_pointerqueue_pop( queue, NULL) ? "???" : "NULL");

   _mulle_pointerqueue_destroy( queue, NULL);
}




// the mulle_testallocator detects and aborts on leaks
static void  run_test( void (*f)( void))
{
   mulle_testallocator_reset();
   (f)();
   mulle_testallocator_reset();
}


int main(int argc, const char * argv[])
{
   run_test( no_leak_empty_queue);
   run_test( no_leak_one_queue);
   run_test( no_leak_filled_queue);
   run_test( simple_queue);

   return( 0);
}
