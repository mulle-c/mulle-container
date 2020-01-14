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


static void  simple( void)
{
   struct mulle_pointerarray              *array;
   struct mulle_pointerarrayenumerator    rover;
   void                                   *item;
   char                                   *s;

   array = mulle_pointerarray_create_nil( NULL);

   s     = "VfL";
   mulle_pointerarray_add( array, s);
   assert( s == mulle_pointerarray_get( array, 0)); // must have been copied

   mulle_pointerarray_add( array, "Bochum");
   mulle_pointerarray_add( array, "1848");

   assert( mulle_pointerarray_get_count( array) == 3);

   assert( ! strcmp( "VfL", mulle_pointerarray_get( array, 0)));
   assert( ! strcmp( "Bochum", mulle_pointerarray_get( array, 1)));
   assert( ! strcmp( "1848", mulle_pointerarray_get( array, 2)));

   rover = mulle_pointerarray_enumerate( array);
   item = mulle_pointerarrayenumerator_next( &rover);
   assert( item);
   assert( ! strcmp( "VfL", item));
   item = mulle_pointerarrayenumerator_next( &rover);
   assert( item);
   assert( ! strcmp( "Bochum", item));
   item = mulle_pointerarrayenumerator_next( &rover);
   assert( item);
   assert( ! strcmp( "1848", item));
   item = mulle_pointerarrayenumerator_next( &rover);
   assert( !item);
   mulle_pointerarrayenumerator_done( &rover);

   rover = mulle_pointerarray_reverseenumerate( array);
   item = mulle_pointerarrayenumerator_next( &rover);
   assert( item);
   assert( ! strcmp( "1848", item));
   item = mulle_pointerarrayenumerator_next( &rover);
   assert( item);
   assert( ! strcmp( "Bochum", item));
   item = mulle_pointerarrayenumerator_next( &rover);
   assert( item);
   assert( ! strcmp( "VfL", item));
   item = mulle_pointerarrayenumerator_next( &rover);
   assert( !item);
   mulle_pointerarrayenumerator_done( &rover);

   mulle_pointerarray_destroy( array);
}


static void  grow( void)
{
   struct mulle_pointerarray   array;
   size_t                      i;

   mulle_pointerarray_init( &array, 10, NULL, NULL);

   for( i = 0; i < 20; i++)
      mulle_pointerarray_add( &array, "VfL");

   assert( mulle_pointerarray_get_count( &array) == 20);

   mulle_pointerarray_done( &array);
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
   run_test( grow, "grow");
   return( 0);
}
