# `mulle_array`

`mulle_array` is a container that uses `mulle_container_keycallback` for
the management of its elements.
Internally it uses a C array of `void *` pointers, that is grown with `realloc`.

![](../pix/mulle-array.svg)


## Types

``` c
struct mulle_array
struct mulle_arrayenumerator
struct mulle_arrayreverseenumerator
```

## Functions

> For most functions there is an '`_`' (underscore-prefixed) version available
> that is not NULL-lenient but asserts in debug builds.


### Setup

#### `mulle_array_create`

``` c
struct mulle_array   *mulle_array_create( size_t capacity,
                                          struct mulle_container_keycallback *callback,
                                          struct mulle_allocator *allocator);
```

Allocate and init a `mulle_array`. You must supply a `callback`, but you can
pass NULL for `allocator`. `capacity` is the initial capacity of the array,
it may be rounded up.


#### `mulle_array_destroy`

``` c
void   mulle_array_destroy( struct mulle_array *array)
```

Destroy the array (done + free). Array must have been allocated with
`mulle_array_create`.


#### `mulle_array_init`

``` c
void   mulle_array_init( struct mulle_array *array,
                         size_t capacity,
                         struct mulle_container_keycallback *callback,
                         struct mulle_allocator *allocator)
```

Setup array. Array will typically be stack-based if this function is called.
`capacity` is the initial capacity of the array, it may be rounded up.
`callback` must be given, but `allocator` may be NULL.


#### `mulle_array_done`

``` c
void   mulle_array_done( struct mulle_array *array)
```

Free internal resources of array. Don't use it afterwards, except for calling
init again.


### Accessors

#### `mulle_array_get_count`

``` c
size_t   mulle_array_get_count( struct mulle_array *array)
```

Return the number of elements in the array.


#### `mulle_array_get_size`

``` c
size_t   mulle_array_get_size( struct mulle_array *array)
```

Return the current allocated size (capacity) of the array. Interesting mostly
for statistical purposes.


#### `mulle_array_get_guaranteed_size`

``` c
size_t   mulle_array_get_guaranteed_size( struct mulle_array *array)
```

Return the number of elements that can be added to the array without incurring
a realloc.


#### `mulle_array_get_allocator`

``` c
struct mulle_allocator   *mulle_array_get_allocator( struct mulle_array *array)
```

Return the allocator used by the array.


#### `mulle_array_get_callback`

``` c
struct mulle_container_keycallback   *mulle_array_get_callback( struct mulle_array *array)
```

Return the callback used by the array.


#### `mulle_array_is_full`

``` c
int   mulle_array_is_full( struct mulle_array *array)
```

The array is filled to its capacity. That doesn't mean it can't grow by adding
elements though.


### Element Operations

#### `mulle_array_add`

``` c
void   mulle_array_add( struct mulle_array *array, void *p)
```

Add `p` to `array`. The array will grow automatically to accommodate the add.
The element is retained via the callback.


#### `mulle_array_set`

``` c
void   mulle_array_set( struct mulle_array *array, size_t i, void *p)
```

Set element at index `i` to `p`. The previous element at that index is released
and `p` is retained via the callback.


#### `mulle_array_get`

``` c
void   *mulle_array_get( struct mulle_array *array, size_t index)
```

Return element at `index`. Returns NULL if array is NULL. The caller should
ensure with `mulle_array_get_count` that `index` is valid.


#### `mulle_array_get_last`

``` c
void   *mulle_array_get_last( struct mulle_array *array)
```

Return the last element in the array. Returns NULL if array is NULL or empty.


#### `mulle_array_remove_last`

``` c
void   mulle_array_remove_last( struct mulle_array *array)
```

Remove and release the last element from array. Does nothing if array is empty.


#### `mulle_array_remove`

``` c
void   mulle_array_remove( struct mulle_array *array, void *item)
```

Remove all occurrences of `item` from the array (equality determined by
callback).


#### `mulle_array_remove_unique`

``` c
void   mulle_array_remove_unique( struct mulle_array *array, void *item)
```

Remove the first occurrence of `item` from the array.


#### `mulle_array_remove_in_range`

``` c
void   mulle_array_remove_in_range( struct mulle_array *array,
                                    struct mulle_range range)
```

Remove `range.length` elements starting at index `range.location`.


### Array Operations


#### `mulle_array_guarantee`

``` c
void   **mulle_array_guarantee( struct mulle_array *array, size_t length)
```

Ensures that the next `length` additions will be done without a `realloc`.
Returns a pointer to the internal storage, or NULL on failure.


#### `mulle_array_grow`

``` c
void   mulle_array_grow( struct mulle_array *array)
```

Grow the array's internal storage. The count and element order remain the same.


#### `mulle_array_reset`

``` c
void   mulle_array_reset( struct mulle_array *array)
```

Remove and release all elements in the array. The internal storage is freed
and reinitialized.


#### `mulle_array_is_equal`

``` c
int   mulle_array_is_equal( struct mulle_array *array, struct mulle_array *other)
```

Check if two arrays contain the same elements (in the same order). The
callbacks of `array` are used for the comparison.


#### `mulle_array_add_array`

``` c
void   mulle_array_add_array( struct mulle_array *array,
                              struct mulle_array *other,
                              struct mulle_range range)
```

Add elements from `other` in the given `range` to `array`. Use
`mulle_range_make_all()` to add all elements.


#### `mulle_array_copy`

``` c
struct mulle_array   *mulle_array_copy( struct mulle_array *array)
```

Create a heap-allocated copy of `array`. Elements are retained via callback.


#### `mulle_array_member`

``` c
int   mulle_array_member( struct mulle_array *array, void *p)
```

Returns non-zero if `p` is found in the array (linear scan using callback
equality).


### Search

#### `mulle_array_find_in_range`

``` c
uintptr_t   mulle_array_find_in_range( struct mulle_array *array,
                                       void *obj,
                                       struct mulle_range range)
```

Find the index of the first element equal to `obj` in `range` (using callback
equality). Returns `mulle_not_found_e` if not found.


#### `mulle_array_find_in_range_identical`

``` c
uintptr_t   mulle_array_find_in_range_identical( struct mulle_array *array,
                                                 void *obj,
                                                 struct mulle_range range)
```

Find the index of the first element identical (`==`) to `obj` in `range`.
Returns `mulle_not_found_e` if not found.


#### `mulle_array_find`

``` c
uintptr_t   mulle_array_find( struct mulle_array *array, void *obj)
```

Find the index of `obj` in the entire array. Convenience for
`mulle_array_find_in_range( array, obj, mulle_range_make_all())`.


### Sorting

#### `mulle_array_qsort_r_inline`

``` c
void   mulle_array_qsort_r_inline( struct mulle_array *array,
                                   mulle_pointerarray_cmp_t *compare,
                                   void *userinfo)
```

Sort the array in place using `qsort_r` semantics.


### Debugging

#### `mulle_array_describe`

``` c
char   *mulle_array_describe( struct mulle_array *array)
```

Return a string description of the array using the callback's `.describe`.
The returned string must be freed by the caller.


### Enumeration

#### `mulle_array_enumerate`

``` c
struct mulle_arrayenumerator   mulle_array_enumerate( struct mulle_array *array)
```

Start enumeration of the array from front to back.

Example:

``` c
struct mulle_arrayenumerator   rover;
void                           *item;

rover = mulle_array_enumerate( array);
while( mulle_arrayenumerator_next( &rover, &item))
   printf( "%s\n", (char *) item);
mulle_arrayenumerator_done( &rover);
```

Or use the convenience macro:

``` c
char   *item;

mulle_array_for( array, item)
   printf( "%s\n", item);
```


#### `mulle_array_reverseenumerate`

``` c
struct mulle_arrayreverseenumerator   mulle_array_reverseenumerate( struct mulle_array *array)
```

Start enumeration of the array from back to front.


## `mulle_arrayenumerator` Functions

### `mulle_arrayenumerator_next`

``` c
int   mulle_arrayenumerator_next( struct mulle_arrayenumerator *rover, void **item)
```

Returns 1 and the next element in `item`. If there are no more items, returns
0 and leaves `item` unchanged.


### `mulle_arrayenumerator_done`

``` c
void   mulle_arrayenumerator_done( struct mulle_arrayenumerator *rover)
```

Marks the end of the lifetime of the enumerator. Conventional (currently a
no-op but required for future-proofing and mutation detection in debug builds).


## `mulle_arrayreverseenumerator` Functions

### `mulle_arrayreverseenumerator_next`

``` c
int   mulle_arrayreverseenumerator_next( struct mulle_arrayreverseenumerator *rover,
                                         void **item)
```

Returns 1 and the next element (traversing back to front) in `item`. If there
are no more items, returns 0 and leaves `item` unchanged.


### `mulle_arrayreverseenumerator_done`

``` c
void   mulle_arrayreverseenumerator_done( struct mulle_arrayreverseenumerator *rover)
```

Marks the end of the lifetime of the enumerator.
