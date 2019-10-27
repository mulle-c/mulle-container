#include <mulle-container/mulle-container.h>

#include <stdio.h>


int   main( int argc, char *argv[])
{
   struct _mulle_rangeset   set;

   _mulle_rangeset_init( &set, 0, NULL);

   // no length
   _mulle_rangeset_insert( &set, mulle_range_create( 0, 0), NULL);

   // contains mulle_not_found_e
   _mulle_rangeset_insert( &set, mulle_range_create( mulle_not_found_e - 1, 2), NULL);

   // wrap around
   _mulle_rangeset_insert( &set, mulle_range_create( -2, 4), NULL);

   // should be 0
   printf( "%ld\n", (long) _mulle_rangeset_get_rangecount( &set));

   _mulle_rangeset_done( &set, NULL);

   return( 0);
}
