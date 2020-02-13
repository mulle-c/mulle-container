#include <mulle-container/mulle-container.h>

#include <stdio.h>


int   main( int argc, char *argv[])
{
   struct mulle__rangeset   set;

   _mulle__rangeset_init( &set, 0, NULL);

   _mulle__rangeset_insert( &set, mulle_range_create( 0, 2), NULL);
   _mulle__rangeset_insert( &set, mulle_range_create( 20, 2), NULL);

   printf( "Before:\n");
   _mulle__rangeset_print( &set);

   _mulle__rangeset_remove( &set, mulle_range_create( 0, 2), NULL);
   _mulle__rangeset_remove( &set, mulle_range_create( 20, 2), NULL);

   printf( "After:\n");
   _mulle__rangeset_print( &set);


   _mulle__rangeset_done( &set, NULL);

   return( 0);
}
