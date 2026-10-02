//
// Regression test for mulle_mapenumerator_next_pair (the public wrapper).
//
// The generic call returns a pointer to the `_space` member of the rover it
// was given, so the wrapper must hand back a pointer into the caller's rover
// (now `rover.base.base._space`, since each enumerator level nests the next).
// Returning a pointer into any intermediate local would hand back a pointer
// into a dead stack frame, which the caller then reads after unrelated stack
// activity has overwritten it.
//
#ifdef NDEBUG
# undef NDEBUG
#endif
#include <assert.h>
#include <string.h>

#include <mulle-container/mulle-container.h>


// Touch a chunk of stack so that a pointer into a returned function's frame
// (the old buggy behaviour) has been overwritten before it is dereferenced.
static void   clobber_stack( void)
{
   volatile char   buffer[ 512];
   size_t          i;

   for( i = 0; i < sizeof( buffer); i++)
      buffer[ i ] = (char) (i * 7 + 1);

   assert( buffer[ 0] == 1);
}


static int   pair_is_known( struct mulle_pointerpair *pair)
{
   if( ! pair || ! pair->key || ! pair->value)
      return( 0);

   return( ! strcmp( "VfL", pair->key)    ||
           ! strcmp( "Bochum", pair->key) ||
           ! strcmp( "1848", pair->key));
}


int   main( void)
{
   struct mulle_map                          *map;
   struct mulle_mapenumerator                rover;
   struct mulle_container_keyvaluecallback   callback;
   struct mulle_pointerpair                  *pair;
   char                                      *beg;
   char                                      *end;
   int                                       count;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   map = mulle_map_create( 0, &callback, NULL);
   mulle_map_set( map, "VfL", "VFL");
   mulle_map_set( map, "Bochum", "BOCHUM");
   mulle_map_set( map, "1848", "1848");

   beg = (char *) &rover;
   end = beg + sizeof( rover);

   count = 0;
   rover = mulle_map_enumerate( map);
   while( (pair = mulle_mapenumerator_next_pair( &rover)))
   {
      // the result must belong to the caller's rover, not to a dead local
      assert( (char *) pair >= beg && (char *) pair < end);
      assert( pair == &rover.base.base._space);

      // and it must survive unrelated stack activity
      clobber_stack();
      assert( pair_is_known( pair));

      ++count;
   }
   assert( count == 3);

   mulle_mapenumerator_done( &rover);
   mulle_map_destroy( map);

   return( 0);
}
