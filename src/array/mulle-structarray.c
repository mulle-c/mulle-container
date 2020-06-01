#include "mulle-structarray.h"

#include "include-private.h"

#include "mulle-container-math.h"



// Should also be usable for "size to fit"
void   _mulle_structarray_sizeto( struct mulle_structarray *array, size_t new_size)
{
   ptrdiff_t   old_size;
   ptrdiff_t   old_index;

   assert( new_size % array->_sizeof_struct == 0);

   old_size  = (char *) array->_sentinel - (char *) array->_structs;
   old_index = (char *) array->_curr - (char *) array->_structs;

   if( new_size < old_index)
      abort();

   array->_structs  = mulle_allocator_realloc( array->_allocator,
                                               array->_structs,
                                               new_size);
   array->_curr     = &((char *) array->_structs)[ old_index];
   array->_sentinel = &((char *) array->_structs)[ new_size];

   assert( (char *) array->_structs <= (char *) array->_curr);
   assert( (char *) array->_curr <= (char *) array->_sentinel);
}


void   _mulle_structarray_grow( struct mulle_structarray *array)
{
   size_t      new_size;
   ptrdiff_t   old_size;

   old_size = (char *) array->_sentinel - (char *) array->_structs;
   new_size = (old_size ? old_size : array->_sizeof_struct) * 2;
   _mulle_structarray_sizeto( array, new_size);
}
