#include <mulle-container/mulle-container.h>

#include <stdio.h>


int   main( int argc, char *argv[])
{
   struct _mulle_rangeset   set;

   _mulle_rangeset_init( &set, 0, NULL);

   _mulle_rangeset_insert( &set, mulle_range_create( 10, 2), NULL);

   _mulle_rangeset_insert( &set, mulle_range_create( 0, 2), NULL);

   _mulle_rangeset_insert( &set, mulle_range_create( 4, 2), NULL);

   _mulle_rangeset_insert( &set, mulle_range_create( 14, 2), NULL);

   _mulle_rangeset_insert( &set, mulle_range_create( 7, 2), NULL);


   _mulle_rangeset_print( &set);

   _mulle_rangeset_done( &set, NULL);

   return( 0);
}
