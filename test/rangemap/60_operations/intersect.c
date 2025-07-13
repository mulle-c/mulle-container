#include <mulle-container/mulle-container.h>
#include <stdio.h>

int test(int empty)
{
   struct mulle_rangemap   map;
   struct mulle_range      range = mulle_range_make( 3, 2);
   void                   *value;

   mulle_rangemap_init( &map, 4, NULL);
   
   if( ! empty)
   {
      mulle_rangemap_insert( &map, mulle_range_make( 0, 2), (void *) 0x1, NULL);
      mulle_rangemap_insert( &map, range, (void *) 0x2, NULL);
      mulle_rangemap_insert( &map, mulle_range_make( 7, 2), (void *) 0x3, NULL);
      mulle_rangemap_insert( &map, mulle_range_make( 8, 2), (void *) 0x4, NULL);
   }

   mulle_rangemap_print( &map, stdout);

   // Note: intersects operation not implemented in public API yet
   printf("Intersects operations not yet implemented in public API\n");
   
   mulle_rangemap_done( &map);

   return( 0);
}

int main(int argc, char *argv[])
{
   test( 0);
   test( 1);
   return( 0);
}
