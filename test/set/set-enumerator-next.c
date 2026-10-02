//
// Regression test for the set enumerator memcpy wrappers.
//
// mulle__setenumerator_next and mulle_setenumerator_next forward the caller's
// rover through a local `struct mulle__genericpointersetenumerator tmp` and
// copy the traversal state back afterwards. A broken round-trip silently
// truncates the traversal, repeats elements or hands back an item that does
// not belong to the set, so drive both entry points over a known set and
// verify every item, with stack clobbering in between.
//
#ifdef NDEBUG
# undef NDEBUG
#endif
#include <assert.h>
#include <string.h>

#include <mulle-container/mulle-container.h>


// Touch a chunk of stack so a pointer left in a dead callee frame, or stale
// traversal state, can't survive unnoticed between iterations.
static void   clobber_stack( void)
{
   volatile char   buffer[ 512];
   size_t          i;

   for( i = 0; i < sizeof( buffer); i++)
      buffer[ i ] = (char) (i * 5 + 3);

   assert( buffer[ 0] == 3);
}


static unsigned int   item_bit( void *item)
{
   if( ! strcmp( "VfL", item))
      return( 0x1);
   if( ! strcmp( "Bochum", item))
      return( 0x2);
   if( ! strcmp( "1848", item))
      return( 0x4);
   return( 0);
}


// public entry point: mulle_set_enumerate / mulle_setenumerator_next
static void   check_public( struct mulle_set *set)
{
   struct mulle_setenumerator   rover;
   void                         *item;
   unsigned int                 state;
   int                          count;

   state = 0;
   rover = mulle_set_enumerate( set);

   count = 0;
   while( count < 8 && mulle_setenumerator_next( &rover, &item))
   {
      clobber_stack();

      // the item must belong to the set, not to the wrapper's dead frame
      assert( item);
      assert( mulle_set_get( set, item) == item);

      state |= item_bit( item);
      ++count;
   }
   assert( count == 3);
   assert( state == 0x7);

   mulle_setenumerator_done( &rover);
}


// internal entry point: _mulle_set_enumerate / _mulle_setenumerator_next
static void   check_internal( struct mulle_set *set)
{
   struct mulle_setenumerator   rover;
   void                         *item;
   unsigned int                 state;
   int                          count;

   state = 0;
   rover = _mulle_set_enumerate( set);

   count = 0;
   while( count < 8 && _mulle_setenumerator_next( &rover, &item))
   {
      clobber_stack();

      assert( item);
      assert( mulle_set_get( set, item) == item);

      state |= item_bit( item);
      ++count;
   }
   assert( count == 3);
   assert( state == 0x7);

   _mulle_setenumerator_done( &rover);
}


int   main( void)
{
   struct mulle_set   *set;

   set = mulle_set_create( 0, &mulle_container_keycallback_copied_cstring, NULL);

   mulle_set_set( set, "VfL");
   mulle_set_set( set, "Bochum");
   mulle_set_set( set, "1848");

   check_public( set);
   check_internal( set);

   mulle_set_destroy( set);

   return( 0);
}
