//
//  init-variants.c
//  test-container
//
//  Exercises all _init_with_static / _init_default variants followed by
//  add + enumerate. Valgrind will catch uninitialized field reads.
//

#include <mulle-container/mulle-container.h>

#include <stdio.h>


static void  test_pointerarray_init_with_static( void)
{
   struct mulle__pointerarray   array;
   void                         *storage[ 8];
   void                         *item;

   _mulle__pointerarray_init_with_static_pointers( &array, storage, 8);

   _mulle__pointerarray_add( &array, (void *) 0x1, NULL);
   _mulle__pointerarray_add( &array, (void *) 0x2, NULL);

   mulle__pointerarray_for( &array, item)
      printf( "0x%tx\n", (ptrdiff_t) item);

   _mulle__pointerarray_done( &array, NULL);
}


static void  test_pointerarray_convenience_init_with_static( void)
{
   struct mulle_pointerarray   array;
   void                        *storage[ 8];
   void                        *item;

   mulle_pointerarray_init_with_static_pointer( &array, storage, 8, NULL);

   mulle_pointerarray_add( &array, (void *) 0x3);
   mulle_pointerarray_add( &array, (void *) 0x4);

   mulle_pointerarray_for( &array, item)
      printf( "0x%tx\n", (ptrdiff_t) item);

   mulle_pointerarray_done( &array);
}


static void  test_pointerset_init_with_static( void)
{
   struct mulle_pointerset   set;
   void                      *storage[ 8];

   memset( storage, 0xFF, sizeof( storage));  // poison
   mulle_pointerset_init_with_static_pointers( &set, storage, 8, NULL);

   mulle_pointerset_set( &set, (void *) 0x10);

   if( mulle_pointerset_get_count( &set) != 1)
      abort();
   if( ! mulle_pointerset_get( &set, (void *) 0x10))
      abort();

   printf( "0x10\n");

   mulle_pointerset_done( &set);
}


static void  test_rangeset_init_with_static( void)
{
   struct mulle__rangeset   set;
   struct mulle_range       storage[ 4];
   uintptr_t               i;
   uintptr_t               n;

   _mulle__rangeset_init_with_static_ranges( &set, storage, 4);

   _mulle__rangeset_insert( &set, mulle_range_make( 10, 5), NULL);
   _mulle__rangeset_insert( &set, mulle_range_make( 0, 3), NULL);

   n = _mulle__rangeset_get_rangecount( &set);
   for( i = 0; i < n; i++)
   {
      struct mulle_range   r;

      r = _mulle__rangeset_get_range( &set, i);
      printf( "%lu-%lu\n", (unsigned long) r.location,
                           (unsigned long) (r.location + r.length - 1));
   }

   _mulle__rangeset_done( &set, NULL);
}


static void  test_structarray_init_with_static( void)
{
   struct mulle__structarray   array;
   int                         storage[ 8];
   int                         *item;
   int                         value;

   _mulle__structarray_init_with_static_storage( &array,
                                                  sizeof( int),
                                                  alignof( int),
                                                  sizeof( storage),
                                                  storage);

   value = 100;
   _mulle__structarray_add( &array, &value, NULL);
   value = 200;
   _mulle__structarray_add( &array, &value, NULL);

   item = _mulle__structarray_get( &array, 0);
   printf( "%d\n", *item);
   item = _mulle__structarray_get( &array, 1);
   printf( "%d\n", *item);

   _mulle__structarray_done( &array, NULL);
}


static void  test_pointerqueue_init_default( void)
{
   struct mulle_pointerqueue   queue;
   void                        *item;

   mulle_pointerqueue_init_default( &queue, NULL);

   mulle_pointerqueue_push( &queue, (void *) 0xA0);
   mulle_pointerqueue_push( &queue, (void *) 0xB0);

   mulle_pointerqueue_for( &queue, item)
      printf( "0x%tx\n", (ptrdiff_t) item);

   mulle_pointerqueue_done( &queue);
}


int   main( int argc, const char *argv[])
{
   test_pointerarray_init_with_static();
   test_pointerarray_convenience_init_with_static();
   test_pointerset_init_with_static();
   test_rangeset_init_with_static();
   test_structarray_init_with_static();
   test_pointerqueue_init_default();
   return( 0);
}
