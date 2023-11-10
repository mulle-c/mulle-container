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
   struct mulle_structqueue   *queue;

   queue = mulle_structqueue_create( sizeof( char), alignof( char), 0, 0, NULL);
   _mulle_structqueue_destroy( queue);
}


static void  no_leak_one_queue( void)
{
   struct mulle_structqueue   *queue;
   char                       c;

   queue = mulle_structqueue_create( sizeof( char), alignof( char), 0, 0, NULL);
   _mulle_structqueue_push( queue, (c = '1', &c));
   _mulle_structqueue_destroy( queue);
}



static void  no_leak_filled_queue( void)
{
   struct mulle_structqueue   *queue;
   char                       c;
   int                        i;

   queue = mulle_structqueue_create( sizeof( char), alignof( char), 0, 0, NULL);
   for( i = 0; i < 1000; i++)
      _mulle_structqueue_push( queue, ((c = i % 255), &c));

   assert( mulle_structqueue_get_count( queue) == 1000);
   _mulle_structqueue_reset( queue);
   assert( mulle_structqueue_get_count( queue) == 0);
   _mulle_structqueue_destroy( queue);
}


static void  simple_queue( void)
{
   char   c;
   int    i;

   mulle_structqueue_do( queue, char *)
   {
      mulle_structqueue_push( queue, (c = '1', &c));
      for( i = 0; i < 5; i++)
         _mulle_structqueue_push( queue, (c = '2' + i, &c));

      while( mulle_structqueue_pop( queue, &c))
         printf( "%c\n", c);
   }
}



static void  misc_coverage( void)
{
   struct mulle__structqueue    *queue;
   char                         c;
   unsigned int                 i;
   char                         *p;

   queue = mulle__structqueue_create( sizeof( char), alignof( char), 32, 32, NULL);
   for( i = 0; i < 1000; i++)
      _mulle__structqueue_push( queue, (c = '0' + i % 10, &c), NULL);
   for( i = 0; i < 900; i++)
      _mulle__structqueue_pop( queue, NULL, &c);

   i = 0;
   mulle__structqueue_for( queue, p)
   {
      i++;
   }
   assert( i == 100);

   _mulle__structqueue_reset( queue, NULL);
   _mulle__structqueue_destroy( queue, NULL);
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
   run_test( misc_coverage);

   return( 0);
}
