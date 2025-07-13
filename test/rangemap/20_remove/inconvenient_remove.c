#include <mulle-container/mulle-container.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
   struct mulle_rangemap   map;
   void                   *removed_values[4];
   size_t                 n_removed;

   mulle_rangemap_init( &map, 4, NULL);
   
   // Setup a continuous range
   mulle_rangemap_insert( &map, mulle_range_make( 0, 2), (void *) 0x1, NULL);
   mulle_rangemap_insert( &map, mulle_range_make( 2, 2), (void *) 0x2, NULL);
   mulle_rangemap_insert( &map, mulle_range_make( 4, 2), (void *) 0x3, NULL);
   mulle_rangemap_insert( &map, mulle_range_make( 6, 2), (void *) 0x4, NULL);
   printf("Initial map:\n");
   mulle_rangemap_print( &map, stdout);

   // Remove middle section
   printf("\nRemoving middle section (2,4):\n");
   mulle_rangemap_remove( &map, mulle_range_make( 2, 4), removed_values, &n_removed);
   printf("Removed %zu values: ", n_removed);
   for(size_t i = 0; i < n_removed; i++)
      printf("%p ", removed_values[i]);
   printf("\n");
   mulle_rangemap_print( &map, stdout);

   // Remove partial overlapping sections
   printf("\nRemoving partial overlapping section (1,3):\n");
   mulle_rangemap_remove( &map, mulle_range_make( 1, 3), removed_values, &n_removed);
   printf("Removed %zu values: ", n_removed);
   for(size_t i = 0; i < n_removed; i++)
      printf("%p ", removed_values[i]);
   printf("\n");
   mulle_rangemap_print( &map, stdout);

   // Try to remove non-existent range
   printf("\nRemoving non-existent range (10,2):\n");
   mulle_rangemap_remove( &map, mulle_range_make( 10, 2), removed_values, &n_removed);
   printf("Removed %zu values\n", n_removed);
   mulle_rangemap_print( &map, stdout);
   
   mulle_rangemap_done( &map);

   return( 0);
}
