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
   struct mulle_pointerarray             *array;
   struct mulle_pointerarrayenumerator   rover;
   char                                  *sep;
   int                                   i;
   int                                   index;

   array = mulle_pointerarray_create( NULL);

   for( i = 0; i < 1000; i++)
   {
      index = random() % 10000;
      mulle_pointerarray_set_zeroing( array, index, (void *) (intptr_t) (index + 1));
   }

   for( i = 0; i < 10000; i++)
   {
      index = (intptr_t) mulle_pointerarray_get_zeroing( array, i);

      if( index && index != (intptr_t) (i + 1))
      {
         printf( "failed\n");
         exit( 1);
      }
   }
   mulle_pointerarrayenumerator_done( &rover);

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
