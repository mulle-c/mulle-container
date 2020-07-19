#include "mulle--structarray.h"

#include "include-private.h"

#include "mulle-container-math.h"



// Should also be usable for "size to fit"
void   _mulle__structarray_sizeto( struct mulle__structarray *array,
                                   size_t new_size,
                                   struct mulle_allocator *allocator)
{
   ptrdiff_t   old_size;
   ptrdiff_t   old_index;

   assert( new_size % array->sizeof_struct == 0);

   old_size  = (char *) array->sentinel - (char *) array->storage;
   old_index = (char *) array->curr - (char *) array->storage;

   if( new_size < old_index)
      abort();

   array->storage  = mulle_allocator_realloc( allocator,
                                               array->storage,
                                               new_size);
   array->curr     = &((char *) array->storage)[ old_index];
   array->sentinel = &((char *) array->storage)[ new_size];

   assert( (char *) array->storage <= (char *) array->curr);
   assert( (char *) array->curr <= (char *) array->sentinel);
}


void   _mulle__structarray_grow( struct mulle__structarray *array,
                                 struct mulle_allocator *allocator)
{
   size_t      new_size;
   ptrdiff_t   old_size;

   old_size = (char *) array->sentinel - (char *) array->storage;
   new_size = (old_size ? old_size : array->sizeof_struct) * 2;
   _mulle__structarray_sizeto( array, new_size, allocator);
}
