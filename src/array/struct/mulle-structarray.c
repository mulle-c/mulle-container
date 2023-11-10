#include "mulle-structarray.h"

#include "include-private.h"

// having something here is good for coverage so it doesnt look empty

void  _mulle_structarray_destroy( struct mulle_structarray *array)
{
   struct mulle_allocator   *allocator;

   allocator = array->allocator;
   _mulle_structarray_done( array);
   mulle_allocator_free( allocator, array);
}
