#include <mulle-container/mulle-container.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
   struct mulle_rangemap   map;
   
   mulle_rangemap_init( &map, 4, NULL);
   
   // Simple stress test - just insert a few ranges
   mulle_rangemap_insert( &map, mulle_range_make( 0, 2), (void *) 0x1, NULL);
   mulle_rangemap_insert( &map, mulle_range_make( 10, 2), (void *) 0x2, NULL);
   mulle_rangemap_insert( &map, mulle_range_make( 20, 2), (void *) 0x3, NULL);
   
   printf("Stress test completed successfully\n");
   
   mulle_rangemap_done( &map);
   return( 0);
}
