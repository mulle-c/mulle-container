#include <mulle-container/mulle-container.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
   struct mulle_rangemap   map;
   void                   *merged;

   mulle_rangemap_init( &map, 4, NULL);
   
   // Create some gaps and then fill them
   mulle_rangemap_insert( &map, mulle_range_make( 0, 2), (void *) 0x1, NULL);
   mulle_rangemap_insert( &map, mulle_range_make( 4, 2), (void *) 0x2, NULL);
   mulle_rangemap_insert( &map, mulle_range_make( 8, 2), (void *) 0x3, NULL);
   printf("Initial map:\n");
   mulle_rangemap_print( &map, stdout);

   // Fill gaps with bigger ranges
   printf("\nFilling gaps with bigger range:\n");
   mulle_rangemap_insert( &map, mulle_range_make( 1, 6), (void *) 0x4, &merged);
   printf("Merged value: %p\n", merged);
   mulle_rangemap_print( &map, stdout);

   // Add overlapping ranges
   printf("\nAdding overlapping ranges:\n");
   mulle_rangemap_insert( &map, mulle_range_make( 7, 4), (void *) 0x5, &merged);
   printf("Merged value: %p\n", merged);
   mulle_rangemap_print( &map, stdout);
   
   mulle_rangemap_done( &map);

   return( 0);
}
