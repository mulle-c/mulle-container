#include "mulle--structarray.h"

#include "include-private.h"

#include "mulle-container-math.h"


//
// this is slightly preferable, because we don't have to divide so much
//
static void   _mulle__structarray_realloc_as_length( struct mulle__structarray *array,
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



static void   _mulle__structarray_realloc( struct mulle__structarray *array,
                                           size_t new_size,
                                           struct mulle_allocator *allocator)
{
   _mulle__structarray_realloc_as_length( array,
                                          new_size * array->_sizeof_struct, allocator);
}


// Should also be usable for "_size to fit"
void   _mulle__structarray_sizeto( struct mulle__structarray *array,
                                   size_t new_size,
                                   struct mulle_allocator *allocator)
{
   _mulle__structarray_realloc( array, new_size, allocator);
}


void   _mulle__structarray_grow( struct mulle__structarray *array,
                                 struct mulle_allocator *allocator)
{
   size_t   new_size;
   size_t   old_size;

   old_size = _mulle__structarray_get_size_as_length( array);
   new_size = (old_size ? old_size : array->_sizeof_struct) * 2;
   _mulle__structarray_realloc_as_length( array, new_size, allocator);
}


void *  _mulle__structarray_guarantee( struct mulle__structarray *array,
                                       size_t length,
                                       struct mulle_allocator *allocator)
{
   size_t   available;
   size_t   size;
   size_t   used;
   size_t   new_size;
   size_t   needed_size;
   size_t   old_size;

   assert( array->_sizeof_struct);

   size = _mulle__structarray_get_size( array);
   used = _mulle__structarray_get_count( array);
   available = size - used;
   if( available < length)
   {
      // grow by default, if exceeds growth sizeto
      old_size    = _mulle__structarray_get_size_as_length( array);
      new_size    = (old_size ? old_size : array->_sizeof_struct) * 2;
      needed_size = (size + (length - available)) * array->_sizeof_struct;
      if( needed_size > new_size)
         new_size = needed_size;
      _mulle__structarray_realloc_as_length( array, new_size, allocator);
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
   size_t   diff; // _mulle__structarray_set_count won't return negative

   diff = _mulle__structarray_set_count( array, count, allocator);
   memset( &((char *) array->_curr)[ - (long) diff * array->_sizeof_struct],
           0,
           diff * array->_sizeof_struct);
}


void
   mulle__structarray_add_structarray( struct mulle__structarray *array,
                                       struct mulle__structarray *other,
                                       struct mulle_range range,
                                       struct mulle_allocator *allocator)
{
   size_t   count;
   void     *buf;

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


struct mulle_data
   _mulle__structarray_extract_data( struct mulle__structarray *array,
                                     struct mulle_allocator *allocator)
{
   struct mulle_data   data;

//#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
//   array->_n_mutations++;
//#endif

   data = mulle_data_make( array->_storage,
                           _mulle__structarray_get_size( array));

   if( data.bytes && data.bytes == array->_initial_storage)
   {
      data.bytes = mulle_allocator_malloc( allocator, data.length);
      memcpy( data.bytes, array->_storage, data.length);

      array->_curr    =
      array->_storage = array->_initial_storage;
      return( data);
   }

   array->_storage         =
   array->_curr            =
   array->_sentinel        =
   array->_initial_storage = NULL;

   return( data);
}


void   _mulle__structarray_insert_in_range( struct mulle__structarray *array,
                                            struct mulle_range range,
                                            void *items,
                                            struct mulle_allocator *allocator)
{
   size_t count;
   size_t new_count;
   size_t size;
   size_t new_size;

   assert( range.length != (size_t) -1);
   if( ! range.length)
      return;

   // Get current number of elements = (curr - storage) / sizeof_struct
   count = _mulle__structarray_get_count( array);
   assert( range.location <= count);

   new_count = count + range.length;

   size = _mulle__structarray_get_size( array);
   if( new_count > size)
   {
      new_size = mulle_pow2round( new_count);
      _mulle__structarray_realloc( array, new_size, allocator);
   }

   // Shift existing elements to right to make room
   memmove(
       (char *) array->_storage + (range.location + range.length) * array->_sizeof_struct,
       (char *) array->_storage + range.location * array->_sizeof_struct,
       (count - range.location) * array->_sizeof_struct);

   //
   // Copy new elements into place, so we assume the incoming buffer is properly
   // aligned, when its a multiple, if its not a multiple we use array->_copyof_struct
   // MEMO: or do we always copy only the last one like this ?
   if( range.length == 1)
      memcpy( (char *) array->_storage + range.location * array->_sizeof_struct,
              items,
              array->_copy_sizeof_struct);
   else
      memcpy( (char *) array->_storage + range.location * array->_sizeof_struct,
              items,
              range.length * array->_sizeof_struct);

   // Update _curr pointer
   array->_curr = (char *) array->_storage + new_count * array->_sizeof_struct;

#if !defined(MULLE__CONTAINER_MISER_MODE) && defined(MULLE__CONTAINER_HAVE_MUTATION_COUNT)
   array->_n_mutations++;
#endif
}



void   _mulle__structarray_remove_in_range( struct mulle__structarray *array,
                                            struct mulle_range range)
{
   size_t   count;
   size_t   tail;
   size_t   length;

   if( ! range.length)
      return;

   count  = _mulle__structarray_get_count( array);
   range  = mulle_range_validate_against_length( range, count);
   tail   = mulle_range_get_max( range);
   length = (count - tail) * array->_sizeof_struct;
   if( length)
      memmove( _mulle__structarray_get( array, range.location),
               _mulle__structarray_get( array, tail),
               length);

   array->_curr = (char *) array->_curr - range.length * array->_sizeof_struct;

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   array->_n_mutations++;
#endif
}


