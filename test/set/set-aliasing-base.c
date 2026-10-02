//
//  set-aliasing-base.c
//
//  Direct regression test for the nested-member layout of the set family:
//
//      struct mulle_set                 ->  struct mulle__set      base
//      struct mulle__set                ->  struct mulle__pointerset base
//      struct mulle_setenumerator       ->  struct mulle__setenumerator base
//      struct mulle__setenumerator      ->  struct mulle__genericpointersetenumerator base
//
//  Every level is a real member at offset 0, so the outer address and each
//  nested `base` address denote the same object.
//
//  The failure this guards against: the old layout repeated the internal
//  fields by macro, making distinct types of identical layout. Accessing one
//  through a pointer to the other is a strict-aliasing violation (C11 6.5p7)
//  that GCC exploits at -O2/-O3, which silently under-enumerated the
//  container. The wrappers now hand out `&set->base` / `&rover->base` instead
//  of casting, and this test drives both the nested body and the nested
//  enumerator through that API.
//
//  Enumeration order of a hashtable is not defined, so the test asserts the
//  set of items seen and their count, never an order.
//
#ifdef NDEBUG
# undef NDEBUG
#endif
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <mulle-container/mulle-container.h>


_Static_assert( offsetof( struct mulle_set, base) == 0,
                "mulle_set.base must be the first member");
_Static_assert( offsetof( struct mulle__set, base) == 0,
                "mulle__set.base must be the first member");
_Static_assert( offsetof( struct mulle_setenumerator, base) == 0,
                "mulle_setenumerator.base must be the first member");
_Static_assert( offsetof( struct mulle__setenumerator, base) == 0,
                "mulle__setenumerator.base must be the first member");
_Static_assert( offsetof( struct mulle__genericpointersetenumerator, _curr) == 0,
                "genericpointersetenumerator._curr must be the first member");
_Static_assert( sizeof( struct mulle_set) >= sizeof( struct mulle__set),
                "mulle_set must be at least as large as its base");
_Static_assert( sizeof( struct mulle_setenumerator) >=
                sizeof( struct mulle__setenumerator),
                "mulle_setenumerator must be at least as large as its base");


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
   struct mulle_set   set;
   struct mulle_set   *p;

   memset( &set, 0, sizeof( set));
   p = &set;

   assert( (void *) p == (void *) &p->base);
   assert( (void *) p == (void *) &p->base.base);
   assert( sizeof( *p) > sizeof( p->base));
}


static void   test_enumerator_identity( void)
{
   struct mulle_setenumerator   rover;
   struct mulle_setenumerator   *p;

   memset( &rover, 0, sizeof( rover));
   p = &rover;

   assert( (void *) p == (void *) &p->base);
   assert( (void *) p == (void *) &p->base.base);
   assert( p->base.base._curr == NULL);
   assert( p->base.base._left == 0);
}


//
// Store through the outer handle, read through the innermost type (and back).
// The interleaved stores matter: the last store must be observed by the
// differently-typed load, which is what a macro-repeated prefix would forbid.
//
static void   test_store_load_nested( void)
{
   struct mulle_set   set;

   memset( &set, 0, sizeof( set));

   // outer store, inner store, outer load
   set.base.base._count = 1;
   _pointerset_set_count( &set.base.base, 3);
   assert( set.base.base._count == 3);

   // inner store, outer store, inner load
   set.base.base._storage = (void **) (uintptr_t) 0x10;
   set.base.base._count   = 5;
   assert( _pointerset_get_count( &set.base.base) == 5);
}


//
// The public `mulle_set_get_count` is `static inline` and computes through
// `&set->base`. Fabricate the count through the outer handle and let the
// public accessor read it.
//
static void   test_public_accessor_through_base( void)
{
   struct mulle_set   set;

   memset( &set, 0, sizeof( set));

   set.base.base._count = 3;
   assert( mulle_set_get_count( &set) == 3);

   set.base.base._count = 0;
   assert( mulle_set_get_count( &set) == 0);
}


static unsigned int   item_bit( void *item)
{
   if( ! item)
      return( 0);

   if( ! strcmp( "VfL", item))    return( 0x1);
   if( ! strcmp( "Bochum", item)) return( 0x2);
   if( ! strcmp( "1848", item))   return( 0x4);
   return( 0);
}


//
// The actual -O2 symptom: the public enumerator and the inner one must share
// the same cursor. Advance once through the middle level, then the rest
// through the public wrapper, and require every item exactly once.
//
static void   test_enumerate_mixed( void)
{
   struct mulle_set             *set;
   struct mulle_setenumerator   rover;
   void                         *item;
   unsigned int                 seen;
   int                          count;

   set = mulle_set_create( 0, &mulle_container_keycallback_copied_cstring, NULL);
   mulle_set_set( set, "VfL");
   mulle_set_set( set, "Bochum");
   mulle_set_set( set, "1848");

   assert( mulle_set_get_count( set) == 3);

   rover = mulle_set_enumerate( set);
   assert( (void *) &rover == (void *) &rover.base);
   assert( (void *) &rover == (void *) &rover.base.base);
   assert( rover.base.base._left == 3);

   seen  = 0;
   count = 0;

   // advance through the middle level and observe the shared nested cursor
   assert( mulle__setenumerator_next( &rover.base, &item));
   seen |= item_bit( item);
   ++count;
   assert( rover.base.base._left == 2);

   // continue through the public wrapper over the same nested cursor
   while( count < 8 && mulle_setenumerator_next( &rover, &item))
   {
      seen |= item_bit( item);
      ++count;
   }

   assert( count == 3);
   assert( seen == 0x7);

   mulle_setenumerator_done( &rover);
   mulle_set_destroy( set);
}


//
// Full traversal through the public API: every item exactly once, and the
// nested cursor is exhausted at the end.
//
static void   test_enumerate_complete( void)
{
   struct mulle_set             *set;
   struct mulle_setenumerator   rover;
   void                         *item;
   unsigned int                 seen;
   int                          count;

   set = mulle_set_create( 0, &mulle_container_keycallback_copied_cstring, NULL);
   mulle_set_set( set, "VfL");
   mulle_set_set( set, "Bochum");
   mulle_set_set( set, "1848");

   seen  = 0;
   count = 0;
   rover = mulle_set_enumerate( set);
   while( count < 8 && mulle_setenumerator_next( &rover, &item))
   {
      seen |= item_bit( item);
      ++count;
   }
   assert( count == 3);
   assert( seen == 0x7);
   assert( rover.base.base._left == 0);

   mulle_setenumerator_done( &rover);
   mulle_set_destroy( set);
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
