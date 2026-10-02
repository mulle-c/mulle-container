#include <mulle-container/mulle-container.h>

#include <stdio.h>


int   main( int argc, char *argv[])
{

   mulle__rangeset_do( set)
   {
      _mulle__rangeset_insert( set, mulle_range_make( 0, 2), NULL);
      _mulle__rangeset_insert( set, mulle_range_make( 10, 2), NULL);
      _mulle__rangeset_insert( set, mulle_range_make( 20, 2), NULL);
      _mulle__rangeset_insert( set, mulle_range_make( 30, 0), NULL);
      _mulle__rangeset_insert( set, mulle_range_make_invalid(), NULL);
      _mulle__rangeset_print( set);
   }

   return( 0);
}
