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

#include <stdio.h>


static void  test_do( void)
{
   size_t   i;

   mulle_pointerarray_do( array)
   {
      for( i = 0; i < 20; i++)
         _mulle_pointerarray_add( array, "VfL");

      if( mulle_pointerarray_get_count( array) != 20)
         abort();
   }
}


static void  test_do_flexible( void)
{
   size_t   i;

   mulle_pointerarray_do_flexible( array, 10)
   {
      for( i = 0; i < 20; i++)
         _mulle_pointerarray_add( array, "VfL");

      if( mulle_pointerarray_get_count( array) != 20)
         abort();
   }
}


// the mulle_allocator_default detects and aborts on leaks
static void  run_test( void (*f)( void), char *name)
{
  // printf( "%s\n", name);
   (f)();
}


int main(int argc, const char * argv[])
{
   run_test( test_do, "do");
   run_test( test_do_flexible, "do_flexible");
   return( 0);
}
