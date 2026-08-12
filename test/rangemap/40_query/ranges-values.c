#include <mulle-container/mulle-container.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>


int   main( void)
{
   struct mulle__rangemap   map;
   struct mulle_range       ranges[ 8];
   void                     *values[ 8];
   struct mulle_range       range;
   uintptr_t                n;
   uintptr_t                i;
   void                     *value;

   _mulle__rangemap_init( &map, 8, NULL);

   // populate: [10,15) [20,25) [30,35)
   _mulle__rangemap_insert( &map, mulle_range_make( 10, 5), (void *) 0x1, NULL);
   _mulle__rangemap_insert( &map, mulle_range_make( 20, 5), (void *) 0x2, NULL);
   _mulle__rangemap_insert( &map, mulle_range_make( 30, 5), (void *) 0x3, NULL);

   printf( "== get_ranges_values ==\n");

   // invalid (zero length) range
   n = _mulle__rangemap_get_ranges_values( &map, mulle_range_make( 10, 0), 8, ranges, values);
   printf( "invalid range: %lu\n", (unsigned long) n);

   // empty map
   _mulle__rangemap_reset( &map, NULL);
   n = _mulle__rangemap_get_ranges_values( &map, mulle_range_make( 10, 5), 8, ranges, values);
   printf( "empty map: %lu\n", (unsigned long) n);

   // repopulate
   _mulle__rangemap_insert( &map, mulle_range_make( 10, 5), (void *) 0x1, NULL);
   _mulle__rangemap_insert( &map, mulle_range_make( 20, 5), (void *) 0x2, NULL);
   _mulle__rangemap_insert( &map, mulle_range_make( 30, 5), (void *) 0x3, NULL);

   // query range intersecting all three: [10,35)
   n = _mulle__rangemap_get_ranges_values( &map, mulle_range_make( 10, 25), 8, ranges, values);
   printf( "all intersecting: %lu\n", (unsigned long) n);
   for( i = 0; i < n; i++)
      printf( "  %lu: %lu,%lu -> 0x%lx\n",
              (unsigned long) i,
              (unsigned long) ranges[ i].location,
              (unsigned long) ranges[ i].length,
              (unsigned long) (uintptr_t) values[ i]);

   // max_out smaller than the number of intersecting ranges
   n = _mulle__rangemap_get_ranges_values( &map, mulle_range_make( 10, 25), 2, ranges, values);
   printf( "max_out 2 returned: %lu, wrote %lu,%lu\n",
           (unsigned long) n,
           (unsigned long) ranges[ 0].location,
           (unsigned long) ranges[ 1].location);

   // partial intersection: [10,15) only
   n = _mulle__rangemap_get_ranges_values( &map, mulle_range_make( 10, 5), 8, ranges, values);
   printf( "single: %lu\n", (unsigned long) n);

   // non-intersecting query
   n = _mulle__rangemap_get_ranges_values( &map, mulle_range_make( 40, 5), 8, ranges, values);
   printf( "non-intersecting: %lu\n", (unsigned long) n);

   // NULL out_ranges
   n = _mulle__rangemap_get_ranges_values( &map, mulle_range_make( 10, 25), 8, NULL, values);
   printf( "NULL out_ranges: %lu, values[ 0]=0x%lx\n",
           (unsigned long) n,
           (unsigned long) (uintptr_t) values[ 0]);

   // NULL out_values
   n = _mulle__rangemap_get_ranges_values( &map, mulle_range_make( 10, 25), 8, ranges, NULL);
   printf( "NULL out_values: %lu, ranges[ 0]=%lu,%lu\n",
           (unsigned long) n,
           (unsigned long) ranges[ 0].location,
           (unsigned long) ranges[ 0].length);

   printf( "== get_exact ==\n");

   // exact match
   value = _mulle__rangemap_get_exact( &map, mulle_range_make( 20, 5));
   printf( "exact: 0x%lx\n", (unsigned long) (uintptr_t) value);

   // not found
   errno = 0;
   value = _mulle__rangemap_get_exact( &map, mulle_range_make( 40, 5));
   printf( "missing: 0x%lx errno=%d (%s)\n",
           (unsigned long) (uintptr_t) value, errno, strerror( errno));

   // intersecting but not equal
   errno = 0;
   value = _mulle__rangemap_get_exact( &map, mulle_range_make( 20, 4));
   printf( "overlap: 0x%lx errno=%d (%s)\n",
           (unsigned long) (uintptr_t) value, errno, strerror( errno));

   printf( "== remove invalid ==\n");

   // zero length range
   n = _mulle__rangemap_remove( &map, mulle_range_make( 10, 0), NULL);
   printf( "remove zero length: %lu\n", (unsigned long) n);

   // invalid range (location beyond max)
   n = _mulle__rangemap_remove( &map, mulle_range_make( mulle_not_found_e, 5), NULL);
   printf( "remove invalid location: %lu\n", (unsigned long) n);

   _mulle__rangemap_done( &map, NULL);

   return( 0);
}
