#include <mulle-container/mulle-container.h>
#include <stdio.h>

int test_remove(void)
{
   struct mulle__rangemap   map;
   void                    *removed_values[3];
   size_t                  n_removed;

   mulle__rangemap_do( map)
   {
      // Setup initial map
      _mulle__rangemap_insert( map, mulle_range_make( 0, 2), (void *) 0x1, NULL, NULL);
      _mulle__rangemap_insert( map, mulle_range_make( 2, 2), (void *) 0x2, NULL, NULL);
      _mulle__rangemap_insert( map, mulle_range_make( 4, 2), (void *) 0x3, NULL, NULL);
      printf("Initial map:\n");
      _mulle__rangemap_print( map);

      // Test complete removal
      printf("\nRemoving range (2,2):\n");
      __mulle__rangemap_remove( map, mulle_range_make( 2, 2), removed_values, &n_removed, NULL);
      printf("Removed %zu values: ", n_removed);
      for(size_t i = 0; i < n_removed; i++)
         printf("%p ", removed_values[i]);
      printf("\n");
      _mulle__rangemap_print( map);

      // Test partial removal creating a hole
      printf("\nRemoving range (0,3):\n");
      __mulle__rangemap_remove( map, mulle_range_make( 0, 3), removed_values, &n_removed, NULL);
      printf("Removed %zu values: ", n_removed);
      for(size_t i = 0; i < n_removed; i++)
         printf("%p ", removed_values[i]);
      printf("\n");
      _mulle__rangemap_print( map);
   }
   return 0;
}

int main(int argc, char *argv[])
{
   test_remove();
   return 0;
}
