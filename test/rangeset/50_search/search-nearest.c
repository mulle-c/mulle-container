#include <mulle-container/mulle-container.h>

#include <stdio.h>


int   main( int argc, char *argv[])
{
   struct mulle__rangeset   set;
   uintptr_t                location;
   uintptr_t                i;
   struct mulle_range       range;

   _mulle__rangeset_init( &set, 0, NULL);

   _mulle__rangeset_insert( &set, mulle_range_make( 2, 2), NULL);
   _mulle__rangeset_insert( &set, mulle_range_make( 6, 2), NULL);


   _mulle__rangeset_print( &set);

   for( i = 0; i < 10; i++)
   {
      printf( "   %tu = ", i);
      range = _mulle__rangeset_search_nearest( &set, i);
      if( ! range.length)
         printf( "not found\n");
      else
         printf( "%tu-%tu\n", range.location, range.length);
   }

   _mulle__rangeset_done( &set, NULL);

   return( 0);
}
