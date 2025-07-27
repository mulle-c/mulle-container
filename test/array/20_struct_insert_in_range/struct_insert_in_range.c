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

static void  test_struct_insert_in_range( void)
{
   struct mulle_structarray   array;
   struct MyStruct            structs[ 5];
   struct mulle_range         range;
   unsigned int               i;
   char                       *sep;

   mulle_structarray_init( &array, sizeof( struct MyStruct), 
                                   alignof( struct MyStruct), 
                                   0, 
                                   NULL);

   printf( "Test #1: Insert into empty array\n");
   structs[ 0] = (struct MyStruct){ .a = 1, .b = 'a' };
   structs[ 1] = (struct MyStruct){ .a = 2, .b = 'b' };
   range        = mulle_range_make( 0, 2);
   mulle_structarray_insert_in_range( &array, range, structs);
   assert( mulle_structarray_get_count( &array) == 2);
   assert( ((struct MyStruct *) mulle_structarray_get( &array, 0))->a == 1);
   assert( ((struct MyStruct *) mulle_structarray_get( &array, 0))->b == 'a');
   assert( ((struct MyStruct *) mulle_structarray_get( &array, 1))->a == 2);
   assert( ((struct MyStruct *) mulle_structarray_get( &array, 1))->b == 'b');
   printf( "Array: ");
   sep = "";
   for( i = 0; i < mulle_structarray_get_count( &array); ++i)
   {
      printf( "%s{%d,%c}", sep,
                           ((struct MyStruct *) mulle_structarray_get( &array, i))->a,
                           ((struct MyStruct *) mulle_structarray_get( &array, i))->b);
      sep = " ";
   }
   printf( "\n");

   printf( "Test #2: Insert at front\n");
   structs[ 0] = (struct MyStruct){ .a = 10, .b = 'x' };
   structs[ 1] = (struct MyStruct){ .a = 20, .b = 'y' };
   range        = mulle_range_make( 0, 2);
   mulle_structarray_insert_in_range( &array, range, structs);
   assert( mulle_structarray_get_count( &array) == 4);
   assert( ((struct MyStruct *) mulle_structarray_get( &array, 0))->a == 10);
   assert( ((struct MyStruct *) mulle_structarray_get( &array, 0))->b == 'x');
   assert( ((struct MyStruct *) mulle_structarray_get( &array, 1))->a == 20);
   assert( ((struct MyStruct *) mulle_structarray_get( &array, 1))->b == 'y');
   assert( ((struct MyStruct *) mulle_structarray_get( &array, 2))->a == 1);
   assert( ((struct MyStruct *) mulle_structarray_get( &array, 2))->b == 'a');
   assert( ((struct MyStruct *) mulle_structarray_get( &array, 3))->a == 2);
   assert( ((struct MyStruct *) mulle_structarray_get( &array, 3))->b == 'b');
   printf( "Array: ");
   sep = "";
   for( i = 0; i < mulle_structarray_get_count( &array); ++i)
   {
      printf( "%s{%d,%c} ", sep, ((struct MyStruct *) mulle_structarray_get( &array, i))->a,
                           ((struct MyStruct *) mulle_structarray_get( &array, i))->b);
      sep = " ";
   }
   printf( "\n");

   printf( "Test #3: Insert in middle\n");
   structs[ 0] = (struct MyStruct){ .a = 100, .b = 'm' };
   structs[ 1] = (struct MyStruct){ .a = 200, .b = 'n' };
   range        = mulle_range_make( 2, 2);
   mulle_structarray_insert_in_range( &array, range, structs);
   assert( mulle_structarray_get_count( &array) == 6);
   assert( ((struct MyStruct *) mulle_structarray_get( &array, 0))->a == 10);
   assert( ((struct MyStruct *) mulle_structarray_get( &array, 1))->a == 20);
   assert( ((struct MyStruct *) mulle_structarray_get( &array, 2))->a == 100);
   assert( ((struct MyStruct *) mulle_structarray_get( &array, 3))->a == 200);
   assert( ((struct MyStruct *) mulle_structarray_get( &array, 4))->a == 1);
   assert( ((struct MyStruct *) mulle_structarray_get( &array, 5))->a == 2);
   printf( "Array: ");
   sep = "";
   for( i = 0; i < mulle_structarray_get_count( &array); ++i)
   {
      printf( "%s{%d,%c} ", sep, ((struct MyStruct *) mulle_structarray_get( &array, i))->a,
                           ((struct MyStruct *) mulle_structarray_get( &array, i))->b);
      sep = " ";
   }
   printf( "\n");

   printf( "Test #4: Insert at back\n");
   structs[ 0] = (struct MyStruct){ .a = 300, .b = 'z' };
   range        = mulle_range_make( 6, 1);
   mulle_structarray_insert_in_range( &array, range, structs);
   assert( mulle_structarray_get_count( &array) == 7);
   assert( ((struct MyStruct *) mulle_structarray_get( &array, 6))->a == 300);
   assert( ((struct MyStruct *) mulle_structarray_get( &array, 6))->b == 'z');
   printf( "Array: ");
   sep = "";
   for( i = 0; i < mulle_structarray_get_count( &array); ++i)
   {
      printf( "%s{%d,%c} ", sep, ((struct MyStruct *) mulle_structarray_get( &array, i))->a,
                           ((struct MyStruct *) mulle_structarray_get( &array, i))->b);
      sep = " ";
   }
   printf( "\n");

   printf( "Test #5: Insert single element into empty array\n");
   mulle_structarray_reset( &array);
   structs[ 0] = (struct MyStruct){ .a = 500, .b = 's' };
   range        = mulle_range_make( 0, 1);
   mulle_structarray_insert_in_range( &array, range, structs);
   assert( mulle_structarray_get_count( &array) == 1);
   assert( ((struct MyStruct *) mulle_structarray_get( &array, 0))->a == 500);
   assert( ((struct MyStruct *) mulle_structarray_get( &array, 0))->b == 's');
   printf( "Array: ");
   sep = "";
   for( i = 0; i < mulle_structarray_get_count( &array); ++i)
   {
      printf( "%s{%d,%c} ", sep, ((struct MyStruct *) mulle_structarray_get( &array, i))->a,
                           ((struct MyStruct *) mulle_structarray_get( &array, i))->b);
      sep = " ";
   }
   printf( "\n");

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
   run_test( test_struct_insert_in_range, "struct_insert_in_range");
   return( 0);
}