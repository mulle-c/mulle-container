//
//  array-aliasing-base.c
//
//  Direct regression test for the nested-member layout of the container
//  family, using the array as the representative. After the conversion
//
//      struct mulle_array             ->  struct mulle__array     base
//      struct mulle__array            ->  struct mulle__pointerarray base
//      struct mulle_arrayenumerator   ->  struct mulle__arrayenumerator base
//      struct mulle__arrayenumerator  ->  struct mulle__pointerarrayenumerator base
//
//  every level is a real member at offset 0, so the outer address, `&x.base`
//  and `&x.base.base` all denote the same object.
//
//  The failure this guards against: the old layout repeated the internal
//  fields by macro, which made `struct mulle_array` and `struct mulle__array`
//  distinct types of identical layout. Accessing one through a pointer to the
//  other is a strict-aliasing violation (C11 6.5p7) that GCC exploits at
//  -O2/-O3, silently corrupting state and under-enumerating. The public
//  wrappers therefore hand out `&array->base` / `&rover->base` instead of
//  casting, and this test drives both the nested body and the nested
//  enumerator through that API.
//
//  The hard guarantee is the layout: the `offsetof` asserts and the nested
//  member accesses below stop compiling if a level is removed or moved, i.e.
//  if the macro-repeated prefix comes back. The interleaved store/load and the
//  mixed inner/outer enumeration then exercise the access pattern the old
//  cast got wrong, under whatever optimization level the build uses.
//
#ifdef NDEBUG
# undef NDEBUG
#endif
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <mulle-container/mulle-container.h>


// One member per level, each at offset 0, so the addresses coincide.
_Static_assert( offsetof( struct mulle_array, base) == 0,
                "mulle_array.base must be the first member");
_Static_assert( offsetof( struct mulle__array, base) == 0,
                "mulle__array.base must be the first member");
_Static_assert( offsetof( struct mulle_arrayenumerator, base) == 0,
                "mulle_arrayenumerator.base must be the first member");
_Static_assert( offsetof( struct mulle__arrayenumerator, base) == 0,
                "mulle__arrayenumerator.base must be the first member");
_Static_assert( sizeof( struct mulle_array) >= sizeof( struct mulle__array),
                "mulle_array must be at least as large as its base");
_Static_assert( sizeof( struct mulle_arrayenumerator) >=
                sizeof( struct mulle__arrayenumerator),
                "mulle_arrayenumerator must be at least as large as its base");


// The innermost view: `struct mulle__pointerarray *`. These helpers are
// `static inline` so the optimizer reasons about the aliasing within one TU.
static inline void **   _pointerarray_get_curr( struct mulle__pointerarray *array)
{
   return( array->_curr);
}

static inline void   _pointerarray_set_curr( struct mulle__pointerarray *array,
                                             void **curr)
{
   array->_curr = curr;
}


static void   test_body_identity( void)
{
   struct mulle_array   array;
   struct mulle_array   *p;

   memset( &array, 0, sizeof( array));
   p = &array;

   assert( (void *) p == (void *) &p->base);
   assert( (void *) p == (void *) &p->base.base);
   assert( sizeof( *p) > sizeof( p->base));
}


static void   test_enumerator_identity( void)
{
   struct mulle_arrayenumerator   rover;
   struct mulle_arrayenumerator   *p;

   memset( &rover, 0, sizeof( rover));
   p = &rover;

   assert( (void *) p == (void *) &p->base);
   assert( (void *) p == (void *) &p->base.base);
   assert( p->base.base._curr == NULL);
   assert( p->base.base._sentinel == NULL);
}


//
// Store through the outer handle, read through the innermost type (and back).
// The interleaved stores matter: the last store must be observed by the
// differently-typed load, which is what a macro-repeated prefix would forbid.
//
static void   test_store_load_nested( void)
{
   struct mulle_array   array;

   // raw object: only the nested members are touched here
   memset( &array, 0, sizeof( array));

   // outer store, inner store, outer load
   array.base.base._storage = (void **) (uintptr_t) 0x10;
   _pointerarray_set_curr( &array.base.base, (void **) (uintptr_t) 0x30);
   assert( array.base.base._curr == (void **) (uintptr_t) 0x30);

   // inner store, outer store, inner load
   array.base.base._sentinel = (void **) (uintptr_t) 0x20;
   array.base.base._curr     = (void **) (uintptr_t) 0x50;
   assert( _pointerarray_get_curr( &array.base.base) == (void **) (uintptr_t) 0x50);
}


//
// The public `mulle_array_get_count` is `static inline` and computes through
// `&array->base`. Fabricate storage/curr through the outer handle and let the
// public accessor read the count, so a hoisted load shows up as a wrong
// result rather than a diff.
//
static void   test_public_accessor_through_base( void)
{
   struct mulle_array   array;
   void                 *storage[ 8];

   memset( &array, 0, sizeof( array));

   array.base.base._storage  = storage;
   array.base.base._initial_storage = storage;
   array.base.base._curr     = &storage[ 3];
   array.base.base._sentinel = &storage[ 8];

   assert( mulle_array_get_count( &array) == 3);
   assert( mulle_array_get_size( &array) >= 8);

   // the same state seen through the nested base
   assert( (size_t) ( array.base.base._curr - array.base.base._storage) ==
           mulle_array_get_count( &array));
}


//
// The actual -O2 symptom: the public enumerator and the inner one must share
// the same cursor when advanced alternately. If the wrappers punned the
// types, the traversal silently under-enumerates or repeats.
//
static void   test_enumerate_mixed( void)
{
   struct mulle_array             array;
   struct mulle_arrayenumerator   rover;
   void                           *item;

   mulle_array_init( &array, 0, &mulle_container_keycallback_nonowned_pointer,
                     NULL);
   mulle_array_add( &array, "VfL");
   mulle_array_add( &array, "Bochum");
   mulle_array_add( &array, "1848");

   assert( mulle_array_get_count( &array) == 3);

   rover = mulle_array_enumerate( &array);

   // the rover nests down to the same storage the array uses
   assert( rover.base.base._curr == array.base.base._storage);
   assert( rover.base.base._sentinel == array.base.base._curr);
   assert( (void *) &rover == (void *) &rover.base);
   assert( (void *) &rover == (void *) &rover.base.base);

   // advance through the *inner* type, observe through the outer one
   assert( mulle__arrayenumerator_next( &rover.base, &item));
   assert( ! strcmp( "VfL", item));
   assert( rover.base.base._curr == array.base.base._storage + 1);

   // and then through the public type, continuing the same cursor
   assert( mulle_arrayenumerator_next( &rover, &item));
   assert( ! strcmp( "Bochum", item));
   assert( mulle_arrayenumerator_next( &rover, &item));
   assert( ! strcmp( "1848", item));
   assert( ! mulle_arrayenumerator_next( &rover, &item));

   mulle_arrayenumerator_done( &rover);
   mulle_array_done( &array);
}


//
// Full traversal through the public API: every element exactly once. This is
// what silently broke at -O2 before the base member existed.
//
static void   test_enumerate_complete( void)
{
   struct mulle_array             array;
   struct mulle_arrayenumerator   rover;
   void                           *item;
   unsigned int                   seen;
   int                            count;

   mulle_array_init( &array, 0, &mulle_container_keycallback_nonowned_pointer,
                     NULL);
   mulle_array_add( &array, "VfL");
   mulle_array_add( &array, "Bochum");
   mulle_array_add( &array, "1848");

   seen  = 0;
   count = 0;
   rover = mulle_array_enumerate( &array);
   while( count < 8 && mulle_arrayenumerator_next( &rover, &item))
   {
      if( ! strcmp( "VfL", item))    seen |= 0x1;
      if( ! strcmp( "Bochum", item)) seen |= 0x2;
      if( ! strcmp( "1848", item))   seen |= 0x4;
      ++count;
   }
   assert( count == 3);
   assert( seen == 0x7);
   assert( mulle_arrayenumerator_next( &rover, &item) == 0);

   mulle_arrayenumerator_done( &rover);
   mulle_array_done( &array);
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
