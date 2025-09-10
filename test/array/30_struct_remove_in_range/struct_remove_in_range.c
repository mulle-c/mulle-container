#include <mulle-container/mulle-container.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>
#include <string.h>

// Define a simple struct for testing
struct MyStruct
{
   int   a;
   char  b;
};


static void  print_array( struct mulle_structarray *array)
{
   unsigned int    i;
   struct MyStruct *p;

   for( i = 0; i < mulle_structarray_get_count( array); ++i)
   {
      p = (struct MyStruct *) mulle_structarray_get( array, i);
      printf( "%s{%d,%c}", i ? " " : "", p->a, p->b);
   }
   printf( "\n");
}


static void  setup_array( struct mulle_structarray *array)
{
   struct MyStruct   value;

   mulle_structarray_reset( array);

   value = (struct MyStruct){ .a = 1, .b = 'a' };
   mulle_structarray_add( array, &value);
   value = (struct MyStruct){ .a = 2, .b = 'b' };
   mulle_structarray_add( array, &value);
   value = (struct MyStruct){ .a = 3, .b = 'c' };
   mulle_structarray_add( array, &value);
   value = (struct MyStruct){ .a = 4, .b = 'd' };
   mulle_structarray_add( array, &value);
   value = (struct MyStruct){ .a = 5, .b = 'e' };
   mulle_structarray_add( array, &value);
}


static void  test_struct_remove_in_range( void)
{
   struct mulle_structarray   array;
   struct mulle_range         range;

   mulle_structarray_init( &array, sizeof( struct MyStruct),
                                   alignof( struct MyStruct),
                                   0,
                                   NULL);

   printf( "Test #1: Remove from front\n");
   setup_array( &array);
   range = mulle_range_make( 0, 2);
   mulle_structarray_remove_in_range( &array, range);
   print_array( &array);

   printf( "Test #2: Remove from middle\n");
   setup_array( &array);
   range = mulle_range_make( 2, 2);
   mulle_structarray_remove_in_range( &array, range);
   print_array( &array);

   printf( "Test #3: Remove from back\n");
   setup_array( &array);
   range = mulle_range_make( 3, 2);
   mulle_structarray_remove_in_range( &array, range);
   print_array( &array);

   printf( "Test #4: Remove all\n");
   setup_array( &array);
   range = mulle_range_make( 0, 5);
   mulle_structarray_remove_in_range( &array, range);
   print_array( &array);

   printf( "Test #5: Remove single element\n");
   setup_array( &array);
   range = mulle_range_make( 2, 1);
   mulle_structarray_remove_in_range( &array, range);
   print_array( &array);

   mulle_structarray_done( &array);
}


static void  run_test( void (*f)( void), char *name)
{
   mulle_testallocator_reset();
   printf( "%s\n", name);
   (f)();
   mulle_testallocator_reset();
}


int main(int argc, const char * argv[])
{
   run_test( test_struct_remove_in_range, "struct_remove_in_range");
   return( 0);
}
