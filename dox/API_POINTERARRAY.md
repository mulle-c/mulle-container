# `mulle_pointerarray`

`mulle_pointerarray` is a growing array of unmanaged `void *` pointers. Pointers
are compared with `==`. No callbacks are used; the memory referenced by stored
pointers is not managed (not freed on removal). It stores its own allocator, so
you don't have to pass one on every call.

`mulle_pointerarray` does no range checking when compiled with asserts disabled.

### Pointer stability

| Pointer from | Valid until |
|--------------|------------|
| `get_storage` (raw `void **`) | next `add`/`grow`/`guarantee` that triggers realloc |
| `guarantee` return | next realloc-triggering operation |
| enumerator iteration | any mutation (add, remove, grow, reset, compact) |

The array stores `void *` values, not pointed-to objects, so element *values*
are stable — but the *storage array itself* may move on realloc.

![](../pix/mulle-pointerarray.svg)


## Types

``` c
struct mulle_pointerarray
struct mulle_pointerarrayenumerator
struct mulle_pointerarrayreverseenumerator
```


## Functions

> For most functions there is an '`_`' (underscore-prefixed) version available
> that is not NULL-lenient but asserts in debug builds.


### Setup / Teardown

#### `mulle_pointerarray_init`

``` c
void   mulle_pointerarray_init( struct mulle_pointerarray *array,
                                size_t capacity,
                                struct mulle_allocator *allocator)
```

Initialize `array` with an initial `capacity`. `allocator` may be NULL for
the default allocator. A typical use is for stack-based arrays.


#### `mulle_pointerarray_init_with_static_pointer`

``` c
void   mulle_pointerarray_init_with_static_pointer( struct mulle_pointerarray *array,
                                                    void **pointers,
                                                    size_t capacity,
                                                    struct mulle_allocator *allocator)
```

Initialize `array` with a caller-supplied `pointers` buffer of `capacity`
elements. Useful when you want the initial storage on the stack.


#### `mulle_pointerarray_done`

``` c
void   mulle_pointerarray_done( struct mulle_pointerarray *array)
```

Free internal resources of `array`. Don't use the array afterwards, except for
calling init again.


#### `mulle_pointerarray_create`

``` c
struct mulle_pointerarray *
   mulle_pointerarray_create( struct mulle_allocator *allocator)
```

Allocate and initialize a heap-based `mulle_pointerarray` with a default
capacity of 8. `allocator` may be NULL.


#### `mulle_pointerarray_destroy`

``` c
void   mulle_pointerarray_destroy( struct mulle_pointerarray *array)
```

Done + free. Use this for arrays created with `mulle_pointerarray_create`.


#### `mulle_pointerarray_reset`

``` c
void   mulle_pointerarray_reset( struct mulle_pointerarray *array)
```

Remove all elements, but keep the allocated storage for reuse.


### Accessors

#### `mulle_pointerarray_get_count`

``` c
size_t   mulle_pointerarray_get_count( struct mulle_pointerarray *array)
```

Return the number of elements currently stored in `array`.


#### `mulle_pointerarray_get_size`

``` c
size_t   mulle_pointerarray_get_size( struct mulle_pointerarray *array)
```

Return the current allocated capacity (number of pointer slots) of `array`.


#### `mulle_pointerarray_get_guaranteed_size`

``` c
size_t   mulle_pointerarray_get_guaranteed_size( struct mulle__pointerarray *array)
```

Return the number of elements that can be added without triggering a realloc.


#### `mulle_pointerarray_is_full`

``` c
int   mulle_pointerarray_is_full( struct mulle_pointerarray *array)
```

Return non-zero if `array` has no remaining capacity.


#### `mulle_pointerarray_get_allocator`

``` c
struct mulle_allocator *
   mulle_pointerarray_get_allocator( struct mulle_pointerarray *array)
```

Return the allocator associated with `array`, or NULL if `array` is NULL.


#### `_mulle_pointerarray_get_storage`

``` c
void **   _mulle_pointerarray_get_storage( struct mulle_pointerarray *array)
```

Return a pointer to the internal storage. Only available as an underscore-prefixed
(non-NULL-safe) variant.


### Memory Operations

#### `mulle_pointerarray_guarantee`

``` c
void **   mulle_pointerarray_guarantee( struct mulle_pointerarray *array,
                                        size_t length)
```

Ensure `array` has room for at least `length` more elements. Returns a pointer
to the current write position, or NULL if `array` is NULL.


#### `mulle_pointerarray_grow`

``` c
void   mulle_pointerarray_grow( struct mulle_pointerarray *array)
```

Grow the internal storage to the next power-of-two size.


#### `mulle_pointerarray_compact`

``` c
void   mulle_pointerarray_compact( struct mulle_pointerarray *array,
                                   void *notakey)
```

Remove all occurrences of `notakey` from `array`, compacting the remaining
elements.


### Element Operations

#### `mulle_pointerarray_add`

``` c
void   mulle_pointerarray_add( struct mulle_pointerarray *array, void *p)
```

Append pointer `p` to the end of `array`. The array grows automatically if
full.


#### `mulle_pointerarray_add_pointerarray`

``` c
void   mulle_pointerarray_add_pointerarray( struct mulle_pointerarray *array,
                                            struct mulle_pointerarray *other,
                                            struct mulle_range range)
```

Append elements from `other` in the given `range` to `array`.


#### `mulle_pointerarray_get`

``` c
void  *mulle_pointerarray_get( struct mulle_pointerarray *array, size_t i)
```

Return the pointer at index `i`. Returns NULL if `array` is NULL or `i` is
out of range (in release builds the behavior is undefined for out-of-range).


#### `mulle_pointerarray_get_in_range`

``` c
size_t   mulle_pointerarray_get_in_range( struct mulle_pointerarray *array,
                                          struct mulle_range range,
                                          void *buf)
```

Copy elements in `range` into `buf`. Returns the number of elements copied.


#### `mulle_pointerarray_get_last`

``` c
void  *mulle_pointerarray_get_last( struct mulle_pointerarray *array)
```

Return the last element of `array`, or NULL if empty.


#### `mulle_pointerarray_set`

``` c
void  *mulle_pointerarray_set( struct mulle_pointerarray *array,
                               size_t i,
                               void *p)
```

Overwrite the element at index `i` with `p`. Returns the previous value.
Index must be within current count.


#### `mulle_pointerarray_find`

``` c
uintptr_t   mulle_pointerarray_find( struct mulle_pointerarray *array, void *p)
```

Linear search for `p` (using `==`). Returns the index if found, or
`mulle_not_found_e` if not found.


#### `mulle_pointerarray_member`

``` c
int   mulle_pointerarray_member( struct mulle_pointerarray *array, void *p)
```

Return non-zero if `p` is contained in `array` (pointer equality).


#### `mulle_pointerarray_pop`

``` c
void *mulle_pointerarray_pop( struct mulle_pointerarray *array)
```

Remove and return the last element. Returns NULL if `array` is empty or NULL.


#### `mulle_pointerarray_remove`

``` c
void   mulle_pointerarray_remove( struct mulle_pointerarray *array,
                                  void *p)
```

Remove all occurrences of `p` from `array` (shifts elements).


#### `mulle_pointerarray_remove_unique`

``` c
void   *mulle_pointerarray_remove_unique( struct mulle_pointerarray *array,
                                          void *p)
```

Remove the first occurrence of `p`. Returns `p` if it was removed, NULL
otherwise.


#### `mulle_pointerarray_remove_in_range`

``` c
void   mulle_pointerarray_remove_in_range( struct mulle_pointerarray *array,
                                           struct mulle_range range)
```

Remove elements in `range` from `array`, shifting trailing elements down.


#### `mulle_pointerarray_remove_at_index`

``` c
void   mulle_pointerarray_remove_at_index( struct mulle_pointerarray *array,
                                           uintptr_t location)
```

Remove the element at `location`, shifting trailing elements down.


#### `mulle_pointerarray_insert`

``` c
void   mulle_pointerarray_insert( struct mulle_pointerarray *array,
                                  uintptr_t location,
                                  void *p)
```

Insert `p` at index `location`, shifting existing elements up.


#### `mulle_pointerarray_insert_in_range`

``` c
void   mulle_pointerarray_insert_in_range( struct mulle_pointerarray *array,
                                           struct mulle_range range,
                                           void **pointers)
```

Insert `pointers` at `range.location`, replacing `range.length` existing
elements.


### Random-Access Set/Get (Zeroing)

#### `mulle_pointerarray_set_zeroing`

``` c
void   mulle_pointerarray_set_zeroing( struct mulle_pointerarray *array,
                                       size_t i,
                                       void *value)
```

Set element at index `i` to `value`. If `i` is beyond the current count, the
array is grown and new slots are zero-filled.


#### `mulle_pointerarray_get_zeroing`

``` c
void   *mulle_pointerarray_get_zeroing( struct mulle_pointerarray *array,
                                        size_t i)
```

Get element at index `i`. If `i` is beyond the current count, returns NULL
(the array is not grown).


### Array Operations

#### `mulle_pointerarray_absorb_array`

``` c
void   mulle_pointerarray_absorb_array( struct mulle_pointerarray *array,
                                        struct mulle_pointerarray *victim)
```

Transfer all contents from `victim` into `array`. The `victim` is empty
afterwards; ownership of the stored pointers transfers to `array`.


#### `mulle_pointerarray_is_equal`

``` c
int   mulle_pointerarray_is_equal( struct mulle_pointerarray *array,
                                   struct mulle_pointerarray *other)
```

Return non-zero if both arrays have the same count and identical pointer
values at each index. Two NULL arrays are considered equal.


### Sorting

#### `mulle_pointerarray_qsort_r`

``` c
void   mulle_pointerarray_qsort_r( struct mulle_pointerarray *array,
                                   mulle_pointerarray_cmp_t *compare,
                                   void *userinfo)
```

Sort the array in place using `compare` (reentrant qsort). `userinfo` is
passed to the comparison function.


#### `mulle_pointerarray_qsort_r_inline`

``` c
void   mulle_pointerarray_qsort_r_inline( struct mulle_pointerarray *array,
                                          mulle_pointerarray_cmp_t *compare,
                                          void *userinfo)
```

Same as `mulle_pointerarray_qsort_r` but uses the inline variant of the sort.


### Enumeration

#### `mulle_pointerarray_enumerate`

``` c
struct mulle_pointerarrayenumerator
   mulle_pointerarray_enumerate( struct mulle_pointerarray *array)
```

Return a forward enumerator for `array`. Enumerating while modifying the
array leads to undefined behavior.


#### `mulle_pointerarrayenumerator_next`

``` c
int   mulle_pointerarrayenumerator_next( struct mulle_pointerarrayenumerator *rover,
                                         void **item)
```

Advance the enumerator. Returns non-zero if `*item` was set, 0 when done.


#### `mulle_pointerarrayenumerator_done`

``` c
void   mulle_pointerarrayenumerator_done( struct mulle_pointerarrayenumerator *rover)
```

Mark enumeration as finished. Currently a no-op, but call it for forward
compatibility.


#### `mulle_pointerarray_reverseenumerate`

``` c
struct mulle_pointerarrayreverseenumerator
   mulle_pointerarray_reverseenumerate( struct mulle_pointerarray *array)
```

Return a reverse enumerator for `array`.


#### `mulle_pointerarrayreverseenumerator_next`

``` c
int   mulle_pointerarrayreverseenumerator_next(
         struct mulle_pointerarrayreverseenumerator *rover,
         void **item)
```

Advance the reverse enumerator. Returns non-zero if `*item` was set, 0 when
done.


#### `mulle_pointerarrayreverseenumerator_done`

``` c
void   mulle_pointerarrayreverseenumerator_done(
          struct mulle_pointerarrayreverseenumerator *rover)
```

Mark reverse enumeration as finished. Currently a no-op.


### Convenience Macros

#### `mulle_pointerarray_do`

``` c
mulle_pointerarray_do( name)
{
   // use `name` as a struct mulle_pointerarray *
}
```

Declare and use a temporary `mulle_pointerarray` that is automatically cleaned
up at the end of the block.


#### `mulle_pointerarray_do_flexible`

``` c
mulle_pointerarray_do_flexible( name, stackcount)
{
   // use `name` as a struct mulle_pointerarray *
}
```

Like `mulle_pointerarray_do` but with `stackcount` elements of initial stack
storage to avoid early heap allocation.


#### `mulle_pointerarray_for`

``` c
mulle_pointerarray_for( array, item)
{
   // item is set to each element in forward order
}
```

Convenience macro to enumerate `array` forwards. `item` must be a `void *`
sized variable.


#### `mulle_pointerarray_for_reverse`

``` c
mulle_pointerarray_for_reverse( array, item)
{
   // item is set to each element in reverse order
}
```

Convenience macro to enumerate `array` in reverse. `item` must be a `void *`
sized variable.


## Usage Example

``` c
#include <mulle-container/mulle-container.h>

void   example( void)
{
   struct mulle_pointerarray              array;
   struct mulle_pointerarrayenumerator    rover;
   void                                   *item;

   mulle_pointerarray_init( &array, 8, NULL);

   mulle_pointerarray_add( &array, "hello");
   mulle_pointerarray_add( &array, "world");

   rover = mulle_pointerarray_enumerate( &array);
   while( mulle_pointerarrayenumerator_next( &rover, &item))
      printf( "%s\n", (char *) item);
   mulle_pointerarrayenumerator_done( &rover);

   mulle_pointerarray_done( &array);
}
```
