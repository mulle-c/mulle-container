#include <mulle-container/mulle-container.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
   struct mulle_rangemap   map;
   void                   *value;
   struct mulle_range      range;
   uintptr_t              i;

   mulle_rangemap_init( &map, 4, NULL);
   
   mulle_rangemap_insert( &map, mulle_range_make( 2, 2), (void *) 0x1, NULL);
   mulle_rangemap_insert( &map, mulle_range_make( 6, 2), (void *) 0x2, NULL);

   printf("Map contents:\n");
   mulle_rangemap_print( &map, stdout);

   printf("\nExact search results:\n");
   for( i = 0; i < 10; i++)
   {
      range = mulle_rangemap_search_exact( &map, i, &value);
      if( range.length)
         printf( "%td -> range: %td,%td value: %p\n", 
                i, range.location, range.length, value);
      else
         printf( "%td -> not found\n", i);
   }

   printf("\nNearest search results:\n");
   for( i = 0; i < 10; i++)
   {
      range = mulle_rangemap_search_nearest( &map, i, &value);
      if( range.length)
         printf( "%td -> nearest range: %td,%td value: %p\n", 
                i, range.location, range.length, value);
      else
         printf( "%td -> not found\n", i);
   }
   
   mulle_rangemap_done( &map);

   return( 0);
}
