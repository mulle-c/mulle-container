//
//  map-aliasing-base.c
//
//  Direct regression test for the nested-member layout of the map family:
//
//      struct mulle_map                  ->  struct mulle__map             base
//      struct mulle__map                 ->  struct mulle__pointermap     base
//      struct mulle_mapenumerator        ->  struct mulle__mapenumerator  base
//      struct mulle__mapenumerator       ->  struct mulle__genericpointermapenumerator base
//      struct mulle__genericpointermapenumerator
//                                        ->  struct mulle__pointermapenumerator _base
//
//  Every level is a real member at offset 0, so the outer address and each
//  nested `base` address denote the same object.
//
//  The failure this guards against: the old layout repeated the internal
//  fields by macro, making distinct types of identical layout. Accessing one
//  through a pointer to the other is a strict-aliasing violation (C11 6.5p7)
//  that GCC exploits at -O2/-O3, which silently under-enumerated the
//  container. The wrappers now hand out `&map->base` / `&rover->base` instead
//  of casting, and this test drives both the nested body and the nested
//  enumerator through that API.
//
//  Enumeration order of a hashtable is not defined, so the test asserts the
//  set of keys seen and their count, never an order.
//
#ifdef NDEBUG
# undef NDEBUG
#endif
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <mulle-container/mulle-container.h>


_Static_assert( offsetof( struct mulle_map, base) == 0,
                "mulle_map.base must be the first member");
_Static_assert( offsetof( struct mulle__map, base) == 0,
                "mulle__map.base must be the first member");
_Static_assert( offsetof( struct mulle_mapenumerator, base) == 0,
                "mulle_mapenumerator.base must be the first member");
_Static_assert( offsetof( struct mulle__mapenumerator, base) == 0,
                "mulle__mapenumerator.base must be the first member");
_Static_assert( offsetof( struct mulle__genericpointermapenumerator, _base) == 0,
                "genericpointermapenumerator._base must be the first member");
_Static_assert( sizeof( struct mulle_map) >= sizeof( struct mulle__map),
                "mulle_map must be at least as large as its base");
_Static_assert( sizeof( struct mulle_mapenumerator) >=
                sizeof( struct mulle__mapenumerator),
                "mulle_mapenumerator must be at least as large as its base");


// The innermost view: `struct mulle__pointermap *`. `static inline` so the
// optimizer reasons about the aliasing within one TU.
static inline size_t   _pointermap_get_count( struct mulle__pointermap *map)
{
   return( map->_count);
}

static inline void   _pointermap_set_count( struct mulle__pointermap *map,
                                            size_t count)
{
   map->_count = count;
}


static void   test_body_identity( void)
{
   struct mulle_map   map;
   struct mulle_map   *p;

   memset( &map, 0, sizeof( map));
   p = &map;

   assert( (void *) p == (void *) &p->base);
   assert( (void *) p == (void *) &p->base.base);
   assert( sizeof( *p) > sizeof( p->base));
}


static void   test_enumerator_identity( void)
{
   struct mulle_mapenumerator   rover;
   struct mulle_mapenumerator   *p;

   memset( &rover, 0, sizeof( rover));
   p = &rover;

   assert( (void *) p == (void *) &p->base);
   assert( (void *) p == (void *) &p->base.base);
   assert( p->base.base._base._curr == NULL);
   assert( p->base.base._base._left == 0);
}


//
// Store through the outer handle, read through the innermost type (and back).
// The interleaved stores matter: the last store must be observed by the
// differently-typed load, which is what a macro-repeated prefix would forbid.
//
static void   test_store_load_nested( void)
{
   struct mulle_map   map;

   memset( &map, 0, sizeof( map));

   // outer store, inner store, outer load
   map.base.base._count = 1;
   _pointermap_set_count( &map.base.base, 3);
   assert( map.base.base._count == 3);

   // inner store, outer store, inner load
   map.base.base._storage = (void **) (uintptr_t) 0x10;
   map.base.base._count   = 5;
   assert( _pointermap_get_count( &map.base.base) == 5);
}


//
// The public `mulle_map_get_count` / `mulle_map_is_empty` are `static inline`
// and compute through `&map->base`. Fabricate the count through the outer
// handle and let the public accessors read it.
//
static void   test_public_accessor_through_base( void)
{
   struct mulle_map   map;

   memset( &map, 0, sizeof( map));

   map.base.base._count = 3;
   assert( mulle_map_get_count( &map) == 3);
   assert( mulle_map_is_empty( &map) == 0);

   map.base.base._count = 0;
   assert( mulle_map_is_empty( &map) == 1);
}


static unsigned int   pair_bit( struct mulle_pointerpair *pair)
{
   if( ! pair || ! pair->key)
      return( 0);

   if( ! strcmp( "VfL", pair->key))    return( 0x1);
   if( ! strcmp( "Bochum", pair->key)) return( 0x2);
   if( ! strcmp( "1848", pair->key))   return( 0x4);
   return( 0);
}


//
// The actual -O2 symptom: the public enumerator and the inner ones must share
// the same cursor. Advance once through the middle level, then the rest
// through the public wrapper, and require every key exactly once.
//
static void   test_enumerate_mixed( void)
{
   struct mulle_map                          *map;
   struct mulle_mapenumerator                rover;
   struct mulle_container_keyvaluecallback   callback;
   struct mulle_pointerpair                  *pair;
   unsigned int                              seen;
   int                                       count;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   map = mulle_map_create( 0, &callback, NULL);
   mulle_map_set( map, "VfL", "VFL");
   mulle_map_set( map, "Bochum", "BOCHUM");
   mulle_map_set( map, "1848", "1848");

   assert( mulle_map_get_count( map) == 3);

   rover = mulle_map_enumerate( map);
   assert( (void *) &rover == (void *) &rover.base);
   assert( (void *) &rover == (void *) &rover.base.base);
   assert( rover.base.base._base._left == 3);

   seen  = 0;
   count = 0;

   // advance through the middle level, item must belong to this rover
   pair = mulle__mapenumerator_next_pair( &rover.base);
   if( pair)
   {
      assert( pair == &rover.base.base._space);
      seen |= pair_bit( pair);
      ++count;
      assert( rover.base.base._base._left == 2);
   }

   // continue through the public wrapper over the same nested cursor
   while( count < 8 && (pair = mulle_mapenumerator_next_pair( &rover)))
   {
      seen |= pair_bit( pair);
      ++count;
   }

   assert( count == 3);
   assert( seen == 0x7);

   mulle_mapenumerator_done( &rover);
   mulle_map_destroy( map);
}


//
// Full traversal through the public API: every key exactly once, and the
// nested cursor is exhausted at the end.
//
static void   test_enumerate_complete( void)
{
   struct mulle_map                          *map;
   struct mulle_mapenumerator                rover;
   struct mulle_container_keyvaluecallback   callback;
   struct mulle_pointerpair                  *pair;
   unsigned int                              seen;
   int                                       count;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   map = mulle_map_create( 0, &callback, NULL);
   mulle_map_set( map, "VfL", "VFL");
   mulle_map_set( map, "Bochum", "BOCHUM");
   mulle_map_set( map, "1848", "1848");

   seen  = 0;
   count = 0;
   rover = mulle_map_enumerate( map);
   while( count < 8 && (pair = mulle_mapenumerator_next_pair( &rover)))
   {
      seen |= pair_bit( pair);
      ++count;
   }
   assert( count == 3);
   assert( seen == 0x7);
   assert( rover.base.base._base._left == 0);
   assert( mulle_mapenumerator_next_pair( &rover) == NULL);

   mulle_mapenumerator_done( &rover);
   mulle_map_destroy( map);
}


int   main( void)
{
   test_body_identity();
   test_enumerator_identity();
   test_store_load_nested();
   test_public_accessor_through_base();
   test_enumerate_mixed();
   test_enumerate_complete();

   return( 0);
}
