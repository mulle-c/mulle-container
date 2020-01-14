#include "mulle-range.h"

#include "include-private.h"



struct mulle_range   mulle_range_intersect( struct mulle_range range,
                                            struct mulle_range other)
{
   uintptr_t            location;
   uintptr_t            min;
   uintptr_t            end1;
   uintptr_t            end2;
   struct mulle_range   result;

   if( ! range.length)
      return( range);
   if( ! other.length)
      return( other);

   end1 = mulle_range_get_end( range);
   end2 = mulle_range_get_end( other);
   min  = (end1 < end2) ? end1 : end2;
   location  = (range.location > other.location) ? range.location : other.location;

   if( min < location)
      result.location = result.length = 0;
   else
   {
      result.location = location;
      result.length   = min - location;
   }

   return( result);
}


struct mulle_range   mulle_range_union( struct mulle_range range,
                                        struct mulle_range other)
{
   uintptr_t            location;
   uintptr_t            end;
   uintptr_t            end1;
   uintptr_t            end2;
   struct mulle_range   result;

   if( ! other.length)
      return( range);
   if( ! range.length)
      return( other);

   end1      = mulle_range_get_end( range);
   end2      = mulle_range_get_end( other);
   end       = (end1 > end2) ? end1 : end2;
   location  = (range.location < other.location) ? range.location : other.location;

   result.location = location;
   result.length   = end - result.location;
   return(result);
}



// find a hole, this only works if its been ascertained
// that location is not contained
uintptr_t   mulle_range_hole_bsearch( struct mulle_range *buf,
                                      uintptr_t n,
                                      intptr_t search_location)
{
   intptr_t             first;
   intptr_t             last;
   intptr_t             middle;
   struct mulle_range   *p;

   first  = 0;
   last   = n - 1;
   middle = (first + last) / 2;

   while( first <= last)
   {
      p = &buf[ middle];
      if( p->location < search_location)
      {
         if( middle == last || p[ 1].location > search_location)
            return( middle + 1);

         first = middle + 1;
      }
      else
      {
         last = middle - 1;
      }

      middle = (first + last) / 2;
   }

   return( last + 1);
}


// find
struct mulle_range   *mulle_range_contains_bsearch( struct mulle_range *buf,
                                                    uintptr_t n,
                                                    struct mulle_range search)
{
   intptr_t             first;
   intptr_t             last;
   intptr_t             middle;
   struct mulle_range   *p;

   assert( search.length);
   assert( search.location != mulle_not_found_e);
   assert( search.location + search.length > search.location);

   first  = 0;
   last   = n - 1;
   middle = (first + last) / 2;

   while( first <= last)
   {
      p = &buf[ middle];
      if( p->location <= search.location)
      {
         if( mulle_range_contains( *p, search))
            return( p);

         first = middle + 1;
      }
      else
         last = middle - 1;

      middle = (first + last) / 2;
   }

   return( NULL);
}


// find
struct mulle_range   *mulle_range_intersects_bsearch( struct mulle_range *buf,
                                                      uintptr_t n,
                                                      struct mulle_range search)
{
   intptr_t             first;
   intptr_t             last;
   intptr_t             middle;
   struct mulle_range   *p;

   assert( search.length);
   assert( search.location != mulle_not_found_e);
   assert( search.location + search.length > search.location);

   first  = 0;
   last   = n - 1;
   middle = (first + last) / 2;

   while( first <= last)
   {
      p = &buf[ middle];
      if( p->location < mulle_range_get_end( search))
      {
         if( mulle_range_intersect( *p, search).length)
            return( p);

         first = middle + 1;
      }
      else
         last = middle - 1;

      middle = (first + last) / 2;
   }

   return( NULL);
}
