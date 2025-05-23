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


// MFing strings are const, though noone asked for that
static char   *test_strings[] =
{
   "Es haben viel fröhliche Menschen\n",
   "Lang vor uns gelebt ",
   "und gelacht\n",
   "Den Ruhenden unter dem Rasen\n",
   "Sei fröhlich ",

   "der Becher gebracht.\n",
   "Es werden viel fröhliche Menschen\n",
   "Lang nach uns des Lebens sich freun\n",
   "Uns Ruhenden unter dem Rasen\n",
   "Den Becher der Fröhlichkeit weihn.\n"
};


static void  loops( void)
{
   struct mulle_array                     *array;
   struct mulle_arrayenumerator           rover;
   struct mulle_arrayreverseenumerator    rerover;
   void                                   *item;
   int                                    i;
   char                                   *s;

   // the string is assumed const by the array and nothing happens during
   // addition or removal
   array = mulle_array_create( 0, &mulle_container_keycallback_nonowned_cstring, NULL);

   for( i = 0; i < 10; i++)
   {
      mulle_array_add( array, test_strings[ i]);
   }

   for( i = 0; i < 10; i++)
      printf( "%s", (char *) mulle_array_get( array, i));
   printf( "\n");

   rover = mulle_array_enumerate( array);
   while( mulle_arrayenumerator_next( &rover, &item))
      printf( "%s", (char *) item);
   mulle_arrayenumerator_done( &rover);
   printf( "\n");

   rerover = mulle_array_reverseenumerate( array);
   while( mulle_arrayreverseenumerator_next( &rerover, &item))
      printf( "%s", (char *) item);
   mulle_arrayreverseenumerator_done( &rerover);
   printf( "\n");

   s = mulle_array_describe( array);
   printf( "---\n%s\n", s);
   mulle_free( s);

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
   size_t  i;

   for( i = 0; i < 10; i++)
      test_strings[ i] = strdup( test_strings[ i]);

   run_test( loops, "loops");

   for( i = 0; i < 10; i++)
      free( test_strings[ i]);

   return( 0);
}
