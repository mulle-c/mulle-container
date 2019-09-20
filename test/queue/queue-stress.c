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
#include <assert.h>

static void  stress_queue( int bucketsize, int spares)
{
   struct _mulle_pointerqueue   *queue;
   unsigned int                 i;
   intptr_t                     read_cnt;
   intptr_t                     write_cnt;
   intptr_t                     popped;

   queue = _mulle_pointerqueue_create( bucketsize, spares, NULL);

   read_cnt  = 0;
   write_cnt = 0;

   // fill more than remove
   for( i = 0; i < 10000; i++)
   {
      switch( rand() % 3)
      {
         default :
            _mulle_pointerqueue_push( queue, (void *) ++write_cnt, NULL);
            break;

         case 2 :
            popped = (intptr_t) _mulle_pointerqueue_pop( queue, NULL);
            assert( read_cnt <= write_cnt || popped == ++read_cnt);
      }
   }

   // remove more than fill
   for( i = 0; i < 10000; i++)
   {
      switch( rand() % 3)
      {
         case 2 :
            popped = (intptr_t) _mulle_pointerqueue_pop( queue, NULL);
            assert( read_cnt <= write_cnt || popped == ++read_cnt);
            break;

         default :
            _mulle_pointerqueue_push( queue, (void *) ++write_cnt, NULL);
      }
   }

   _mulle_pointerqueue_destroy( queue, NULL);
}


// the mulle_testallocator detects and aborts on leaks
static void  run_test( void (*f)( int, int), int bucketsize, int spares)
{
   mulle_testallocator_reset();
   (f)( bucketsize, spares);
   mulle_testallocator_reset();
}


int main(int argc, const char * argv[])
{
   int   i;

   for( i = 0; i < 100; i++)
   {
      run_test( stress_queue, 0, 0);
      run_test( stress_queue, 2, 0);
      run_test( stress_queue, 16, 0);

      run_test( stress_queue, 0, 2);
      run_test( stress_queue, 2, 2);
      run_test( stress_queue, 16, 2);

      run_test( stress_queue, 0, 16);
      run_test( stress_queue, 2, 16);
      run_test( stress_queue, 16, 16);
   }
   return( 0);
}
