#include "mulle--rangeset.h"

#include "include-private.h"

#include "mulle-container-math.h"


#include <string.h>



#if defined( MULLE_TEST) && ! defined( NDEBUG)

#include <assert.h>


static void   _mulle__rangeset_assert( struct mulle__rangeset *set)
{
   struct mulle_range   *curr;
   struct mulle_range   *old;
   struct mulle_range   *sentinel;

   old      = NULL;
   curr     = &set->_ranges[ 0];
   sentinel = &set->_ranges[ set->_length];
   while( curr < sentinel)
   {
      assert( curr->length);
      mulle_range_is_valid( *curr);
      if( old)
         assert( mulle_range_get_end( *old) <= curr->location);

      old = curr;
      ++curr;
   }
}

#else

static inline void   _mulle__rangeset_assert( struct mulle__rangeset *set)
{
}

#endif


void   _mulle__rangeset_grow( struct mulle__rangeset *p,
                             struct mulle_allocator *allocator)
{
   uintptr_t   new_size;

   new_size = p->size * 2;
   new_size = mulle_pow2round( new_size);
   if( new_size == 0)
      new_size = 2;

   p->_ranges = mulle_allocator_realloc( allocator, p->_ranges, sizeof( struct mulle_range) * new_size);
//   memset( &p->_ranges[ p->size], 0, sizeof( struct mulle_range) * (new_size - p->size));
   p->size  = new_size;
}


void   _mulle__rangeset_reserve( struct mulle__rangeset *p,
                                uintptr_t additional,
                                struct mulle_allocator *allocator)
{
   for(;;)
   {
      if( p->_length + additional <= p->size)
         return;

      _mulle__rangeset_grow( p, allocator);
   }
}


void   _mulle__rangeset_shrink( struct mulle__rangeset *p,
                               struct mulle_allocator *allocator)
{
   uintptr_t   new_size;
   uintptr_t   size;

   size = p->size;
   do
   {
      new_size = size;
      size     = new_size / 2;
   }
   while( size >= p->_length);

   if( new_size >= p->size)
      return;

   p->_ranges = mulle_allocator_realloc_strict( allocator,
                                                p->_ranges,
                                                sizeof( struct mulle_range) * new_size);
   p->size   = new_size;
}


void   _mulle__rangeset_reset( struct mulle__rangeset *p,
                              struct mulle_allocator *allocator)
{
   p->_length = 0;
   _mulle__rangeset_shrink( p, allocator);
}



int   _mulle__rangeset_contains( struct mulle__rangeset *p,
                                struct mulle_range range)
{
   struct mulle_range   *found;

   if( ! range.length)
      return( 0);
   if( ! p->_length)
      return( 0);

   found = mulle_range_contains_bsearch( p->_ranges, p->_length, range);
   return( found ? 1 : 0);
}


int   _mulle__rangeset_intersects( struct mulle__rangeset *p,
                                  struct mulle_range range)
{
   struct mulle_range   *found;

   if( ! range.length)
      return( 0);
   if( ! p->_length)
      return( 0);

   found = mulle_range_intersects_bsearch( p->_ranges, p->_length, range);
   return( found ? 1 : 0);
}




// low level stuff, doesn't sort, doesn't check

static void   __mulle__rangeset_move_down_pointer( struct mulle__rangeset *p,
                                                  struct mulle_range *curr,
                                                  struct mulle_allocator *allocator)
{
   uintptr_t   n;

   n = &p->_ranges[ p->_length] - curr;
   if( n > 1)
      memmove( curr, curr + 1, (n - 1) * sizeof( struct mulle_range));
   --p->_length;
}


static inline void
   __mulle__rangeset_move_down( struct mulle__rangeset *p,
                                       uintptr_t index,
                                       struct mulle_allocator *allocator)
{
   __mulle__rangeset_move_down_pointer( p, &p->_ranges[ index], allocator);
}


static void   __mulle__rangeset_move_up( struct mulle__rangeset *p,
                                        uintptr_t index,
                                        struct mulle_allocator *allocator)
{
   uintptr_t            n;
   struct mulle_range   *curr;

   // this can change curr and p->_ranges
   _mulle__rangeset_reserve( p, 1, allocator);

   curr = &p->_ranges[ index];
   n    = &p->_ranges[ p->_length] - curr;
   if( n)
      memmove( curr + 1, curr, n * sizeof( struct mulle_range));
   ++p->_length;
}


static void   __mulle__rangeset_insert_known_absent( struct mulle__rangeset *p,
                                                    struct mulle_range range,
                                                    struct mulle_allocator *allocator)
{
   struct mulle_range   *prev;
   struct mulle_range   *next;
   uintptr_t            index;
   uintptr_t            n;

   // we don't store empty ranges
   if( ! range.length)
      return;

   index = mulle_range_hole_bsearch( p->_ranges, p->_length, range.location);

   // we know that we don't intersect, but maybe we can combine ?
   // check the previous and the next for combine possibilities
   if( p->_length)
   {
      prev = NULL;
      next = NULL;

      if( index)
      {
         prev = &p->_ranges[ index - 1];
         if( mulle_range_get_end( *prev) == range.location)
            prev->length += range.length;
         else
            prev = NULL;
      }

      if( index < p->_length)
      {
         next = &p->_ranges[ index];
         if( mulle_range_get_end( range) == next->location)
         {
            if( prev && mulle_range_get_end( *prev) == next->location)
            {
               prev->length += next->length;
               __mulle__rangeset_move_down_pointer( p, prev + 1, allocator);
            }
            else
            {
               next->location -= range.length;
               next->length   += range.length;
            }

            // done if we did next
            return;
         }
      }

      // done if we did prev
      if( prev)
         return;
   }

   __mulle__rangeset_move_up( p, index, allocator);

   p->_ranges[ index] = range;

   _mulle__rangeset_assert( p);
}


void   __mulle__rangeset_insert( struct mulle__rangeset *p,
                                 struct mulle_range range,
                                 struct mulle_allocator *allocator)
{
   struct mulle_range   *prev;
   struct mulle_range   *sentinel;
   struct mulle_range   *next;
   struct mulle_range   *found;
   struct mulle_range   *curr;
   uintptr_t            location;
   uintptr_t            length;

   // we don't store empty ranges
   if( ! range.length)
      return;

   _mulle__rangeset_assert( p);

   found = mulle_range_intersects_bsearch( p->_ranges, p->_length, range);
   if( ! found)
   {
      __mulle__rangeset_insert_known_absent( p, range, allocator);
      return;
   }

   // we have four cases
   // 1. complete overlap -> nothing to do:
   // 2. overflow in the front (possibly coalesce with previous and shrink)
   // 3. overflow in the back (possibly coalesce with next and shrink)

_1:
   if( mulle_range_contains( *found, range))
      return;

_2:
   if( range.location < found->location)
   {
      curr     = found;
      sentinel = &p->_ranges[ 0];
loop_2:
      prev = curr - 1;
      if( prev < sentinel || ! mulle_range_intersect( *prev, range).length)
      {
         // easy, just change current front and adjust length
         if( range.location < curr->location)
         {
            curr->length  += curr->location - range.location;
            curr->location = range.location;
         }
         goto _3;
      }

      // prev extends over curr, and remove curr
      // add length of hole and then curr->length
      prev->length  += curr->location - mulle_range_get_end( *prev) + curr->length;
      __mulle__rangeset_move_down_pointer( p, curr, allocator);
      curr           = prev;
      found          = prev;  // found is now really prev
      goto loop_2;  // could overlap more in front, so go back
   }

 _3:
   if( mulle_range_get_end( range) > mulle_range_get_end( *found))
   {
      curr     = found;
loop_3:
      sentinel = &p->_ranges[ p->_length];
      next     = curr + 1;
      if( next >= sentinel || ! mulle_range_intersect( *next, range).length)
      {
         // easy, just change current end
         length = mulle_range_get_end( range) - curr->location;
         if( length > curr->length)
            curr->length = length;
         goto _4;
      }

      // make found gobble up next
      curr->length = mulle_range_get_end( *next) - curr->location;
      __mulle__rangeset_move_down_pointer( p, next, allocator);
      // curr         = next;

      goto loop_3;  // could overlap more in back, so go back
   }

 _4:
   // hm we are done here check number of coalesces and if we have too many
   // coalesce and possibly shrink
   _mulle__rangeset_assert( p);
}


void   __mulle__rangeset_remove( struct mulle__rangeset *p,
                                 struct mulle_range range,
                                 struct mulle_allocator *allocator)
{
   struct mulle_range   *prev;
   struct mulle_range   *found;
   struct mulle_range   newRange;
   uintptr_t            range_end;
   uintptr_t            found_end;
   uintptr_t            n;

   // we don't deal with empty ranges
   if( ! range.length)
      return;

   _mulle__rangeset_assert( p);

   for(;;)
   {
      found = mulle_range_intersects_bsearch( p->_ranges, p->_length, range);
      if( ! found)
      {
         // no current range intersects, so we're done
         break;
      }

      // we have four cases
      // 1. range overlaps found completely : remove found:
      // 2. range makes a hole
      // 3. range removes part of the front
      // 4. range removes part of the back

      // 1.
      if( mulle_range_contains( range, *found))
      {
         __mulle__rangeset_move_down_pointer( p, found, allocator);
         continue;
      }

      found_end = mulle_range_get_end( *found);
      range_end = mulle_range_get_end( range);

      // 2. make a hole, we shrink what we have
      //    and insert a new piece with the  trailing bit
      if( range.location > found->location && range_end < found_end)
      {
         // front part
         found->length = range.location - found->location;

         // end part
         newRange = mulle_range_make( range_end, found_end - range_end);
         __mulle__rangeset_insert_known_absent( p, newRange, allocator);
         continue;
      }

      // 3. { 10, 10 } - { 8, 4 } = { 12, 8 }
      if( range_end < found_end)
      {
         found->length   = found_end - range_end;
         found->location = range_end;
         continue;
      }

      // 4. { 10, 10 } - { 12, 10 } = { 10, 2 }
      found->length = range.location - found->location;
   }

   _mulle__rangeset_assert( p);
}


void   _mulle__rangeset_insert_ranges( struct mulle__rangeset *p,
                                      struct mulle_range *ranges,
                                      uintptr_t n,
                                      struct mulle_allocator *allocator)
{
   struct mulle_range   *curr;
   struct mulle_range   *sentinel;

   curr     = &ranges[ 0];
   sentinel = &curr[ n];

   //
   // if we knew the ranges are sorted and that they are outside of
   // out range, we could just memcpy them over. We do know they
   // are sored, when we add another RangeSet though...
   //
   while( curr < sentinel)
      __mulle__rangeset_insert( p, *curr++, allocator);
}


void   _mulle__rangeset_remove_ranges( struct mulle__rangeset *p,
                                      struct mulle_range *ranges,
                                      uintptr_t n,
                                      struct mulle_allocator *allocator)
{
   struct mulle_range   *curr;
   struct mulle_range   *sentinel;

   curr     = &ranges[ 0];
   sentinel = &curr[ n];

   // turn off coalescing for now, and do one big one in the end
   while( curr < sentinel)
      __mulle__rangeset_remove( p, *curr++, allocator);

   _mulle__rangeset_shrink( p, allocator);
}


static struct mulle_range  *_mulle__rangeset_search_nearest( struct mulle__rangeset *p,
                                                            uintptr_t location)
{
   struct mulle_range   *curr;
   uintptr_t            index;
   //
   // re-retrieve curr since we don't know what remove ranges did
   //
   curr = mulle_range_intersects_bsearch( p->_ranges,
                                       p->_length,
                                       mulle_range_make( location, 1));
   if( ! curr)
   {
      // get the best fitting hole index then
      index = mulle_range_hole_bsearch( p->_ranges,
                                    p->_length,
                                    location);
      curr = &p->_ranges[ index];
   }
   return( curr);
}


static void  _mulle__rangeset_deltashift( struct mulle__rangeset *p,
                                                        struct mulle_range *curr,
                                                        intptr_t delta)
{
   struct mulle_range   *sentinel;

   sentinel = &p->_ranges[ p->_length];
   // we shift everything up AFTER found coz thats easy
   while( curr < sentinel)
   {
      curr->location += delta;
      ++curr;
   }
}


int   _mulle__rangeset_shift( struct mulle__rangeset *p,
                             uintptr_t location,
                             intptr_t delta,
                             struct mulle_allocator *allocator)
{
   struct mulle_range   *found;
   struct mulle_range   *sentinel;
   struct mulle_range   *prev;
   struct mulle_range   newRange;
   struct mulle_range   killRange;
   uintptr_t            start;
   uintptr_t            end;
   uintptr_t            found_end;
   uintptr_t            kill_start;
   uintptr_t            n;
   uintptr_t            index;

   if( ! p->_length  || ! delta)
      return( 0);

   /* check that we are not creating negatives or mulle_not_found_e */
   if( delta < 0)
   {
      start = p->_ranges[ 0].location;
      if( start < location)
         start = location;
      if( start < -delta)
         return( -1);

      // if delta is negative, we are clobbering so do this right now
      __mulle__rangeset_remove( p,
                               mulle_range_make( start + delta, -delta),
                               allocator);
   }
   else
   {
      end = mulle_range_get_end( p->_ranges[ p->_length - 1]);
      if( end + delta >= mulle_range_max)
         return( +1);
   }

   found = _mulle__rangeset_search_nearest( p, location);
   if( mulle_range_contains_location( *found, location) && found->location < location)
   {
      // first delta up all the rest
      _mulle__rangeset_deltashift( p, found + 1, delta);

      //
      // if we don't match exactly, we have a part that will be unaffected
      // and the rest will be affected. So split this off
      //
      found_end     = mulle_range_get_end( *found);
      found->length = location - found->location;

      // we need to add delta to this block immediately to avoid
      // coalescing
      newRange      = mulle_range_make( location + delta, found_end - location);
      __mulle__rangeset_insert( p, newRange, allocator);
      return( 0);
   }

   _mulle__rangeset_deltashift( p, found, delta);

   if( delta < 0)
   {
      // there is a possibility that we need to coalesce found with prev
      // here as the shift doesn't check
      prev = found - 1;
      if( prev >= p->_ranges)
      {
         if( mulle_range_get_end( *prev) == found->location)
         {
            prev->length += found->length;
            __mulle__rangeset_move_down_pointer( p, found, allocator);
         }
      }
   }
   return( 0);
}


uintptr_t   _mulle__rangeset_sum_lengths_range( struct mulle__rangeset *p,
                                               struct mulle_range range)
{
   struct mulle_range   *curr;
   struct mulle_range   *sentinel;
   uintptr_t            length;
   uintptr_t            total;

   total = 0;
   curr  = _mulle__rangeset_search_nearest( p, range.location);
   if( curr)
   {
      sentinel = &p->_ranges[ p->_length];
      while( curr < sentinel)
      {
         length = mulle_range_intersect( *curr, range).length;
         if( ! length)
            break;
         total += length;
         ++curr;
      }
   }
   return( total);
}


void   _mulle__rangeset_fprint( struct mulle__rangeset *p, FILE *fp)
{
   struct mulle_range      range;
   uintptr_t   i;

   if( ! fp)
      fp = stderr;
   for( i = 0; i < _mulle__rangeset_get_rangecount( p); i++)
   {
      range = _mulle__rangeset_get_range( p, i);
      fprintf( fp, "#%ld: %ld,%ld\n", (long) i, (long) range.location, (long) range.length);
   }
}


uintptr_t   _mulle__rangeset_search( struct mulle__rangeset *p,
                                    uintptr_t location,
                                    enum mulle_rangeset_searchoperation op)
{
   struct mulle_range   *curr;

   curr  = _mulle__rangeset_search_nearest( p, location);
   if( ! curr)
      return( mulle_not_found_e);

   switch( op)
   {
   case  mulle_rangeset_less_than_or_equal :
   case  mulle_rangeset_equal :
   case  mulle_rangeset_greater_than_or_equal :
      if( mulle_range_contains_location( *curr, location))
         return( location);
      // fall thru
   default:
      break;
   }

   switch( op)
   {
   case  mulle_rangeset_less_than :
   case  mulle_rangeset_less_than_or_equal :
      if( location)
      {
         --location;
         if( mulle_range_contains_location( *curr, location))
            return( location);
         if( curr > p->_ranges)
         {
            --curr;
            return( curr->location + curr->length - 1);
         }
      }
      break;

   case  mulle_rangeset_greater_than_or_equal :
   case  mulle_rangeset_greater_than :
      if( location < mulle_range_max)
      {
         ++location;
         if( mulle_range_contains_location( *curr, location))
            return( location);
         if( curr < &p->_ranges[ p->_length - 1])
         {
            ++curr;
            return( curr->location);
         }
      }
      // fall thru
   default:
      break;
   }
   return( mulle_not_found_e);
}


void   _mulle__rangeset_insert( struct mulle__rangeset *p,
                               struct mulle_range range,
                               struct mulle_allocator *allocator)
{
   if( ! range.length)
      return;
   // illegal,
   if( ! mulle_range_is_valid( range))
      return;

   __mulle__rangeset_insert( p, range, allocator);
}


void   _mulle__rangeset_remove( struct mulle__rangeset *p,
                               struct mulle_range range,
                               struct mulle_allocator *allocator)
{
   if( ! range.length)
      return;
   // illegal,
   if( ! mulle_range_is_valid( range))
      return;

   __mulle__rangeset_remove( p, range, allocator);
}
