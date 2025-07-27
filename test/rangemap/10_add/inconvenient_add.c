#include <mulle-container/mulle-container.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
   struct mulle__rangemap   map;

   _mulle__rangemap_init( &map, 4, NULL);
   
   printf("Adding non-overlapping ranges:\n");
   _mulle__rangemap_insert( &map, mulle_range_make( 0, 2), (void *) 0x1, NULL);
   _mulle__rangemap_insert( &map, mulle_range_make( 4, 2), (void *) 0x2, NULL);
   _mulle__rangemap_insert( &map, mulle_range_make( 8, 2), (void *) 0x3, NULL);
   _mulle__rangemap_print( &map, stdout);

   printf("\nAttempting to add overlapping range (should fail):\n");
   _mulle__rangemap_insert( &map, mulle_range_make( 1, 3), (void *) 0x4, NULL);
   _mulle__rangemap_print( &map, stdout);

   printf("\nAttempting to add adjacent range (should not merge):\n");
   _mulle__rangemap_insert( &map, mulle_range_make( 2, 2), (void *) 0x5, NULL);
   _mulle__rangemap_print( &map, stdout);
   
   _mulle__rangemap_done( &map, NULL);

   return( 0);
}
