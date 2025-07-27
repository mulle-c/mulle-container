#include <mulle-container/mulle-container.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
   struct mulle__rangemap   map;

   _mulle__rangemap_init( &map, 4, NULL);
   
   // Setup a continuous range
   _mulle__rangemap_insert( &map, mulle_range_make( 0, 2), (void *) 0x1, NULL);
   _mulle__rangemap_insert( &map, mulle_range_make( 2, 2), (void *) 0x2, NULL);
   _mulle__rangemap_insert( &map, mulle_range_make( 4, 2), (void *) 0x3, NULL);
   _mulle__rangemap_insert( &map, mulle_range_make( 6, 2), (void *) 0x4, NULL);
   printf("Initial map:\n");
   _mulle__rangemap_print( &map, stdout);

   // Remove middle section
   printf("\nRemoving middle section (2,2):\n");
   _mulle__rangemap_remove( &map, mulle_range_make( 2, 2), NULL);
   _mulle__rangemap_print( &map, stdout);

   // Remove partial overlapping sections (fails)
   printf("\nRemoving partial overlapping section (1,3):\n");
   _mulle__rangemap_remove( &map, mulle_range_make( 1, 3), NULL);
   _mulle__rangemap_print( &map, stdout);

   // Try to remove non-existent range
   printf("\nRemoving non-existent range (10,2):\n");
   _mulle__rangemap_remove( &map, mulle_range_make( 10, 2), NULL);
   _mulle__rangemap_print( &map, stdout);

   // Try to remove first
   printf("\nRemoving first range (0,2):\n");
   _mulle__rangemap_remove( &map, mulle_range_make( 0, 2), NULL);
   _mulle__rangemap_print( &map, stdout);
   
   printf("\nRemoving last range (6,2):\n");
   _mulle__rangemap_remove( &map, mulle_range_make( 6, 2), NULL);
   _mulle__rangemap_print( &map, stdout);

   printf("\nRemoving remaining range (4,2):\n");
   _mulle__rangemap_remove( &map, mulle_range_make( 4, 2), NULL);
   _mulle__rangemap_print( &map, stdout);


   _mulle__rangemap_done( &map, NULL);

   return( 0);
}
