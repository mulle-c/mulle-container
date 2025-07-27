#include <mulle-container/mulle-container.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
   struct mulle__rangemap   map;
   void                   *values[] = { (void *) 0x1, (void *) 0x2, (void *) 0x3 };

   _mulle__rangemap_init( &map, 4, NULL);
   
   _mulle__rangemap_insert( &map, mulle_range_make( 0, 2), values[0], NULL);
   _mulle__rangemap_insert( &map, mulle_range_make( 2, 2), values[1], NULL);
   _mulle__rangemap_insert( &map, mulle_range_make( 4, 2), values[2], NULL);
   
   _mulle__rangemap_done( &map, NULL);

   return( 0);
}