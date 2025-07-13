#include <mulle-container/mulle-container.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
   struct mulle_rangemap   map;

   mulle_rangemap_init( &map, 4, NULL);
   
   // empty range
   mulle_rangemap_insert( &map, mulle_range_make( 0, 0), (void *) 0x1, NULL);
   // invalid range (overflow)
   mulle_rangemap_insert( &map, mulle_range_make( -1, 2), (void *) 0x2, NULL);
   // invalid range (max)
   mulle_rangemap_insert( &map, mulle_range_make( mulle_range_location_max, 2), (void *) 0x3, NULL);
   // null value (should be fine)
   mulle_rangemap_insert( &map, mulle_range_make( 0, 2), NULL, NULL);

   mulle_rangemap_print( &map, stdout);
   
   mulle_rangemap_done( &map);

   return( 0);
}
