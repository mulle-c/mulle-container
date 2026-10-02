//
//  do-macros.c
//
//  Coverage for the container `_do` convenience macros that had no call site
//  anywhere in the project (same generator as the `_for` macros, see
//  for-macros.c):
//
//     mulle__assoc_do              mulle__map_do
//     mulle__pointerset_do         mulle__rangemap_do
//     mulle__structarray_do        mulle__structarray_do_flexible
//     mulle_pointerpairarray_do
//
//  plus the orphaned `_default` creation macros and the data initializer:
//
//     mulle_array_init_default           mulle_array_create_default
//     _mulle_pointerarray_init_default   mulle_pointerarray_init_default
//     mulle_pointerarray_create_default
//     mulle_structarray_init_default     mulle_structarray_create_default
//     MULLE_POINTERSET_DATA
//
//  and the second half of the `_do` family, the inner/public variants that
//  live outside the generator comments:
//
//     mulle__array_do          mulle__array_do_flexible
//     mulle__pointerarray_do   mulle__pointerpairarray_do
//     mulle__set_do            mulle__set_for
//     mulle__structqueue_do    mulle_structarray_do
//
//  None of these had ever been compiled, so this test is what keeps them
//  working.
//
#ifdef NDEBUG
# undef NDEBUG
#endif
#include <assert.h>
#include <stddef.h>
#include <string.h>

#include <mulle-container/mulle-container.h>


static void   test_assoc_do( void)
{
   struct mulle_container_keyvaluecallback   callback;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   mulle__assoc_do( assoc, &callback)
   {
      _mulle__assoc_add( assoc, "1848", "VfL", &callback, NULL);
      _mulle__assoc_add( assoc, "Bochum", "VfL", &callback, NULL);
      assert( _mulle__assoc_get_count( assoc) == 2);
   }
}


static void   test_map_do( void)
{
   struct mulle_container_keyvaluecallback   callback;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   mulle__map_do( map, &callback)
   {
      _mulle__map_set( map, "1848", "VfL", &callback, NULL);
      _mulle__map_set( map, "Bochum", "VfL", &callback, NULL);
      assert( _mulle__map_get_count( map) == 2);
   }
}


static void   test_pointerset_do( void)
{
   mulle__pointerset_do( set)
   {
      _mulle__pointerset_set( set, mulle_int_as_pointer( 1), NULL);
      _mulle__pointerset_set( set, mulle_int_as_pointer( 2), NULL);
      assert( _mulle__pointerset_get_count( set) == 2);
   }
}


static void   test_rangemap_do( void)
{
   mulle__rangemap_do( map)
   {
      _mulle__rangemap_insert( map, mulle_range_make( 0, 5), mulle_int_as_pointer( 1), NULL);
      _mulle__rangemap_insert( map, mulle_range_make( 5, 5), mulle_int_as_pointer( 2), NULL);
      assert( _mulle__rangemap_get_count( map) == 2);
   }
}


static void   test_structarray_do( void)
{
   mulle__structarray_do( array, int)
   {
      int   v;
      int   i;

      for( i = 0; i < 8; i++)
      {
         v = i;
         _mulle__structarray_add( array, &v, NULL);
      }
      assert( _mulle__structarray_get_count( array) == 8);
   }
}


static void   test_structarray_do_flexible( void)
{
   mulle__structarray_do_flexible( array, int, 16)
   {
      int   v;
      int   i;

      for( i = 0; i < 8; i++)
      {
         v = i;
         _mulle__structarray_add( array, &v, NULL);
      }
      assert( _mulle__structarray_get_count( array) == 8);
   }
}


static void   test_pointerpairarray_do( void)
{
   mulle_pointerpairarray_do( array)
   {
      _mulle_pointerpairarray_add_kv( array,
                                      mulle_int_as_pointer( 1),
                                      mulle_int_as_pointer( 11));
      _mulle_pointerpairarray_add_kv( array,
                                      mulle_int_as_pointer( 2),
                                      mulle_int_as_pointer( 22));
      assert( _mulle_pointerpairarray_get_count( array) == 2);
   }
}


static void   test_inner_array_do( void)
{
   const struct mulle_container_keycallback   *callback;

   callback = &mulle_container_keycallback_copied_cstring;

   mulle__array_do( array, callback)
   {
      _mulle__array_add( array, "VfL", callback, NULL);
      _mulle__array_add( array, "Bochum", callback, NULL);
      assert( _mulle__array_get_count( array) == 2);
   }
}


static void   test_inner_array_do_flexible( void)
{
   const struct mulle_container_keycallback   *callback;

   callback = &mulle_container_keycallback_copied_cstring;

   mulle__array_do_flexible( array, 64, callback)
   {
      _mulle__array_add( array, "VfL", callback, NULL);
      _mulle__array_add( array, "Bochum", callback, NULL);
      assert( _mulle__array_get_count( array) == 2);
   }
}


static void   test_inner_pointerarray_do( void)
{
   mulle__pointerarray_do( array)
   {
      _mulle__pointerarray_add( array, mulle_int_as_pointer( 1), NULL);
      _mulle__pointerarray_add( array, mulle_int_as_pointer( 2), NULL);
      assert( _mulle__pointerarray_get_count( array) == 2);
   }
}


static void   test_inner_pointerpairarray_do( void)
{
   mulle__pointerpairarray_do( array)
   {
      _mulle__pointerpairarray_add( array,
                                    mulle_pointerpair_make( mulle_int_as_pointer( 1),
                                                            mulle_int_as_pointer( 11)),
                                    NULL);
      _mulle__pointerpairarray_add( array,
                                    mulle_pointerpair_make( mulle_int_as_pointer( 2),
                                                            mulle_int_as_pointer( 22)),
                                    NULL);
      assert( _mulle__pointerpairarray_get_count( array) == 2);
   }
}


static void   test_inner_set_do( void)
{
   const struct mulle_container_keycallback   *callback;

   callback = &mulle_container_keycallback_copied_cstring;

   mulle__set_do( set, callback)
   {
      _mulle__set_set( set, "VfL", callback, NULL);
      _mulle__set_set( set, "Bochum", callback, NULL);
      assert( _mulle__set_get_count( set) == 2);
   }
}


static void   test_inner_set_for( void)
{
   const struct mulle_container_keycallback   *callback;
   struct mulle__setenumerator                rover;
   struct mulle__set                          *set;
   void                                       *item;
   int                                        count;

   callback = &mulle_container_keycallback_copied_cstring;

   set = _mulle__set_create( 0, 0, callback, NULL);
   assert( set != NULL);

   _mulle__set_set( set, "VfL", callback, NULL);
   _mulle__set_set( set, "Bochum", callback, NULL);

   count = 0;
   mulle__set_for( set, callback, item)
   {
      assert( item != NULL);
      ++count;
   }
   assert( count == 2);

   _mulle__set_destroy( set, callback, NULL);

   // NULL set yields the empty enumerator (this is what used to not link)
   rover = mulle__set_enumerate( NULL, callback);
   assert( ! _mulle__setenumerator_next( &rover, &item));
}


static void   test_inner_structqueue_do( void)
{
   mulle__structqueue_do( queue, int)
   {
      int   v;
      int   i;

      for( i = 0; i < 8; i++)
      {
         v = i;
         _mulle__structqueue_push( queue, &v, NULL);
      }
      assert( _mulle__structqueue_get_count( queue) == 8);
   }
}


static void   test_structarray_do_public( void)
{
   mulle_structarray_do( array, int)
   {
      int   v;
      int   i;

      for( i = 0; i < 8; i++)
      {
         v = i;
         mulle_structarray_add( array, &v);
      }
      assert( mulle_structarray_get_count( array) == 8);
   }
}


//
// the `_default` creation macros
//

static void   test_array_default( void)
{
   struct mulle_array   array;
   struct mulle_array   *created;
   void                 *item;
   int                  count;

   mulle_array_init_default( &array, &mulle_container_keycallback_copied_cstring);
   assert( mulle_array_get_count( &array) == 0);
   mulle_array_add( &array, "VfL");
   mulle_array_add( &array, "Bochum");

   count = 0;
   mulle_array_for( &array, item)
   {
      ++count;
   }
   assert( count == 2);
   mulle_array_done( &array);

   created = mulle_array_create_default( &mulle_container_keycallback_copied_cstring);
   assert( mulle_array_get_count( created) == 0);
   mulle_array_destroy( created);
}


static void   test_pointerarray_default( void)
{
   struct mulle_pointerarray   array;
   struct mulle_pointerarray   *created;

   _mulle_pointerarray_init_default( &array);
   assert( _mulle_pointerarray_get_count( &array) == 0);
   _mulle_pointerarray_done( &array);

   mulle_pointerarray_init_default( &array);
   assert( mulle_pointerarray_get_count( &array) == 0);
   mulle_pointerarray_done( &array);

   created = mulle_pointerarray_create_default();
   assert( mulle_pointerarray_get_count( created) == 0);
   mulle_pointerarray_destroy( created);
}


static void   test_structarray_default( void)
{
   struct mulle_structarray   array;
   struct mulle_structarray   *created;

   mulle_structarray_init_default( &array, int);
   assert( mulle_structarray_get_count( &array) == 0);
   mulle_structarray_done( &array);

   created = mulle_structarray_create_default( int);
   assert( mulle_structarray_get_count( created) == 0);
   mulle_structarray_destroy( created);
}


static void   test_pointerset_data( void)
{
   struct mulle_pointerset   set;

   set = MULLE_POINTERSET_DATA( NULL);
   assert( set.allocator == NULL);
   assert( mulle_pointerset_get_count( &set) == 0);
}


int   main( void)
{
   test_assoc_do();
   test_map_do();
   test_pointerset_do();
   test_rangemap_do();
   test_structarray_do();
   test_structarray_do_flexible();
   test_pointerpairarray_do();

   test_inner_array_do();
   test_inner_array_do_flexible();
   test_inner_pointerarray_do();
   test_inner_pointerpairarray_do();
   test_inner_set_do();
   test_inner_set_for();
   test_inner_structqueue_do();
   test_structarray_do_public();

   test_array_default();
   test_pointerarray_default();
   test_structarray_default();
   test_pointerset_data();

   return( 0);
}
