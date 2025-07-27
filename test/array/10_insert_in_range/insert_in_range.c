#include <mulle-container/mulle-container.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>
#include <string.h>

static void  test_insert_in_range( void)
{
   struct mulle_pointerarray   array;
   void                        *pointers[ 5];
   void                        *item;
   struct mulle_range          range;
   unsigned int                i;
   char                        *sep;

   mulle_pointerarray_init( &array, 0, NULL);

   printf( "Test #1: Insert into empty array\n");
   pointers[ 0] = (void *) "A";
   pointers[ 1] = (void *) "B";
   range        = mulle_range_make( 0, 2);
   _mulle__pointerarray_insert_in_range( &array, range, pointers, NULL);
   assert( mulle_pointerarray_get_count( &array) == 2);
   assert( ! strcmp( _mulle_pointerarray_get( &array, 0), "A"));
   assert( ! strcmp( _mulle_pointerarray_get( &array, 1), "B"));
   printf( "Array: ");
   sep = "";
   for( i = 0; i < mulle_pointerarray_get_count( &array); ++i)
   {
      printf( "%s%s", sep, (char *) _mulle_pointerarray_get( &array, i));
      sep = " ";
   }
   printf( "\n");

   printf( "Test #2: Insert at front\n");
   pointers[ 0] = (void *) "X";
   pointers[ 1] = (void *) "Y";
   range        = mulle_range_make( 0, 2);
   _mulle__pointerarray_insert_in_range( &array, range, pointers, NULL);
   assert( mulle_pointerarray_get_count( &array) == 4);
   assert( ! strcmp( _mulle_pointerarray_get( &array, 0), "X"));
   assert( ! strcmp( _mulle_pointerarray_get( &array, 1), "Y"));
   assert( ! strcmp( _mulle_pointerarray_get( &array, 2), "A"));
   assert( ! strcmp( _mulle_pointerarray_get( &array, 3), "B"));
   printf( "Array: ");
   sep = "";
   for( i = 0; i < mulle_pointerarray_get_count( &array); ++i)
   {
      printf( "%s%s", sep, (char *) _mulle_pointerarray_get( &array, i));
      sep = " ";
   }
   printf( "\n");

   printf( "Test #3: Insert in middle\n");
   pointers[ 0] = (void *) "M";
   pointers[ 1] = (void *) "N";
   range        = mulle_range_make( 2, 2);
   _mulle__pointerarray_insert_in_range( &array, range, pointers, NULL);
   assert( mulle_pointerarray_get_count( &array) == 6);
   assert( ! strcmp( _mulle_pointerarray_get( &array, 0), "X"));
   assert( ! strcmp( _mulle_pointerarray_get( &array, 1), "Y"));
   assert( ! strcmp( _mulle_pointerarray_get( &array, 2), "M"));
   assert( ! strcmp( _mulle_pointerarray_get( &array, 3), "N"));
   assert( ! strcmp( _mulle_pointerarray_get( &array, 4), "A"));
   assert( ! strcmp( _mulle_pointerarray_get( &array, 5), "B"));
   printf( "Array: ");
   sep = "";
   for( i = 0; i < mulle_pointerarray_get_count( &array); ++i)
   {
      printf( "%s%s", sep, (char *) _mulle_pointerarray_get( &array, i));
      sep = " ";

   }
   printf( "\n");

   printf( "Test #4: Insert at back\n");
   pointers[ 0] = (void *) "Z";
   range        = mulle_range_make( 6, 1);
   _mulle__pointerarray_insert_in_range( &array, range, pointers, NULL);
   assert( mulle_pointerarray_get_count( &array) == 7);
   assert( ! strcmp( _mulle_pointerarray_get( &array, 6), "Z"));
   printf( "Array: ");
   sep = "";
   for( i = 0; i < mulle_pointerarray_get_count( &array); ++i)
   {
      printf( "%s%s", sep, (char *) _mulle_pointerarray_get( &array, i));
      sep = " ";
   }

   printf( "\n");

   printf( "Test #5: Insert single element into empty array\n");
   mulle_pointerarray_reset( &array);
   pointers[ 0] = (void *) "S";
   range        = mulle_range_make( 0, 1);
   _mulle__pointerarray_insert_in_range( &array, range, pointers, NULL);
   assert( mulle_pointerarray_get_count( &array) == 1);
   assert( ! strcmp( _mulle_pointerarray_get( &array, 0), "S"));
   printf( "Array: ");
   sep = "";
   for( i = 0; i < mulle_pointerarray_get_count( &array); ++i)
   {
      printf( "%s%s", sep, (char *) _mulle_pointerarray_get( &array, i));
      sep = " ";
   }
   printf( "\n");

   mulle_pointerarray_done( &array);
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
   run_test( test_insert_in_range, "insert_in_range");
   return( 0);
}