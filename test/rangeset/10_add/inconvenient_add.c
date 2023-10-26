#include <mulle-container/mulle-container.h>

#include <stdio.h>


int   main( int argc, char *argv[])
{
   mulle__rangeset_do_flexible( set, 2)
   {
      _mulle__rangeset_insert( set, mulle_range_make( 10, 2), NULL);

      _mulle__rangeset_insert( set, mulle_range_make( 0, 2), NULL);

      _mulle__rangeset_insert( set, mulle_range_make( 4, 2), NULL);

      _mulle__rangeset_insert( set, mulle_range_make( 14, 2), NULL);

      _mulle__rangeset_insert( set, mulle_range_make( 7, 2), NULL);

      _mulle__rangeset_print( set);
   }

   return( 0);
}
