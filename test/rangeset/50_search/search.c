#include <mulle-container/mulle-container.h>

#include <stdio.h>


int   main( int argc, char *argv[])
{
   struct mulle__rangeset   set;
   int                      mode;
   uintptr_t                location;
   uintptr_t                i;

   _mulle__rangeset_init( &set, 0, NULL);

   _mulle__rangeset_insert( &set, mulle_range_make( 2, 2), NULL);
   _mulle__rangeset_insert( &set, mulle_range_make( 6, 2), NULL);

   _mulle__rangeset_print( &set);
   printf( "| # ");
   for( mode = mulle_rangeset_equal; mode <= mulle_rangeset_greater_than_or_equal; mode++)
      printf( "| %s ", mulle_rangeset_searchoperation_utf8_string( mode));
   printf( "|\n");

   printf( "|---");
   for( mode = mulle_rangeset_equal; mode <= mulle_rangeset_greater_than_or_equal; mode++)
      printf( "|%.*s", (int) strlen( mulle_rangeset_searchoperation_utf8_string( mode)) + 2, "--------");
   printf( "|\n");

   for( i = 0; i < 10; i++)
   {
      printf( "| %tu ", i);
      for( mode = mulle_rangeset_equal; mode <= mulle_rangeset_greater_than_or_equal; mode++)
      {
         location = _mulle__rangeset_search_location( &set, i, mode);
         printf( "|%.*s", (int) strlen( mulle_rangeset_searchoperation_utf8_string( mode)), "        ");
         if( location  == mulle_not_found_e)
            printf( "- ");
         else
            printf( "%tu ", location);
      }
      printf( "|\n");
   }
   _mulle__rangeset_done( &set, NULL);

   return( 0);
}
