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
   double   x;
   char     y;
};


int  reverse_compare( void *_a, void *_b, void *context)
{
   struct demo_struct  *a = _a;
   struct demo_struct  *b = _b;

   if( a->x > b->x)
      return( -1);
   return( a->x != b->x);
}


#define LOOPS   16


static void  simple( void)
{
   struct demo_struct                   a;
   struct demo_struct                   *item;
   struct mulle_structarray             *array;
   struct mulle_structarrayenumerator   rover;
   unsigned int                         i;

   array = mulle_structarray_create( sizeof( struct demo_struct),
                                     alignof( struct demo_struct),
                                     LOOPS,
                                     NULL);

   assert( mulle_structarray_get_last( array) == NULL);

   for( i = 0; i < LOOPS; i++)
   {
      a.x = (double) i + 1848;
      a.y = i + 'A';

      mulle_structarray_add( array, &a);
   }

   // more or less just a syntax check
   printf( "Initial:\n");
   i = 0;
   mulle_structarray_for( array, item)
   {
      assert( item);
      printf( "\t%u: %g=%c\n", i, item->x, item->y);
      ++i;
   }


   mulle_qsort_r( array->_storage, LOOPS, sizeof( struct demo_struct), reverse_compare, NULL);

   printf( "\nReversed:\n");
   // more or less just a syntax check
   i = 0;
   mulle_structarray_for( array, item)
   {
      assert( item);
      printf( "\t%u: %g=%c\n", i, item->x, item->y);
      ++i;
   }

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
   run_test( simple, "simple");
   return( 0);
}
