#include <mulle-container/mulle-container.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
   struct mulle__rangemap   map;
   void                   *value;
   struct mulle_range      range;
   uintptr_t              i;
   uintptr_t              index;

   _mulle__rangemap_init( &map, 4, NULL);
   
   _mulle__rangemap_insert( &map, mulle_range_make( 2, 2), (void *) 0x1, NULL);
   _mulle__rangemap_insert( &map, mulle_range_make( 6, 2), (void *) 0x2, NULL);

   printf("Map contents:\n");
   _mulle__rangemap_print( &map, stdout);

   printf("\nSearch results:\n");
   for( i = 0; i < 10; i++)
   {
      index = _mulle__rangemap_search( &map, i);
      if( index != mulle_not_found_e)
      {
         value = _mulle__rangemap_get_value( &map, index);
         range = _mulle__rangemap_get_range( &map, index);
         printf( "%td -> range: %td,%td value: %p\n",
                i, range.location, range.length, value);
      }
      else
         printf( "%td -> not found\n", i);
   }
   
   _mulle__rangemap_done( &map, NULL);

   return( 0);
}
