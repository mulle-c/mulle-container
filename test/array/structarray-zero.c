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


static void  zero_array( void)
{
   struct mulle_structarray   *array;
   struct demo_struct         a;
   struct demo_struct         *p;
   int                        i;

   array = mulle_structarray_create( sizeof( struct demo_struct),
                                     alignof( struct demo_struct),
                                     10,
                                     NULL);


   printf( "%zu", mulle_structarray_get_count( array));
   _mulle_structarray_zero_to_count( array, 20);

   for( i = 0; i < 20; i++)
   {
      a.a = 'Z' - i;
      a.b = i * 1848;
      a.c = i + 'A';

      _mulle_structarray_set( array, i, &a);
   }

   printf( " -> %zu", mulle_structarray_get_count( array));
   _mulle_structarray_zero_to_count( array, 10);

   for( i = 0; i < 10; i++)
   {

      p = _mulle_structarray_get( array, i);
      assert( p->a == 'Z' - i);
      assert( p->b == i * 1848);
      assert( p->c == i + 'A');
   }

   printf( " -> %zu\n", mulle_structarray_get_count( array));

   mulle_structarray_destroy( array);
}


static void  set_count_array( void)
{
   struct mulle_structarray   *array;

   array = mulle_structarray_create( sizeof( struct demo_struct),
                                     alignof( struct demo_struct),
                                     10,
                                     NULL);


   printf( "%zu", mulle_structarray_get_count( array));
   _mulle_structarray_set_count( array, 20);
   printf( " -> %zu", mulle_structarray_get_count( array));
   _mulle_structarray_set_count( array, 10);
   printf( " -> %zu\n", mulle_structarray_get_count( array));

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
   run_test( set_count_array, "set count");
   run_test( zero_array, "zero array");
   return( 0);
}
