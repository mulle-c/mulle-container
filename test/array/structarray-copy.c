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


static void  add_array( void)
{
   struct demo_struct                         a;
   struct demo_struct                         *p;
   struct mulle_structarray                   *array;
   struct mulle_structarray                   *other;
   struct mulle_structarrayenumerator         rover;
   int                                        i;
   void                                       *item;

   array = mulle_structarray_create( sizeof( struct demo_struct),
                                     alignof( struct demo_struct),
                                     10,
                                     NULL);
   other = mulle_structarray_create( sizeof( struct demo_struct),
                                     alignof( struct demo_struct),
                                     10,
                                     NULL);
   assert( mulle_structarray_get_last( other) == NULL);

   for( i = 0; i < 10; i++)
   {
      a.x = (double) i + 1848;
      a.y = i + 'A';

      _mulle_structarray_add( other, &a);
   }

   // coverage
   mulle_structarray_add_structarray( array, NULL, mulle_range_make_all());
   // coverage
   mulle_structarray_add_structarray( array, other, mulle_range_make( 1, 0));

   // for real
   mulle_structarray_add_structarray( array, other, mulle_range_make( 1, 8));

   i     = 0;
   rover = mulle_structarray_enumerate( array);
   while( _mulle_structarrayenumerator_next( &rover, (void **) &p))
   {
      printf( "%d: %.0f,%c\n", i, p->x, p->y);
      ++i;
   }
   mulle_structarrayenumerator_done( &rover);

   mulle_structarray_destroy( other);
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
   run_test( add_array, "add array");
   return( 0);
}
