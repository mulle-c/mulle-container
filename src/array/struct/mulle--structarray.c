#include "mulle--structarray.h"

#include "include-private.h"

#include "mulle-container-math.h"


void   _mulle__structarray_sizeto_length( struct mulle__structarray *array,
                                          size_t new_size,
                                          struct mulle_allocator *allocator)
{
   size_t   old_size;

   old_size = _mulle__structarray_get_used_as_length( array);
   if( new_size < old_size)
      abort();

   // if we have an initial storage
   if( array->_initial_storage)
   {
      // and we would still fit, ignore request
      if( new_size <= _mulle__structarray_get_size_as_length( array))
         return;

      // otherwise we need to malloc, now
      array->_storage  = mulle_allocator_malloc( allocator,
                                                 new_size);
      memcpy( array->_storage,
              array->_initial_storage,
              old_size);

      array->_initial_storage = NULL;
   }
   else
      array->_storage  = mulle_allocator_realloc( allocator,
                                                  array->_storage,
                                                  new_size);

   array->_curr     = &((char *) array->_storage)[ old_size];
   array->_sentinel = &((char *) array->_storage)[ new_size];

   assert( (char *) array->_storage <= (char *) array->_curr);
   assert( (char *) array->_curr <= (char *) array->_sentinel);
}


// Should also be usable for "_size to fit"
void   _mulle__structarray_sizeto( struct mulle__structarray *array,
                                   size_t new_size,
                                   struct mulle_allocator *allocator)
{
   _mulle__structarray_sizeto_length( array, new_size * array->_sizeof_struct, allocator);
}


void   _mulle__structarray_grow( struct mulle__structarray *array,
                                 struct mulle_allocator *allocator)
{
   size_t   new_size;
   size_t   old_size;

   old_size = _mulle__structarray_get_size_as_length( array);
   new_size = (old_size ? old_size : array->_sizeof_struct) * 2;
   _mulle__structarray_sizeto_length( array, new_size, allocator);
}


void *  _mulle__structarray_guarantee( struct mulle__structarray *array,
                                       size_t length,
                                       struct mulle_allocator *allocator)
{
   size_t   available;
   size_t   _size;
   size_t   _used;
   size_t         new_size;
   size_t         needed_size;
   size_t         old_size;

   _size = _mulle__structarray_get_size( array);
   _used = _mulle__structarray_get_count( array);
   available = _size - _used;
   if( available < length)
   {
      // grow by default, if exceeds growth sizeto
      old_size    = _mulle__structarray_get_size_as_length( array);
      new_size    = (old_size ? old_size : array->_sizeof_struct) * 2;
      needed_size = (_size + (length - available)) * array->_sizeof_struct;
      if( needed_size > new_size)
         new_size = needed_size;
      _mulle__structarray_sizeto_length( array, new_size, allocator);
   }
   return( array->_curr);
}


size_t  _mulle__structarray_set_count( struct mulle__structarray *array,
                                             size_t count,
                                             struct mulle_allocator *allocator)
{
   long   diff;

   diff = (long) count - (long) _mulle__structarray_get_count( array);
   if( diff <= 0)
   {
      array->_curr =  &((char *) array->_curr)[ diff * array->_sizeof_struct];
      _mulle__structarray_size_to_fit( array, allocator);
      return( 0);
   }

   _mulle__structarray_advance( array, (size_t) diff, allocator);
   return( (size_t) diff);
}



void   _mulle__structarray_zero_to_count( struct mulle__structarray *array,
                                          size_t count,
                                          struct mulle_allocator *allocator)
{
   long   diff;

   diff = _mulle__structarray_set_count( array, count, allocator);
   memset( &((char *) array->_curr)[ -diff * array->_sizeof_struct], 0, (size_t) diff * array->_sizeof_struct);
}


void
   mulle__structarray_add_structarray( struct mulle__structarray *array,
                                       struct mulle__structarray *other,
                                       struct mulle_range range,
                                       struct mulle_allocator *allocator)
{
   size_t   count;
   void           *buf;

   if( ! array)
      return;

   count = mulle__structarray_get_count( other);
   range = mulle_range_validate_against_length( range, count);
   if( ! range.length)
      return;

   if( other->_sizeof_struct != array->_sizeof_struct)
      abort();

   buf  = _mulle__structarray_advance( (struct mulle__structarray *) array, 
                                       range.length, 
                                       allocator);
   memcpy( buf, 
           &((char *) other->_storage)[ range.location * array->_sizeof_struct], 
           range.length * array->_sizeof_struct);

}
