#include <mulle-container/mulle-container.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
   struct mulle_rangemap   map;
   void                   *values[] = { (void *) 0x1, (void *) 0x2, (void *) 0x3 };

   mulle_rangemap_init( &map, 4, NULL);
   
   mulle_rangemap_insert( &map, mulle_range_make( 0, 2), values[0], NULL);
   mulle_rangemap_insert( &map, mulle_range_make( 10, 2), values[1], NULL);
   mulle_rangemap_insert( &map, mulle_range_make( 20, 2), values[2], NULL);
   // test invalid adds
   mulle_rangemap_insert( &map, mulle_range_make( 30, 0), NULL, NULL);
   mulle_rangemap_insert( &map, mulle_range_make_invalid(), NULL, NULL);
   
   mulle_rangemap_print( &map, stdout);
   
   mulle_rangemap_done( &map);

   return( 0);
}
