#ifndef mulle_range_h__
#define mulle_range_h__

#include "include.h"

#include "mulle-container-operation.h"

#include <stdint.h>


//
// The range values are unsigned, but the actual range is limited to
// positive signed values
//
// a range with zero length can be valid
// the location of a range with zero length is basically undefined, but
// actually has to be within mulle_range_max
//
// a range that extends over mulle_range_max is invalid
//
struct mulle_range
{
   uintptr_t   location;
   uintptr_t   length;
};

#define mulle_range_min       (0)
#define mulle_range_max       (mulle_not_found_e-1)

// the struct and the three defines need to stay compatible with MulleObjC/mulle-objc-type.h


//
// experimentally make max=0xF....E and notfound=0xF...F
// #define mulle_range_max       ((~(uintptr_t) 0) - 1)
// compatible would be max=0x7....E and notfound=0x7...F
//
// Why this is moot: if we are storing void *, they will take up 2 or 3 bits
// worth of address space
//
static inline struct mulle_range   mulle_range_make( uintptr_t location, uintptr_t length)
{
    struct mulle_range   range;

    range.location = location;
    range.length   = length;
    return( range);
}



static inline uintptr_t   mulle_range_get_end( struct mulle_range range)
{
   return( range.location + range.length);
}


static inline int   mulle_range_contains_location( struct mulle_range range,
                                                   uintptr_t location)
{
   return( location - range.location < range.length);
}


static inline int  mulle_range_is_valid( struct mulle_range range)
{
   uintptr_t   end;

   if( range.location > mulle_range_max)
      return( 0);

   // zero length is always valid if location is sane
   if( ! range.length)
      return( 1);

   end = mulle_range_get_end( range);
   return( end > range.location && end <= mulle_range_max + 1);
}


static struct mulle_range
   mulle_range_validate_against_length( struct mulle_range range,
                                        uintptr_t length)
{
   uintptr_t  end;

   //
   // specialty, if length == -1, it means "full" range
   // this speeds up these cases, where you want to specify full range
   // but need to call -length first to create the range, and then
   // later call -length again to validate the range...
   //
   if( range.length == (uintptr_t) -1)
      range = mulle_range_make( 0, length);

   //
   // assume NSUInteger is 8 bit and range is { 3, 255 }, then we need to
   // check also for a negative length/location value making things difficult
   //
   end = mulle_range_get_end( range);
   if( end > length || end < range.location)
      return( mulle_range_make( 0, 0));

   return( range);
}

//
// the problem here is mostly, if a zero length range can contain another
// zero length range. Does a non-zero length range contain a zero length
// range. We say yes if the location is in range.
//
static inline int  mulle_range_contains( struct mulle_range big, struct mulle_range small)
{
   if( ! mulle_range_contains_location( big, small.location))
      return( 0);
   if( ! small.length)
      return( 1);
   return( mulle_range_contains_location( big, mulle_range_get_end( small) - 1));
}


struct mulle_range   mulle_range_intersect( struct mulle_range range,
                                            struct mulle_range other);

struct mulle_range   mulle_range_union( struct mulle_range range,
                                        struct mulle_range other);

// returns index where location would be inserted
uintptr_t   mulle_range_hole_bsearch( struct mulle_range *buf,
                                      uintptr_t n,
                                      intptr_t search_location);

// returns range containing search or NULL
struct mulle_range   *mulle_range_contains_bsearch( struct mulle_range *buf,
                                                    uintptr_t n,
                                                    struct mulle_range search);

struct mulle_range   *mulle_range_intersects_bsearch( struct mulle_range *buf,
                                                      uintptr_t n,
                                                      struct mulle_range search);
#endif
