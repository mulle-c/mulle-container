//
//  main.c
//  test-array
//
//  Created by Nat! on 04.11.15.
//  Copyright (c) 2015 Nat! - Mulle kybernetiK.
//  Copyright (c) 2015 Codeon GmbH.
//  All rights reserved.
//

#include <mulle-container/mulle-container.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>
#include <stdlib.h>



static void  run_random( void)
{
   int                         i;
   int                         zeroes;
   intptr_t                    index;
   struct mulle_pointerarray   *array;
   size_t                n;

   array  = mulle_pointerarray_create( NULL);

   zeroes = 0;
   index  = 1;
   for( i = 0; i < 10000; i++)
   {
      if( rand() % 10 == 1)
      {
         mulle_pointerarray_add( array, NULL);
         zeroes++;
      }
      else
      {
         mulle_pointerarray_add( array, (void *) index);
         ++index;
      }
   }

   n = mulle_pointerarray_get_count( array);
   if( n != 10000)
   {
      printf( "failed assertion\n");
      exit( 1);
   }

   mulle_pointerarray_compact( array, NULL);

   n = mulle_pointerarray_get_count( array);
   if( n != 10000 - zeroes)
   {
      printf( "failed compact\n");
      exit( 1);
   }   

   for( i = 0; i < n; i++)
   {
      index = (intptr_t) mulle_pointerarray_get( array, i);
      if( index != (intptr_t) (i + 1))
      {
         printf( "failed compacted values\n");
         exit( 1);
      }
   }

   mulle_pointerarray_destroy( array);
}



// the mulle_testallocator detects and aborts on leaks
static void  run_test( void (*f)( void), char *name)
{
   mulle_testallocator_reset();
  // printf( "%s\n", name);
   (f)();
   mulle_testallocator_reset();
}


int main(int argc, const char * argv[])
{
   run_test( run_random, "random");
   return( 0);
}
