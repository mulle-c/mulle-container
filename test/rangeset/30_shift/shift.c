#include <mulle-container/mulle-container.h>

#include <stdio.h>


int   main( int argc, char *argv[])
{
   struct mulle__rangeset   set;

   _mulle__rangeset_init( &set, 0, NULL);

   _mulle__rangeset_insert( &set, mulle_range_make( 0, 5), NULL);
   _mulle__rangeset_insert( &set, mulle_range_make( 20, 10), NULL);

   printf( "Before:\n");
   _mulle__rangeset_print( &set);

   printf( "\nShift one up\n");
   _mulle__rangeset_shift( &set, 0, 1, NULL);
   _mulle__rangeset_print( &set);

   printf( "\nShift three down\n");
   _mulle__rangeset_shift( &set, 0, -3, NULL);
   _mulle__rangeset_print( &set);

//   printf( "\nShift mulle_range_location_max -3 up\n");
//   _mulle__rangeset_shift( &set, 0, mulle_range_location_max - 3, NULL);
//   _mulle__rangeset_print( &set);
 
   _mulle__rangeset_done( &set, NULL);

   return( 0);
}
