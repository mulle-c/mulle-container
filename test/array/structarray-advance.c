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


struct demo_struct
{
   char     a;
   int      b;
   char     c;
};


static void  advance_array( void)
{
   struct mulle_structarray   *array;

   array = mulle_structarray_create( sizeof( struct demo_struct),
                                     alignof( struct demo_struct),
                                     10,
                                     NULL);


   printf( "%zu\n", mulle_structarray_get_count( array));
   _mulle_structarray_advance( array, 20);
   printf( "%zu\n", mulle_structarray_get_count( array));

   mulle_structarray_destroy( array);
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
   run_test( advance_array, "zero array");
   return( 0);
}
