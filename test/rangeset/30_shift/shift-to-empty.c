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

   // shifting everything out must empty the set and not crash
   printf( "\nShift 100 down\n");
   _mulle__rangeset_shift( &set, 0, -100, NULL);
   printf( "Rangecount: %lu\n",
           (unsigned long) _mulle__rangeset_get_rangecount( &set));

   _mulle__rangeset_done( &set, NULL);

   return( 0);
}
