#include <mulle-container/mulle-container.h>
#include <stdio.h>

int test_basic_merge(void)
{
   struct mulle_rangemap   map;
   void                   *merged;

   mulle_rangemap_init( &map, 4, NULL);
   
   // test left merge
   mulle_rangemap_insert( &map, mulle_range_make( 0, 2), (void *) 0x1, NULL);
   mulle_rangemap_insert( &map, mulle_range_make( 2, 2), (void *) 0x2, &merged);
   printf("Left merge - merged value: %p\n", merged);
   mulle_rangemap_print( &map, stdout);

   // test right merge
   mulle_rangemap_insert( &map, mulle_range_make( 6, 2), (void *) 0x3, NULL);
   mulle_rangemap_insert( &map, mulle_range_make( 4, 2), (void *) 0x4, &merged);
   printf("Right merge - merged value: %p\n", merged);
   mulle_rangemap_print( &map, stdout);
   
   mulle_rangemap_done( &map);
   return 0;
}

int test_three_way_merge(void)
{
   struct mulle_rangemap   map;
   void                   *merged;

   mulle_rangemap_init( &map, 4, NULL);
   
   mulle_rangemap_insert( &map, mulle_range_make( 0, 2), (void *) 0x1, NULL);
   mulle_rangemap_insert( &map, mulle_range_make( 4, 2), (void *) 0x2, NULL);
   mulle_rangemap_insert( &map, mulle_range_make( 2, 2), (void *) 0x3, &merged);
   printf("Three-way merge - merged value: %p\n", merged);
   mulle_rangemap_print( &map, stdout);
   
   mulle_rangemap_done( &map);
   return 0;
}

int main(int argc, char *argv[])
{
   test_basic_merge();
   test_three_way_merge();
   return 0;
}
