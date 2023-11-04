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
   struct mulle_pointerqueue   *queue;

   queue = mulle_pointerqueue_create( 0, 0, NULL);
   _mulle_pointerqueue_destroy( queue);
}


static void  no_leak_one_queue( void)
{
   struct mulle_pointerqueue   *queue;

   queue = mulle_pointerqueue_create( 0, 0, NULL);
   mulle_pointerqueue_push( queue, "1");
   mulle_pointerqueue_destroy( queue);
}



static void  no_leak_filled_queue( void)
{
   struct mulle_pointerqueue   *queue;

   queue = mulle_pointerqueue_create( 0, 0, NULL);
   mulle_pointerqueue_push( queue, "1");
   mulle_pointerqueue_push( queue, "2");
   mulle_pointerqueue_push( queue, "3");
   mulle_pointerqueue_push( queue, "4");
   mulle_pointerqueue_push( queue, "5");
   mulle_pointerqueue_destroy( queue);
}


static void  reset( void)
{
   struct mulle_pointerqueue   *queue;

   // use spare allowance for coverage
   queue = mulle_pointerqueue_create( 32, 8, NULL);
   mulle_pointerqueue_push( queue, "1");
   mulle_pointerqueue_push( queue, "2");

   mulle_pointerqueue_reset( queue);

   mulle_pointerqueue_push( queue, "3");
   mulle_pointerqueue_push( queue, "4");
   mulle_pointerqueue_push( queue, "5");
   printf( "%s\n", (char *) mulle_pointerqueue_pop( queue));
   printf( "%s\n", (char *) mulle_pointerqueue_pop( queue));
   printf( "%s\n", (char *) mulle_pointerqueue_pop( queue));
   mulle_pointerqueue_destroy( queue);
}


static void  simple_queue( void)
{
   mulle_pointerqueue_do( queue)
   {
      printf( "%s\n", _mulle_pointerqueue_pop( queue) ? "???" : "NULL");
      mulle_pointerqueue_push( queue, "1");
      mulle_pointerqueue_push( queue, "2");
      mulle_pointerqueue_push( queue, "3");
      mulle_pointerqueue_push( queue, "4");
      mulle_pointerqueue_push( queue, "5");
      printf( "%s\n", (char *) mulle_pointerqueue_pop( queue));
      printf( "%s\n", (char *) mulle_pointerqueue_pop( queue));
      printf( "%s\n", (char *) mulle_pointerqueue_pop( queue));
      printf( "%s\n", (char *) mulle_pointerqueue_pop( queue));
      printf( "%s\n", (char *) mulle_pointerqueue_pop( queue));
      printf( "%s\n", mulle_pointerqueue_pop( queue) ? "???" : "NULL");
   }
}


static void  large_queue( void)
{
   int    i;
   void   *p;

   mulle_pointerqueue_do( queue)
   {
      for( i = 0; i < 1000; i++)
         mulle_pointerqueue_push( queue, mulle_int_as_pointer( i + 1));

      i = 0;
      mulle_pointerqueue_for( queue, p)
      {
         assert( p == mulle_int_as_pointer( i + 1));
         i++;
      }
      assert( i == 1000);

      for( i = 0; i < 1000; i++)
      {
         p = mulle_pointerqueue_pop( queue);
         assert( p == mulle_int_as_pointer( i + 1));
      }
      p = mulle_pointerqueue_pop( queue);
      assert( p == NULL);
   }
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
   run_test( reset);
   run_test( simple_queue);
   run_test( large_queue);

   return( 0);
}
