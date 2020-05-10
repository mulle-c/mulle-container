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


#define LOOPS   2

static void  simple( void)
{
   char                                       *s;
   struct demo_struct                         *a;
   struct demo_struct                         space;
   struct mulle_structarray                   *array;
   struct mulle_structarrayenumerator         rover;
   struct mulle_structarrayreverseenumerator  rrover;
   unsigned int                               i;
   void                                       *item;

   array = mulle_structarray_create( sizeof( struct demo_struct),
                                     alignof( struct demo_struct),
                                     10,
                                     NULL);

   assert( mulle_structarray_get_last( array) == NULL);

   for( i = 0; i < LOOPS; i++)
   {
      if( i < LOOPS / 2)
         a = _mulle_structarray_reserve( array);
      else
         a = &space;

      a->x = (double) i + 1848;
      a->y = i & 0x7F;

      if( i >= LOOPS / 2)
         _mulle_structarray_add( array, a);
   }

   assert( mulle_structarray_get_last( array) != NULL);

   for( i = 0; i < LOOPS; i++)
   {
      a = _mulle_structarray_get( array, i);

      assert( a->x == (double) i + 1848);
      assert( a->y == i & 0x7F);
   }

   i     = 0;
   rover = mulle_structarray_enumerate( array);
   while( a = _mulle_structarrayenumerator_next( &rover))
   {
      assert( a->x == (double) i + 1848);
      assert( a->y == i & 0x7F);
      ++i;
   }
   assert( i == LOOPS);
   mulle_structarrayenumerator_done( &rover);

   rrover = mulle_structarray_reverseenumerate( array);
   while( a = _mulle_structarrayreverseenumerator_next( &rrover))
   {
      --i;
      assert( a->x == (double) i + 1848);
      assert( a->y == i & 0x7F);
   }
   assert( i == 0);
   mulle_structarrayreverseenumerator_done( &rrover);

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
