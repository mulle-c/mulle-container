# `mulle_structarray`

`mulle_structarray` is a growing array of fixed-size structs. Unlike
`mulle_pointerarray` which stores `void *` pointers, `mulle_structarray`
stores struct copies inline in the array with proper alignment.

It has been coded for a fast "reserve" operation.

**BEWARE**: As the `mulle_structarray` reallocs, returned pointers are
guaranteed to be useful only until you modify the `mulle_structarray`. If you
need pointer-stable storage use `mulle_structqueue` instead.


## Types

``` c
struct mulle_structarray
struct mulle_structarrayenumerator
struct mulle_structarrayreverseenumerator
```


## Functions

> For most functions there is an '`_`' version available, that is not
> NULL lenient.


### Setup

#### `mulle_structarray_init`

``` c
void   mulle_structarray_init( struct mulle_structarray *array,
                               size_t sizeof_struct,
                               size_t alignof_struct,
                               size_t capacity,
                               struct mulle_allocator *allocator)
```

Initialize a `mulle_structarray` for elements of `sizeof_struct` bytes with
`alignof_struct` alignment. `capacity` is the initial number of elements.
`allocator` may be NULL for the default allocator.

Typical use:

``` c
struct mulle_structarray   array;

mulle_structarray_init( &array, sizeof( struct my_struct), alignof( struct my_struct), 8, NULL);
```


#### `mulle_structarray_create`

``` c
struct mulle_structarray   *mulle_structarray_create( size_t sizeof_struct,
                                                      size_t alignof_struct,
                                                      size_t capacity,
                                                      struct mulle_allocator *allocator)
```

Allocate and init a `mulle_structarray` on the heap.

There is also a convenience macro:

``` c
mulle_structarray_create_default( type)
```


#### `mulle_structarray_destroy`

``` c
void   mulle_structarray_destroy( struct mulle_structarray *array)
```

Destroy the array (done + free).


#### `mulle_structarray_done`

``` c
void   mulle_structarray_done( struct mulle_structarray *array)
```

Free internal resources.


#### `mulle_structarray_reset`

``` c
void   mulle_structarray_reset( struct mulle_structarray *array)
```

Remove all elements, keep allocation.


### Accessors

#### `mulle_structarray_get_count`

``` c
size_t   mulle_structarray_get_count( struct mulle_structarray *array)
```

Return the number of elements.


#### `mulle_structarray_get_size`

``` c
size_t   mulle_structarray_get_size( struct mulle_structarray *array)
```

Return the current capacity (number of elements that fit).


#### `mulle_structarray_get_element_size`

``` c
size_t   mulle_structarray_get_element_size( struct mulle_structarray *array)
```

Return the aligned size of each element in bytes.


#### `mulle_structarray_get_size_as_length`

``` c
size_t   mulle_structarray_get_size_as_length( struct mulle_structarray *array)
```

Return the capacity in bytes.


### Basic Element Operations

#### `mulle_structarray_add`

``` c
void   mulle_structarray_add( struct mulle_structarray *array, void *item)
```

Copy `item` (which must point to a struct of the correct size) into the array.
Returns a pointer to the copy in the array. The array grows automatically.


#### `mulle_structarray_set`

``` c
void   mulle_structarray_set( struct mulle_structarray *array, size_t i, void *item)
```

Overwrite element at index `i` with a copy of `item`.


#### `mulle_structarray_get`

``` c
void   *mulle_structarray_get( struct mulle_structarray *array, size_t i)
```

Return a pointer to the element at index `i`. The pointer is valid only until
the next modification of the array.


#### `mulle_structarray_get_first`

``` c
void   *mulle_structarray_get_first( struct mulle_structarray *array)
```

Return a pointer to the first element.


#### `mulle_structarray_get_last`

``` c
void   *mulle_structarray_get_last( struct mulle_structarray *array)
```

Return a pointer to the last element.


#### `mulle_structarray_remove_last`

``` c
void   mulle_structarray_remove_last( struct mulle_structarray *array)
```

Remove the last element.


#### `mulle_structarray_pop`

``` c
int   mulle_structarray_pop( struct mulle_structarray *array, void *item)
```

Remove the last element and copy it into `item`. Returns 1 on success, 0 if
the array was empty.


#### `mulle_structarray_insert`

``` c
void   *mulle_structarray_insert( struct mulle_structarray *array, size_t i, void *item)
```

Insert `item` at index `i`, shifting subsequent elements.


#### `mulle_structarray_remove_in_range`

``` c
void   mulle_structarray_remove_in_range( struct mulle_structarray *array,
                                          struct mulle_range range)
```

Remove elements in the given range.


### Array Operations

#### `mulle_structarray_guarantee`

``` c
void   *mulle_structarray_guarantee( struct mulle_structarray *array, size_t count)
```

Ensure space for `count` more elements without realloc.


#### `mulle_structarray_reserve`

``` c
void   *mulle_structarray_reserve( struct mulle_structarray *array)
```

Reserve space for one element and return a pointer to it. The element is
counted as used — copy your data in.


#### `mulle_structarray_advance`

``` c
void   *mulle_structarray_advance( struct mulle_structarray *array, size_t count)
```

Reserve space for `count` elements and return a pointer to the first. All
are counted as used.


#### `mulle_structarray_set_count`

``` c
void   mulle_structarray_set_count( struct mulle_structarray *array, size_t count)
```

Set the element count directly. Grows if needed.


#### `mulle_structarray_zero_to_count`

``` c
void   mulle_structarray_zero_to_count( struct mulle_structarray *array, size_t count)
```

Set count and zero-fill any new elements.


#### `mulle_structarray_add_structarray`

``` c
void   mulle_structarray_add_structarray( struct mulle_structarray *array,
                                          struct mulle_structarray *other,
                                          struct mulle_range range)
```

Add elements from `other` in the given range to `array`.


#### `mulle_structarray_copy`

``` c
struct mulle_structarray   *mulle_structarray_copy( struct mulle_structarray *other,
                                                    struct mulle_allocator *allocator)
```

Create a heap-allocated copy.


### Sorting

#### `mulle_structarray_qsort_r_inline`

``` c
void   mulle_structarray_qsort_r_inline( struct mulle_structarray *array,
                                         mulle_structarray_cmp_t *compare,
                                         void *userinfo)
```

Sort the array using a comparison function.


### Enumeration

#### `mulle_structarrayenumerator_next`

``` c
int   mulle_structarrayenumerator_next( struct mulle_structarrayenumerator *rover,
                                        void **item)
```

Returns 1 and a pointer to the next element in `item`. Returns 0 when done.


#### `mulle_structarrayenumerator_done`

``` c
void   mulle_structarrayenumerator_done( struct mulle_structarrayenumerator *rover)
```

Marks the end of the enumerator lifetime.


#### `mulle_structarrayreverseenumerator_next`

``` c
int   mulle_structarrayreverseenumerator_next( struct mulle_structarrayreverseenumerator *rover,
                                               void **item)
```

Returns 1 and a pointer to the next element (back to front). Returns 0 when done.


### Macros

#### `mulle_structarray_do`

``` c
mulle_structarray_do( name, type)
{
   // use name as struct mulle_structarray *
}
```

Stack-allocate a structarray with automatic cleanup.


#### `mulle_structarray_for`

``` c
mulle_structarray_for( array, type, item)
{
   // item is type *
}
```

Enumerate all elements.
