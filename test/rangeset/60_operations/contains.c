#include <mulle-container/mulle-container.h>

#include <stdio.h>

int   test( int empty)
{
   struct mulle__rangeset   set;
   struct mulle_range       range = mulle_range_make( 3, 2);

   mulle__rangeset_do( set)
   {
      if( ! empty)
      {
         _mulle__rangeset_insert( set, mulle_range_make( 0, 2), NULL);
         _mulle__rangeset_insert_ranges( set, &range, 1, NULL);
         _mulle__rangeset_remove_ranges( set, &range, 1, NULL);

         _mulle__rangeset_insert( set, mulle_range_make( 3, 2), NULL);
         _mulle__rangeset_insert( set, mulle_range_make( 7, 2), NULL);
         _mulle__rangeset_insert( set, mulle_range_make( 8, 2), NULL);
      }

      _mulle__rangeset_print( set);

      for( range.length = 0; range.length < 10; range.length++)
         for( range.location = 0; range.location < 10; range.location++)
         {
            if( _mulle__rangeset_contains( set, range))
               printf( "{ %td, %td } contains\n", range.location, range.length);
         }
   }

   return( 0);
}


int   main( int argc, char *argv[])
{
   test( 0);
   test( 1);
   return( 0);
}