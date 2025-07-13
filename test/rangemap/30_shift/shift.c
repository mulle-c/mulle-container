#include <mulle-container/mulle-container.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
   struct mulle_rangemap   map;
   void                   *value;
   struct mulle_range      range;

   mulle_rangemap_init( &map, 4, NULL);
   
   // Setup initial ranges
   mulle_rangemap_insert( &map, mulle_range_make( 2, 2), (void *) 0x1, NULL);
   mulle_rangemap_insert( &map, mulle_range_make( 6, 2), (void *) 0x2, NULL);
   printf("Initial map:\n");
   mulle_rangemap_print( &map, stdout);

   // Note: shift operation not implemented in public API yet
   printf("\nShift operations not yet implemented in public API\n");

   // Test value retrieval
   range = mulle_rangemap_search_exact( &map, 2, &value);
   printf("Value at 2: %p\n", value);
   
   mulle_rangemap_done( &map);

   return( 0);
}
