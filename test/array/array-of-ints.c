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


static void  loops( void)
{
   struct mulle_array                     *array;
   struct mulle_arrayenumerator           rover;
   struct mulle_arrayreverseenumerator    rerover;
   void                                   *item;
   int                                     i;

   array = mulle_array_create( 0, &mulle_container_keycallback_int, NULL);

   for( i = -100; i <= 100; i++)
      mulle_array_add( array, mulle_int_as_pointer( i));

   for( i = 0; i <= 200; i++)
      printf( "%d ", mulle_pointer_as_int( mulle_array_get( array, i)));
   printf( "\n");

   rover = mulle_array_enumerate( array);
   while( mulle_arrayenumerator_next( &rover, &item))
      printf( "%d ", mulle_pointer_as_int( item));
   mulle_arrayenumerator_done( &rover);
   printf( "\n");

   rerover = mulle_array_reverseenumerate( array);
   while( mulle_arrayreverseenumerator_next( &rerover, &item))
      printf( "%d ", mulle_pointer_as_int( item));
   mulle_arrayreverseenumerator_done( &rerover);
   printf( "\n");

   mulle_array_destroy( array);
}



// the mulle_testallocator detects and aborts on leaks
static void  run_test( void (*f)( void), char *name)
{
   mulle_testallocator_reset();
#ifdef DEBUG
   fprintf( stderr, "%s\n", name);
#endif
   (f)();
   mulle_testallocator_reset();
}


int main(int argc, const char * argv[])
{
   run_test( loops, "loops");
   return( 0);
}
