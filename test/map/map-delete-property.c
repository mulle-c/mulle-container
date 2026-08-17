#ifdef NDEBUG
# undef NDEBUG
#endif
#include <assert.h>
#include <stdint.h>
#include <string.h>

#include <mulle-container/mulle-container.h>


#define KEY_COUNT   97
#define STEP_COUNT  5000


static uint32_t   next_random( uint32_t *state)
{
   *state = *state * UINT32_C( 1664525) + UINT32_C( 1013904223);
   return( *state);
}


static void   check_map( struct mulle_map *map,
                         unsigned char *present,
                         intptr_t *values)
{
   struct mulle_mapenumerator                rover;
   unsigned char                             seen[ KEY_COUNT];
   void                                     *key;
   void                                     *value;
   size_t                                    count;
   size_t                                    i;
   intptr_t                                  index;

   memset( seen, 0, sizeof( seen));
   count = 0;
   for( i = 0; i < KEY_COUNT; i++)
   {
      key = (void *) (intptr_t) i;
      assert( mulle_map_contains( map, key) == present[ i]);
      value = mulle_map_get( map, key);
      if( present[ i])
         assert( (intptr_t) value == values[ i]);
      else
         assert( value == NULL);
      count += present[ i] != 0;
   }
   assert( mulle_map_get_count( map) == count);

   rover = mulle_map_enumerate( map);
   while( mulle_mapenumerator_next( &rover, &key, &value))
   {
      index = (intptr_t) key;
      assert( index >= 0 && index < KEY_COUNT);
      assert( present[ index]);
      assert( ! seen[ index]);
      assert( (intptr_t) value == values[ index]);
      seen[ index] = 1;
   }
   mulle_mapenumerator_done( &rover);

   for( i = 0; i < KEY_COUNT; i++)
      assert( seen[ i] == present[ i]);
}


static void   check_set( struct mulle_set *set,
                         unsigned char *present)
{
   struct mulle_setenumerator   rover;
   unsigned char                 seen[ KEY_COUNT];
   void                         *key;
   size_t                        count;
   size_t                        i;
   intptr_t                      index;

   memset( seen, 0, sizeof( seen));
   count = 0;
   for( i = 0; i < KEY_COUNT; i++)
   {
      key = (void *) (intptr_t) i;
      assert( mulle_set_member( set, key) == present[ i]);
      assert( (mulle_set_get( set, key) != mulle_set_get_keycallback( set)->notakey) == present[ i]);
      count += present[ i] != 0;
   }
   assert( mulle_set_get_count( set) == count);

   rover = mulle_set_enumerate( set);
   while( mulle_setenumerator_next( &rover, &key))
   {
      index = (intptr_t) key;
      assert( index >= 0 && index < KEY_COUNT);
      assert( present[ index]);
      assert( ! seen[ index]);
      seen[ index] = 1;
   }
   mulle_setenumerator_done( &rover);

   for( i = 0; i < KEY_COUNT; i++)
      assert( seen[ i] == present[ i]);
}


static void   test_map_deletion( void)
{
   struct mulle_container_keyvaluecallback   callback;
   struct mulle_map                         *map;
   unsigned char                             present[ KEY_COUNT];
   intptr_t                                  values[ KEY_COUNT];
   uint32_t                                  state;
   size_t                                    i;
   size_t                                    key_index;
   size_t                                    step;
   void                                     *key;
   void                                     *value;

   callback.keycallback   = mulle_container_keycallback_intptr;
   callback.valuecallback = mulle_container_valuecallback_intptr;
   map = mulle_map_create( 1, &callback, NULL);
   assert( map);
   memset( present, 0, sizeof( present));
   memset( values, 0, sizeof( values));
   state = UINT32_C( 0x9e3779b9);

   for( step = 0; step < STEP_COUNT; step++)
   {
      key_index = next_random( &state) % KEY_COUNT;
      key = (void *) (intptr_t) key_index;
      switch( next_random( &state) % 3)
      {
      case 0:
         value = (void *) (intptr_t) (step + 1);
         mulle_map_set( map, key, value);
         present[ key_index] = 1;
         values[ key_index] = (intptr_t) value;
         break;
      case 1:
         mulle_map_remove( map, key);
         present[ key_index] = 0;
         values[ key_index] = 0;
         break;
      default:
         value = mulle_map_get( map, key);
         if( present[ key_index])
            assert( (intptr_t) value == values[ key_index]);
         else
            assert( value == NULL);
         break;
      }

      if( (step & 0x1f) == 0)
         check_map( map, present, values);
   }
   check_map( map, present, values);

   for( i = 0; i < KEY_COUNT; i++)
      mulle_map_remove( map, (void *) (intptr_t) i);
   memset( present, 0, sizeof( present));
   memset( values, 0, sizeof( values));
   check_map( map, present, values);
   mulle_map_destroy( map);
}


static void   test_set_deletion( void)
{
   struct mulle_set                         *set;
   unsigned char                             present[ KEY_COUNT];
   uint32_t                                  state;
   size_t                                    key_index;
   size_t                                    step;
   void                                     *key;

   set = mulle_set_create( 1, &mulle_container_keycallback_intptr, NULL);
   assert( set);
   memset( present, 0, sizeof( present));
   state = UINT32_C( 0x243f6a88);

   for( step = 0; step < STEP_COUNT; step++)
   {
      key_index = next_random( &state) % KEY_COUNT;
      key = (void *) (intptr_t) key_index;
      switch( next_random( &state) % 3)
      {
      case 0:
         mulle_set_set( set, key);
         present[ key_index] = 1;
         break;
      case 1:
         mulle_set_remove( set, key);
         present[ key_index] = 0;
         break;
      default:
         assert( mulle_set_member( set, key) == present[ key_index]);
         break;
      }

      if( (step & 0x1f) == 0)
         check_set( set, present);
   }
   check_set( set, present);

   mulle_set_reset( set);
   memset( present, 0, sizeof( present));
   check_set( set, present);
   mulle_set_destroy( set);
}


int   main( void)
{
   test_map_deletion();
   test_set_deletion();
   return( 0);
}
