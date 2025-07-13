#include <mulle-container/mulle-container.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
   struct mulle__rangemap   map;
   void                    *value;
   struct mulle_range       range;

   mulle__rangemap_do( map)
   {
      // Setup initial ranges
      _mulle__rangemap_insert( map, mulle_range_make( 2, 2), (void *) 0x1, NULL, NULL);
      _mulle__rangemap_insert( map, mulle_range_make( 6, 2), (void *) 0x2, NULL, NULL);
      printf("Initial map:\n");
      _mulle__rangemap_print( map);

      // Shift right
      printf("\nShifting right by 2 from position 4:\n");
      _mulle__rangemap_shift( map, 4, 2, NULL);
      _mulle__rangemap_print( map);

      // Test value retrieval after shift
      range = _mulle__rangemap_search_exact( map, 8, &value);
      printf("Value at 8: %p\n", value);

      // Shift left
      printf("\nShifting left by 3 from position 5:\n");
      _mulle__rangemap_shift( map, 5, -3, NULL);
      _mulle__rangemap_print( map);

      // Test value retrieval after shift
      range = _mulle__rangemap_search_exact( map, 5, &value);
      printf("Value at 5: %p\n", value);
   }

   return( 0);
}
