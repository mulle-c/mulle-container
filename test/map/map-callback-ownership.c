#ifdef NDEBUG
# undef NDEBUG
#endif
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mulle-container/mulle-container.h>


#define KEY_COUNT   64
#define STEP_COUNT  4000


struct ownership_stats
{
   size_t   key_retains;
   size_t   key_releases;
   size_t   key_live;
   size_t   value_retains;
   size_t   value_releases;
   size_t   value_live;
};


static uint32_t   next_random( uint32_t *state)
{
   *state = *state * UINT32_C( 1664525) + UINT32_C( 1013904223);
   return( *state);
}


static void   *copy_string( void *p)
{
   char    *copy;
   size_t   length;

   length = strlen( p) + 1;
   copy   = malloc( length);
   assert( copy);
   memcpy( copy, p, length);
   return( copy);
}


static void   *retain_key( struct mulle_container_keycallback *callback,
                           void *p,
                           struct mulle_allocator *allocator)
{
   struct ownership_stats   *stats;

   MULLE_C_UNUSED( allocator);
   stats = callback->userinfo;
   ++stats->key_retains;
   ++stats->key_live;
   return( copy_string( p));
}


static void   release_key( struct mulle_container_keycallback *callback,
                           void *p,
                           struct mulle_allocator *allocator)
{
   struct ownership_stats   *stats;

   MULLE_C_UNUSED( allocator);
   stats = callback->userinfo;
   assert( stats->key_live);
   ++stats->key_releases;
   --stats->key_live;
   free( p);
}


static void   *retain_value( struct mulle_container_valuecallback *callback,
                             void *p,
                             struct mulle_allocator *allocator)
{
   struct ownership_stats   *stats;

   MULLE_C_UNUSED( allocator);
   stats = callback->userinfo;
   ++stats->value_retains;
   ++stats->value_live;
   return( copy_string( p));
}


static void   release_value( struct mulle_container_valuecallback *callback,
                             void *p,
                             struct mulle_allocator *allocator)
{
   struct ownership_stats   *stats;

   MULLE_C_UNUSED( allocator);
   stats = callback->userinfo;
   assert( stats->value_live);
   ++stats->value_releases;
   --stats->value_live;
   free( p);
}


static void   test_randomized_ownership( void)
{
   struct mulle_container_keyvaluecallback   callback;
   struct ownership_stats                    stats;
   struct mulle_map                         *map;
   char                                      keys[ KEY_COUNT][ 16];
   char                                      values[ KEY_COUNT][ 16];
   unsigned char                             present[ KEY_COUNT];
   size_t                                    value_index[ KEY_COUNT];
   uint32_t                                  state;
   size_t                                    i;
   size_t                                    key_index;
   size_t                                    step;
   void                                     *value;

   memset( &stats, 0, sizeof( stats));
   memset( present, 0, sizeof( present));
   for( i = 0; i < KEY_COUNT; i++)
   {
      snprintf( keys[ i], sizeof( keys[ i]), "key-%zu", i);
      snprintf( values[ i], sizeof( values[ i]), "value-%zu", i);
   }

   callback.keycallback   = mulle_container_keycallback_nonowned_cstring;
   callback.valuecallback = mulle_container_valuecallback_nonowned_cstring;
   callback.keycallback.retain       = retain_key;
   callback.keycallback.release      = release_key;
   callback.keycallback.userinfo     = &stats;
   callback.valuecallback.retain     = retain_value;
   callback.valuecallback.release    = release_value;
   callback.valuecallback.userinfo   = &stats;

   map = mulle_map_create( 1, &callback, NULL);
   assert( map);
   state = UINT32_C( 0x6a09e667);

   for( step = 0; step < STEP_COUNT; step++)
   {
      key_index = next_random( &state) % KEY_COUNT;
      switch( next_random( &state) % 3)
      {
      case 0:
         value_index[ key_index] = next_random( &state) % KEY_COUNT;
         mulle_map_set( map, keys[ key_index], values[ value_index[ key_index]]);
         present[ key_index] = 1;
         break;
      case 1:
         mulle_map_remove( map, keys[ key_index]);
         present[ key_index] = 0;
         break;
      default:
         value = mulle_map_get( map, keys[ key_index]);
         if( present[ key_index])
            assert( ! strcmp( value, values[ value_index[ key_index]]));
         else
            assert( value == NULL);
         break;
      }
      assert( stats.key_live <= stats.key_retains);
      assert( stats.value_live <= stats.value_retains);
   }

   for( i = 0; i < KEY_COUNT; i++)
      if( present[ i])
         assert( mulle_map_contains( map, keys[ i]));
      else
         assert( ! mulle_map_contains( map, keys[ i]));

   mulle_map_destroy( map);
   assert( stats.key_live == 0);
   assert( stats.value_live == 0);
   assert( stats.key_retains == stats.key_releases);
   assert( stats.value_retains == stats.value_releases);
}


static void   test_boundary_growth( void)
{
   struct mulle_container_keyvaluecallback   callback;
   struct mulle_map                          *map;
   size_t                                    i;

   callback.keycallback   = mulle_container_keycallback_intptr;
   callback.valuecallback = mulle_container_valuecallback_intptr;
   map = mulle_map_create( 1, &callback, NULL);
   assert( map);
   for( i = 0; i < KEY_COUNT; i++)
      mulle_map_set( map,
                     (void *) (intptr_t) (i + 1),
                     (void *) (intptr_t) (i + 1001));
   assert( mulle_map_get_count( map) == KEY_COUNT);

   for( i = 0; i < KEY_COUNT; i += 2)
      mulle_map_remove( map, (void *) (intptr_t) (i + 1));
   assert( mulle_map_get_count( map) == KEY_COUNT / 2);

   for( i = 0; i < KEY_COUNT; i += 2)
      mulle_map_set( map,
                     (void *) (intptr_t) (i + 1),
                     (void *) (intptr_t) (i + 2001));
   assert( mulle_map_get_count( map) == KEY_COUNT);

   mulle_map_remove_all( map);
   assert( mulle_map_is_empty( map));
   mulle_map_destroy( map);
}


int   main( void)
{
   test_randomized_ownership();
   test_boundary_growth();
   return( 0);
}
