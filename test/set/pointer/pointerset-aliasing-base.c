//
//  pointerset-aliasing-base.c
//
//  Direct regression test for the nested-member layout of the pointerset
//  family:
//
//      struct mulle_pointerset  ->  struct mulle__pointerset  base
//
//  `base` is a real member at offset 0, so `&set` and `&set->base` denote the
//  same object.
//
//  The failure this guards against: the old layout repeated the internal
//  fields by macro, making `struct mulle_pointerset` and
//  `struct mulle__pointerset` distinct types of identical layout. Accessing
//  one through a pointer to the other is a strict-aliasing violation
//  (C11 6.5p7) that GCC exploits at -O2/-O3. Two spots kept doing that:
//
//    - copy_storage_generic() passed raw storage cast to a set pointer, so
//      the mutation check read set->_n_mutations out of the first storage
//      slot. It now takes the real set.
//    - mulle__genericpointerset_for() cast the wrapper to the inner type. It
//      now walks &set->base like the public macro does.
//
//  This test exercises both: the grow/shrink paths that call
//  copy_storage_generic() and the generic enumeration macro, plus the nested
//  store/load pattern. Hashtable enumeration order is undefined, so it never
//  asserts an order, only the set of items seen and their count.
//
#ifdef NDEBUG
# undef NDEBUG
#endif
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <mulle-container/mulle-container.h>


_Static_assert( offsetof( struct mulle_pointerset, base) == 0,
                "mulle_pointerset.base must be the first member");
_Static_assert( offsetof( struct mulle__pointerset, _storage) == 0,
                "mulle__pointerset._storage must be the first member");
_Static_assert( sizeof( struct mulle_pointerset) >= sizeof( struct mulle__pointerset),
                "mulle_pointerset must be at least as large as its base");


// The innermost view: `struct mulle__pointerset *`. `static inline` so the
// optimizer reasons about the aliasing within one TU.
static inline size_t   _pointerset_get_count( struct mulle__pointerset *set)
{
   return( set->_count);
}

static inline void   _pointerset_set_count( struct mulle__pointerset *set,
                                            size_t count)
{
   set->_count = count;
}


static void   test_body_identity( void)
{
   struct mulle_pointerset   set;
   struct mulle_pointerset   *p;

   memset( &set, 0, sizeof( set));
   p = &set;

   assert( (void *) p == (void *) &p->base);
   assert( sizeof( *p) > sizeof( p->base));
}


//
// Store through the outer handle, read through the innermost type (and back).
// The interleaved stores matter: the last store must be observed by the
// differently-typed load, which is what a macro-repeated prefix would forbid.
//
static void   test_store_load_nested( void)
{
   struct mulle_pointerset   set;

   memset( &set, 0, sizeof( set));

   // outer store, inner store, outer load
   set.base._count = 1;
   _pointerset_set_count( &set.base, 3);
   assert( set.base._count == 3);

   // inner store, outer store, inner load
   set.base._storage = (void **) (uintptr_t) 0x10;
   set.base._count   = 5;
   assert( _pointerset_get_count( &set.base) == 5);
}


//
// The public `mulle_pointerset_get_count` is `static inline` and computes
// through `&set->base`. Fabricate the count through the outer handle and let
// the public accessor read it.
//
static void   test_public_accessor_through_base( void)
{
   struct mulle_pointerset   set;

   memset( &set, 0, sizeof( set));

   set.base._count = 3;
   assert( mulle_pointerset_get_count( &set) == 3);

   set.base._count = 0;
   assert( mulle_pointerset_get_count( &set) == 0);
}


static void  *make_pointer( int i)
{
   return( (void *) (uintptr_t) (0x10000 + i * 0x10));
}


//
// Drive enough inserts to force several grow() calls (each one runs the fixed
// copy_storage_generic), then remove enough to force shrink(). Every element
// must survive the moves and stay retrievable.
//
static void   test_grow_shrink( void)
{
   struct mulle_pointerset   *set;
   void                      *p;
   int                       i;
   int                       n;

   n   = 400;
   set = mulle_pointerset_create( NULL);
   assert( set != NULL);

   for( i = 0; i < n; i++)
   {
      p = make_pointer( i);
      assert( mulle_pointerset_insert( set, p) == 1);
   }

   assert( mulle_pointerset_get_count( set) == (size_t) n);
   assert( _pointerset_get_count( &set->base) == (size_t) n);

   for( i = 0; i < n; i++)
      assert( mulle_pointerset_get( set, make_pointer( i)) == make_pointer( i));

   // remove every other element, then force a shrink
   for( i = 0; i < n; i += 2)
      assert( mulle_pointerset_remove( set, make_pointer( i)) == 1);

   mulle_pointerset_shrink( set);

   assert( mulle_pointerset_get_count( set) == (size_t) (n / 2));
   for( i = 1; i < n; i += 2)
      assert( mulle_pointerset_get( set, make_pointer( i)) == make_pointer( i));
   for( i = 0; i < n; i += 2)
      assert( mulle_pointerset_get( set, make_pointer( i)) == NULL);

   mulle_pointerset_destroy( set);
}


//
// Decode the index a test pointer was built with, so an enumeration can be
// checked for "every item exactly once" without relying on hash order.
//
static size_t   item_index( void *item)
{
   uintptr_t   value;

   assert( item != NULL);

   value = (uintptr_t) item;
   assert( value >= 0x10000);

   return( (value - 0x10000) / 0x10);
}


static void   assert_all_seen( char seen[ 64], size_t count)
{
   int   i;

   assert( count == 64);
   for( i = 0; i < 64; i++)
      assert( seen[ i]);
}


static struct mulle_pointerset   *make_set_of_64( void)
{
   struct mulle_pointerset   *set;
   int                       i;

   set = mulle_pointerset_create( NULL);
   assert( set != NULL);

   for( i = 0; i < 64; i++)
      mulle_pointerset_insert( set, make_pointer( i));

   return( set);
}


//
// The fixed mulle__genericpointerset_for macro: it reaches the inner set
// through &(set)->base and must visit every element exactly once.
//
static void   test_generic_for( void)
{
   struct mulle_container_keycallback   *callback;
   struct mulle_pointerset             *set;
   void                                *item;
   char                                seen[ 64];
   size_t                              count;

   callback = &mulle_container_keycallback_nonowned_pointer_or_null;
   set      = make_set_of_64();

   memset( seen, 0, sizeof( seen));
   count = 0;
   mulle__genericpointerset_for( set, callback, item)
   {
      assert( seen[ item_index( item)] == 0);
      seen[ item_index( item)] = 1;
      ++count;
   }
   assert_all_seen( seen, count);

   mulle_pointerset_destroy( set);
}


//
// The public mulle_pointerset_for macro (reaches the inner set through
// &set->base).
//
static void   test_public_for( void)
{
   struct mulle_pointerset   *set;
   void                      *item;
   char                      seen[ 64];
   size_t                    count;

   set = make_set_of_64();

   memset( seen, 0, sizeof( seen));
   count = 0;
   mulle_pointerset_for( set, item)
   {
      assert( seen[ item_index( item)] == 0);
      seen[ item_index( item)] = 1;
      ++count;
   }
   assert_all_seen( seen, count);

   mulle_pointerset_destroy( set);
}


//
// The inner mulle__pointerset_for macro, called with &set->base.
//
static void   test_inner_for( void)
{
   struct mulle_pointerset   *set;
   void                      *item;
   char                      seen[ 64];
   size_t                    count;

   set = make_set_of_64();

   memset( seen, 0, sizeof( seen));
   count = 0;
   mulle__pointerset_for( &set->base, item)
   {
      assert( seen[ item_index( item)] == 0);
      seen[ item_index( item)] = 1;
      ++count;
   }
   assert_all_seen( seen, count);

   mulle_pointerset_destroy( set);
}


int   main( void)
{
   test_body_identity();
   test_store_load_nested();
   test_public_accessor_through_base();
   test_grow_shrink();
   test_generic_for();
   test_public_for();
   test_inner_for();

   return( 0);
}
