//
//  main.c
//  test-assoc
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
   "Es haben viel fröhliche Menschen",
   "Lang vor uns gelebt",
   "und gelacht",
   "Den Ruhenden unter dem Rasen",
   "Sei fröhlich",

   "der Becher gebracht.",
   "Es werden viel fröhliche Menschen",
   "Lang nach uns des Lebens sich freun",
   "Uns Ruhenden unter dem Rasen",
   "Den Becher der Fröhlichkeit weihn."
};


static void  loops( void)
{
   struct mulle_assoc                        *assoc;
   struct mulle_assocenumerator              rover;
   struct mulle_pointerpair                  item;
   int                                       i;
   struct mulle_container_keyvaluecallback   callback;
   void                                      *key;
   void                                      *value;

   callback.keycallback    = mulle_container_keycallback_copied_cstring;
   callback.valuecallback  = mulle_container_valuecallback_copied_cstring;
   //
   // the string is "strdup" by the assoc on add, and will be freed when
   // removed or destroyed
   //
   assoc = mulle_assoc_create( 0,
                               &callback,
                               _mulle_pointerpair_compare_string_key,
                               NULL);

   for( i = 0; i < 10; i++)
      mulle_assoc_add( assoc, test_strings[ i], test_strings[ 10 - 1 - i]);

   for( i = 0; i < 10; i++)
      test_strings[ i][ 0] = 0; // destroy original

   printf( "Get (should be already sorted):\n");
   for( i = 0; i < 10; i++)
   {
      item = mulle_assoc_get_at_index( assoc, i);
      printf( "\t%s=%s\n", (char *) item.key, (char *) item.value);
   }
   printf( "\n");

   printf( "Enumeration:\n");
   rover = mulle_assoc_enumerate( assoc);
   while( mulle_assocenumerator_next( &rover, &key, &value))
      printf( "\t%s=%s\n", (char *) key, (char *) value);
   mulle_assocenumerator_done( &rover);
   printf( "\n");

   mulle_assoc_destroy( assoc);
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


int main( int argc, const char * argv[])
{
   size_t  i;

   for( i = 0; i < 10; i++)
      test_strings[ i] = strdup( test_strings[ i]);

   run_test( loops, "loops");

   for( i = 0; i < 10; i++)
      free( test_strings[ i]);

   return( 0);
}
