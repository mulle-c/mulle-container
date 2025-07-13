#include <mulle-container/mulle-container.h>
#include <stdio.h>

int test(int empty)
{
   struct mulle__rangemap   map;
   struct mulle_range       range = mulle_range_make( 3, 2);
   void                    *value;

   mulle__rangemap_do( map)
   {
      if( ! empty)
      {
         _mulle__rangemap_insert( map, mulle_range_make( 0, 2), (void *) 0x1, NULL, NULL);
         _mulle__rangemap_insert( map, range, (void *) 0x2, NULL, NULL);
         _mulle__rangemap_insert( map, mulle_range_make( 7, 2), (void *) 0x3, NULL, NULL);
         _mulle__rangemap_insert( map, mulle_range_make( 8, 2), (void *) 0x4, NULL, NULL);
      }

      _mulle__rangemap_print( map);

      for( range.length = 0; range.length < 10; range.length++)
         for( range.location = 0; range.location < 10; range.location++)
         {
            if( _mulle__rangemap_intersects( map, range, &value))
               printf( "{ %td, %td } intersects with value %p\n", 
                      range.location, range.length, value);
         }
   }

   return( 0);
}

int main(int argc, char *argv[])
{
   test( 0);
   test( 1);
   return( 0);
}
