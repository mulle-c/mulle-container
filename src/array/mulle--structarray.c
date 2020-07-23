#include "mulle--structarray.h"

#include "include-private.h"

#include "mulle-container-math.h"


struct mulle__structarrayenumerator          mulle__structarrayenumerator_empty;
struct mulle__structarrayreverseenumerator   mulle__structarrayreverseenumerator_empty;



// Should also be usable for "_size to fit"
void   _mulle__structarray_sizeto( struct mulle__structarray *array,
                                   size_t new_size,
                                   struct mulle_allocator *allocator)
{
   ptrdiff_t   old_size;
   ptrdiff_t   old_index;

   assert( new_size % array->_sizeof_struct == 0);

   old_size  = (char *) array->_sentinel - (char *) array->_storage;
   old_index = (char *) array->_curr - (char *) array->_storage;

   if( new_size < old_index)
      abort();

   array->_storage  = mulle_allocator_realloc( allocator,
                                               array->_storage,
                                               new_size);
   array->_curr     = &((char *) array->_storage)[ old_index];
   array->_sentinel = &((char *) array->_storage)[ new_size];

   assert( (char *) array->_storage <= (char *) array->_curr);
   assert( (char *) array->_curr <= (char *) array->_sentinel);
}


void   _mulle__structarray_grow( struct mulle__structarray *array,
                                 struct mulle_allocator *allocator)
{
   size_t      new_size;
   ptrdiff_t   old_size;

   old_size = (char *) array->_sentinel - (char *) array->_storage;
   new_size = (old_size ? old_size : array->_sizeof_struct) * 2;
   _mulle__structarray_sizeto( array, new_size, allocator);
}
