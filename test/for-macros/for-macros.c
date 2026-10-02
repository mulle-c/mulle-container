//
//  for-macros.c
//
//  Coverage for container `_for` convenience macros that had no call site
//  anywhere in the project:
//
//     mulle__pointerqueue_for   mulle_structqueue_for
//     mulle__structarray_for    mulle__assoc_for
//     mulle__map_for            mulle__pointermap_for
//     mulle__pointerpairarray_for
//
//  and the `_for_reverse` siblings that likewise had no call site:
//
//     mulle_array_for_reverse          mulle__array_for_reverse
//     mulle__pointerarray_for_reverse  mulle__structarray_for_reverse
//
//  Several of these had never even been compiled (their enumerator call had
//  a stale signature), so this test is also what keeps them working.
//
#ifdef NDEBUG
# undef NDEBUG
#endif
#include <assert.h>
#include <stddef.h>
#include <string.h>

#include <mulle-container/mulle-container.h>


static void   test_pointerqueue_for( void)
{
   struct mulle_pointerqueue   *queue;
   void                        *item;
   int                         count;
   int                         i;

   queue = mulle_pointerqueue_create( 0, 0, NULL);
   assert( queue != NULL);

   for( i = 0; i < 8; i++)
      mulle_pointerqueue_push( queue, mulle_int_as_pointer( i + 1));

   count = 0;
   mulle__pointerqueue_for( &queue->base, item)
   {
      assert( item == mulle_int_as_pointer( count + 1));
      ++count;
   }
   assert( count == 8);

   mulle_pointerqueue_destroy( queue);
}


static void   test_structqueue_for( void)
{
   struct mulle_structqueue   *queue;
   void                       *item;
   char                       c;
   int                        count;
   int                        i;

   queue = mulle_structqueue_create( sizeof( char), alignof( char), 0, 0, NULL);
   assert( queue != NULL);

   for( i = 0; i < 8; i++)
   {
      c = (char) ('A' + i);
      mulle_structqueue_push( queue, &c);
   }

   count = 0;
   mulle_structqueue_for( queue, item)
   {
      assert( *(char *) item == (char) ('A' + count));
      ++count;
   }
   assert( count == 8);

   mulle_structqueue_destroy( queue);
}


static void   test_structarray_for( void)
{
   struct mulle_structarray   *array;
   void                       *item;
   int                        count;
   int                        i;
   int                        v;

   array = mulle_structarray_create( sizeof( int), alignof( int), 0, NULL);
   assert( array != NULL);

   for( i = 0; i < 8; i++)
   {
      v = i;
      mulle_structarray_add( array, &v);
   }

   count = 0;
   mulle__structarray_for( &array->base, item)
   {
      assert( *(int *) item == count);
      ++count;
   }
   assert( count == 8);

   mulle_structarray_destroy( array);
}


static void   test_assoc_for( void)
{
   struct mulle_container_keyvaluecallback   callback;
   struct mulle_assoc                       *assoc;
   void                                     *key;
   void                                     *value;
   int                                      count;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   assoc = mulle_assoc_create( 0, &callback, _mulle_pointerpair_compare_string_key, NULL);
   assert( assoc != NULL);

   mulle_assoc_set( assoc, "1848", "VfL");
   mulle_assoc_set( assoc, "Bochum", "VfL");

   count = 0;
   mulle__assoc_for( &assoc->base, &callback, _mulle_pointerpair_compare_string_key, NULL, key, value)
   {
      assert( key != NULL);
      assert( value != NULL);
      ++count;
   }
   assert( count == 2);

   mulle_assoc_destroy( assoc);
}


static void   test_map_for( void)
{
   struct mulle_container_keyvaluecallback   callback;
   struct mulle_map                         *map;
   void                                     *key;
   void                                     *value;
   int                                      count;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   map = mulle_map_create( 0, &callback, NULL);
   assert( map != NULL);

   mulle_map_set( map, "1848", "VfL");
   mulle_map_set( map, "Bochum", "VfL");

   count = 0;
   mulle__map_for( &map->base, &callback, key, value)
   {
      assert( key != NULL);
      assert( value != NULL);
      ++count;
   }
   assert( count == 2);

   mulle_map_destroy( map);
}


static void   test_pointermap_for( void)
{
   struct mulle__pointermap   *map;
   void                       *key;
   void                       *value;
   int                        count;

   map = mulle__pointermap_create( 0, 0, NULL);
   assert( map != NULL);

   _mulle__pointermap_set( map, mulle_int_as_pointer( 1), mulle_int_as_pointer( 11), NULL);
   _mulle__pointermap_set( map, mulle_int_as_pointer( 2), mulle_int_as_pointer( 22), NULL);

   count = 0;
   mulle__pointermap_for( map, key, value)
   {
      assert( key != NULL);
      assert( value != NULL);
      ++count;
   }
   assert( count == 2);

   mulle__pointermap_destroy( map, NULL);
}


static void   test_inner_pointerpairarray_for( void)
{
   struct mulle__pointerpairarray   *array;
   struct mulle_pointerpair         pair;
   int                              count;

   array = mulle__pointerpairarray_create( NULL);
   assert( array != NULL);

   _mulle__pointerpairarray_add( array,
                                 mulle_pointerpair_make( mulle_int_as_pointer( 1),
                                                         mulle_int_as_pointer( 11)),
                                 NULL);
   _mulle__pointerpairarray_add( array,
                                 mulle_pointerpair_make( mulle_int_as_pointer( 2),
                                                         mulle_int_as_pointer( 22)),
                                 NULL);

   count = 0;
   mulle__pointerpairarray_for( array, pair)
   {
      ++count;
      assert( pair.key == mulle_int_as_pointer( count));
      assert( pair.value == mulle_int_as_pointer( count * 11));
   }
   assert( count == 2);

   mulle__pointerpairarray_destroy( array, NULL);
}


static void   test_array_for_reverse( void)
{
   struct mulle_array   *array;
   void                 *item;
   char                 *expected[ 3];
   int                  count;

   expected[ 0] = "1848";
   expected[ 1] = "Bochum";
   expected[ 2] = "VfL";

   array = mulle_array_create( 0, &mulle_container_keycallback_copied_cstring, NULL);
   assert( array != NULL);

   mulle_array_add( array, "VfL");
   mulle_array_add( array, "Bochum");
   mulle_array_add( array, "1848");

   count = 0;
   mulle_array_for_reverse( array, item)
   {
      assert( ! strcmp( (char *) item, expected[ count]));
      ++count;
   }
   assert( count == 3);

   mulle_array_destroy( array);
}


static void   test_inner_array_for_reverse( void)
{
   struct mulle_array   *array;
   void                 *item;
   char                 *expected[ 3];
   int                  count;

   expected[ 0] = "1848";
   expected[ 1] = "Bochum";
   expected[ 2] = "VfL";

   array = mulle_array_create( 0, &mulle_container_keycallback_copied_cstring, NULL);
   assert( array != NULL);

   mulle_array_add( array, "VfL");
   mulle_array_add( array, "Bochum");
   mulle_array_add( array, "1848");

   count = 0;
   mulle__array_for_reverse( &array->base, array->callback, item)
   {
      assert( ! strcmp( (char *) item, expected[ count]));
      ++count;
   }
   assert( count == 3);

   mulle_array_destroy( array);
}


static void   test_inner_pointerarray_for_reverse( void)
{
   struct mulle__pointerarray   array;
   void                         *storage[ 8];
   void                         *item;
   int                          count;

   _mulle__pointerarray_init_with_static_pointers( &array, storage, 8);

   _mulle__pointerarray_add( &array, mulle_int_as_pointer( 1), NULL);
   _mulle__pointerarray_add( &array, mulle_int_as_pointer( 2), NULL);
   _mulle__pointerarray_add( &array, mulle_int_as_pointer( 3), NULL);

   count = 3;
   mulle__pointerarray_for_reverse( &array, item)
   {
      assert( item == mulle_int_as_pointer( count));
      --count;
   }
   assert( count == 0);

   _mulle__pointerarray_done( &array, NULL);
}


static void   test_inner_structarray_for_reverse( void)
{
   struct mulle__structarray   array;
   int                         storage[ 8];
   void                        *item;
   int                         count;
   int                         v;

   _mulle__structarray_init_with_static_storage( &array,
                                                  sizeof( int),
                                                  alignof( int),
                                                  sizeof( storage),
                                                  storage);

   for( v = 0; v < 8; v++)
      _mulle__structarray_add( &array, &v, NULL);

   count = 7;
   mulle__structarray_for_reverse( &array, item)
   {
      assert( *(int *) item == count);
      --count;
   }
   assert( count == -1);

   _mulle__structarray_done( &array, NULL);
}


int   main( void)
{
   test_pointerqueue_for();
   test_structqueue_for();
   test_structarray_for();
   test_assoc_for();
   test_map_for();
   test_pointermap_for();
   test_inner_pointerpairarray_for();

   test_array_for_reverse();
   test_inner_array_for_reverse();
   test_inner_pointerarray_for_reverse();
   test_inner_structarray_for_reverse();

   return( 0);
}
