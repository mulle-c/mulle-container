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


// just tests that it don't crash
struct demo_struct
{
   double   x;
   char     y;
};


static void  simple( int n)
{
   unsigned int   i;

   mulle_flexarray_do( array, struct demo_struct, 4, n)
   {
      for( i = 0; i < n; i++)
      {
         array[ i] = (struct demo_struct) { i,  i};
      }
      break; // should not introduce memory leak
      
      // platform dependent output
      // printf( "%td\n", mulle_structarray_get_size_as_length( &array__array));
   }
}


static struct demo_struct   flex_return( int n)
{
   unsigned int   i;

   mulle_flexarray_do( array, struct demo_struct, 4, n)
   {
      for( i = 0; i < n; i++)
      {
         array[ i] = (struct demo_struct) { i,  i};
         if( i == n - 2)
            mulle_flexarray_return( array, array[ i]);
      }
   }
}



// the mulle_testallocator detects and aborts on leaks
static void  run_test( void (*f)( int), char *name, int n)
{
   mulle_testallocator_reset();
  // printf( "%s\n", name);
   (f)( n);
   mulle_testallocator_reset();
}


int main(int argc, const char * argv[])
{
   run_test( simple, "simple", 1);
   run_test( simple, "simple", 4);
   run_test( simple, "simple", 16);
   run_test( simple, "simple", 100000);
   run_test( simple, "simple", 0);

   run_test( (void (*)( int)) flex_return, "flex_return", 100);

   return( 0);
}
