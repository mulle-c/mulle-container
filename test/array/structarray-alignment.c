//
// Test that structarray properly aligns elements even when sizeof != alignof multiple.
// Regression test for: MULLE__STRUCTARRAY_ALIGNED_SIZE was sizeof+sizeof%alignof
// instead of proper round-up.
//
#include <mulle-container/mulle-container.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>


// A struct where sizeof is already a multiple of alignof (common case).
// This should always work.
struct aligned_s
{
   double   x;     // size=8, align=8
};


// Use the static-storage initializer to verify sentinel calculation.
// Regression test for: sentinel was &storage[count] instead of
// &storage[count * sizeof_struct].
static void   test_static_storage( void)
{
   struct aligned_s          buf[4];
   struct mulle__structarray array;

   array = MULLE__STRUCTARRAY_DATA( buf, struct aligned_s, 4);

   // Sentinel should be 4 * sizeof(struct aligned_s) bytes from start
   printf( "static storage sentinel ok: %s\n",
           (char *) array._sentinel == (char *) buf + 4 * sizeof( struct aligned_s)
              ? "YES" : "NO");
   printf( "sizeof_struct: %lu\n", (unsigned long) array._sizeof_struct);
   printf( "expected: %lu\n", (unsigned long) sizeof( struct aligned_s));
}


// Test that runtime init computes the same stride as the macro
static void   test_runtime_init( void)
{
   struct mulle__structarray   array;

   _mulle__structarray_init( &array,
                             sizeof( struct aligned_s),
                             alignof( struct aligned_s),
                             4,
                             NULL);

   printf( "runtime sizeof_struct: %lu\n", (unsigned long) array._sizeof_struct);
   printf( "matches macro: %s\n",
           array._sizeof_struct == MULLE__STRUCTARRAY_ALIGNED_SIZE( struct aligned_s)
              ? "YES" : "NO");

   // Add elements and verify they're at correct offsets
   struct aligned_s   *p0;
   struct aligned_s   *p1;
   struct aligned_s   val = { 3.14 };

   p0 = _mulle__structarray_advance( &array, 1, NULL);
   memcpy( p0, &val, sizeof( val));
   val.x = 2.71;
   p1 = _mulle__structarray_advance( &array, 1, NULL);
   memcpy( p1, &val, sizeof( val));

   printf( "element stride correct: %s\n",
           (char *) p1 - (char *) p0 == (ptrdiff_t) array._sizeof_struct
              ? "YES" : "NO");

   _mulle__structarray_done( &array, NULL);
}


// Test static storage with runtime init path
static void   test_static_storage_runtime( void)
{
   struct aligned_s          buf[8];
   struct mulle__structarray array;

   _mulle__structarray_init_with_static_storage( &array,
                                                 sizeof( struct aligned_s),
                                                 alignof( struct aligned_s),
                                                 8,
                                                 buf);

   printf( "static runtime sentinel ok: %s\n",
           (char *) array._sentinel == (char *) buf + 8 * sizeof( struct aligned_s)
              ? "YES" : "NO");

   // Fill all 8 slots
   for( int i = 0; i < 8; i++)
   {
      struct aligned_s   val = { (double) i };
      struct aligned_s   *p = _mulle__structarray_advance( &array, 1, NULL);
      memcpy( p, &val, sizeof( val));
   }

   printf( "count after fill: %lu\n",
           (unsigned long) _mulle__structarray_get_count( &array));

   _mulle__structarray_done( &array, NULL);
}


int   main( void)
{
   test_static_storage();
   test_runtime_init();
   test_static_storage_runtime();
   return( 0);
}
