#include <mulle-container/mulle-container.h>
#include <stdio.h>

int test_remove(void)
{
   struct mulle_rangemap   map;
   void                   *removed_values[3];
   size_t                 n_removed;

   mulle_rangemap_init( &map, 4, NULL);
   
   // Setup initial map
   mulle_rangemap_insert( &map, mulle_range_make( 0, 2), (void *) 0x1, NULL);
   mulle_rangemap_insert( &map, mulle_range_make( 2, 2), (void *) 0x2, NULL);
   mulle_rangemap_insert( &map, mulle_range_make( 4, 2), (void *) 0x3, NULL);
   printf("Initial map:\n");
   mulle_rangemap_print( &map, stdout);

   // Test complete removal
   printf("\nRemoving range (2,2):\n");
   mulle_rangemap_remove( &map, mulle_range_make( 2, 2), removed_values, &n_removed);
   printf("Removed %zu values: ", n_removed);
   for(size_t i = 0; i < n_removed; i++)
      printf("%p ", removed_values[i]);
   printf("\n");
   mulle_rangemap_print( &map, stdout);

   // Test partial removal creating a hole
   printf("\nRemoving range (0,3):\n");
   mulle_rangemap_remove( &map, mulle_range_make( 0, 3), removed_values, &n_removed);
   printf("Removed %zu values: ", n_removed);
   for(size_t i = 0; i < n_removed; i++)
      printf("%p ", removed_values[i]);
   printf("\n");
   mulle_rangemap_print( &map, stdout);
   
   mulle_rangemap_done( &map);
   return 0;
}

int main(int argc, char *argv[])
{
   test_remove();
   return 0;
}
