#include <mulle-container/mulle-container.h>

#include <stdio.h>


int   main( int argc, char *argv[])
{
   struct mulle__rangeset   set;

   _mulle__rangeset_init( &set, 0, NULL);

   // no length
   _mulle__rangeset_insert( &set, mulle_range_make( 0, 0), NULL);

   // contains mulle_not_found_e
   _mulle__rangeset_insert( &set, mulle_range_make( mulle_not_found_e - 1, 2), NULL);

   // wrap around
   _mulle__rangeset_insert( &set, mulle_range_make( -2, 4), NULL);

   // should be 0
   printf( "%ld\n", (long) _mulle__rangeset_get_rangecount( &set));

   _mulle__rangeset_done( &set, NULL);

   return( 0);
}
