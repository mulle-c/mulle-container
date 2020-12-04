# `mulle_array`

`mulle_array` is a container that uses `mulle_container_keycallback` for
the management of it's elements.
Internally it uses a C array of void * pointers, that is grown with `realloc`.

![](../pix/mulle-array.svg)

## Types

```
struct mulle_array
struct mulle_arrayenumerator
struct mulle_arrayreverseenumerator
```

## Functions

> For most functions there is an '`_`' version available, that is not
> NULL lenient.

### Setup

#### `mulle_array_create`

```
struct mulle_array   *mulle_array_create( struct mulle_container_keycallback *callback,
                                          struct mulle_allocator *allocator);
```

Allocate and init a mulle_array. You must supply a `callback`, but you can
pass NULL for `allocator`.


#### `mulle_array_destroy`

```
void   mulle_array_destroy( struct mulle_array *array)
```

Destroy the array (done + free). Array must have been allocated with `mulle_array_create`
 or `mulle_array_alloc`.

#### `mulle_array_init`

```
void    mulle_array_init( struct mulle_array *array,
                          size_t capacity,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator)
```

Setup array. Array will typically be stack based, if this function is called.
`capacity` is the initial capacity of the array. `callback` must be given, but
`allocator` may be NULL.


#### `mulle_array_done`

```
void   mulle_array_done( struct mulle_array *array)
```

Free internal resources of array. Don't use it aftewards, except for calling init again.


### Accessors

#### `mulle_array_get_count`

```
size_t   mulle_array_get_count( struct mulle_array *array)
```

Return the number of elements in the array.


#### `mulle_array_get_guaranteed_size`

```
size_t   mulle_array_get_guaranteed_size( struct mulle_array *array)
```

Return the number of elements that can be added to the array without encurring a realloc.

#### `mulle_array_get_guaranteed_size`

```
struct mulle_allocator *mulle_array_get_allocator( struct mulle_array *array)
```

Return the allocator used by the array. `allocator` is also a public field of `struct mulle_array`.


#### `mulle_array_get_size`

```
size_t   mulle_array_get_size( struct mulle_array *array)
```

Return the current size of the array. Interesting mostly for statistical
purposes.

#### `mulle_array_is_full`

```
int   mulle_array_is_full( struct mulle_array *array)
```

The array is filled to its capacity. That doesn't mean it can't grow by adding elements though.


### Basic Element Operations

### `mulle_array_add`

```
void    mulle_array_add( struct mulle_array *array, void  *p)
```

Add `p` to `array`. Array will grow automatically to accomodate the add.


### `mulle_array_get`

```
void   *mulle_array_get( struct mulle_array *array, size_t index)
```

Return element at `index`. The caller is expected to ensure himself with a call to `mulle_array_get_count`, that index is valid.



### `mulle_array_get`

```
void   *mulle_array_get_last( struct mulle_array *array)
```

Return the last element int he array. Will be **notakey** if array is empty.


### `mulle_array_remove_last`

```
void   mulle_array_remove_last( struct mulle_array *array)
```

Remove the last element from array. Does nothing if array is empty.


### `mulle_array_remove_in_range`

```
void   mulle_array_remove_in_range( struct mulle_array *array, struct mulle_range range)
```

Remove `range.length` elements at indices `range.location` and up,




### Array Operations


#### `mulle_array_guarantee`

```
void   **mulle_array_guarantee( struct mulle_array *array,
                                size_t length)
```

Ensures that the next `length` additions will be
done without a `realloc`.


#### `mulle_array_grow`

```
void   **mulle_array_grow( struct mulle_array *array)
```

Grow the array to twice its current size. The count and element order remains the same.


#### `mulle_array_reset`

```
void   **mulle_array_reset( struct mulle_array *array)
```

Removes all elements in the array.


#### `mulle_array_size_to_fit`


```
static inline int    mulle_array_size_to_fit( struct mulle_array *array)
```

Minimize memory usage of array. Useful if you don't expect any further additions.



### `mulle_array_is_equal `

```
inline int   mulle_array_is_equal( struct mulle_array *array, struct mulle_array *other)
```

Check if two arrays contain the same elements. The callbacks of `array` will be used for the comparison.


### `mulle_array_guaranteedsize`

```
size_t   mulle_array_get_guaranteedsize( struct mulle_array *array)
```

Return number of elements that can be added without the array having to malloc again.

### `mulle_array_guarantee`

```
void   **mulle_array_guarantee( struct mulle_array *array, size_t length)
```

Extend array to be able to hold at least `range.length` more elements.



### `mulle_array_add_multiple`

```
void   mulle_array_add_multiple( struct mulle_array *array,
                                 void **pointers,
                                 size_t length)
```

Add `range.length` elements in `pointers` to the array.


### `mulle_array_add_array`

```
void   mulle_array_add_array( struct mulle_array *array,
                              struct mulle_array *other)
```

Add contents of other to array. `other` remains unchanged.


### `mulle_array_reset`

```
void   mulle_array_reset( struct mulle_array *array)
```

Calls `mulle_array_done` and `mulle_array_init` in succession. The result is a clean slate, even cleaner than `mulle_array_remove_all`.


### `mulle_array_find_in_range_identical`

```
uintptr_t  mulle_array_find_in_range_identical( struct mulle_array *array,
                                                void *obj,
                                                struct mulle_range range)
```

Find index of element which is identical to `obj` in the range given by `range.location` and `range.length`.


### `mulle_array_find_in_range`

```
uintptr_t  mulle_array_find_in_range( struct mulle_array *array,
                                      void *obj,
                                      struct mulle_range range)
```

Find index of element which is equal to `obj` in the range given by `range.location` and `range.length`. The equality is determined by the callback of array.



### `mulle_array_enumerate`

```
struct mulle_arrayenumerator   mulle_array_enumerate( struct mulle_array *array)
```

Start the enumeration of array from front to back.


### `mulle_array_reverseenumerate`

```
struct mulle_arrayreverseenumerator   mulle_array_reverseenumerate( struct mulle_array *array)
```

Start the enumeration of array from back to front.



## `mulle_arrayenumerator` Functions

### `mulle_arrayenumerator_next`

```
static inline int   mulle_arrayenumerator_next( struct mulle_arrayenumerator *rover, void **item)
```

Returns 1 and the next element in the array in `item`. If there are no more
items left, returns 0 and leaves item unchanged!


### `mulle_arrayenumerator_done`

```
static inline void   * mulle_arrayenumerator_done( struct mulle_arrayenumerator *rover)
```

Marks the end of the lifetime of the enumerator. It's use is merely conventional.


## `mulle_arrayreverseenumerator` Functions

### `mulle_arrayreverseenumerator_next`

```
static inline int   mulle_arrayreverseenumerator_next( struct mulle_arrayreverseenumerator *rover, void **item)
```

Returns 1 and the next element in the array in `item`. If there are no more
items left, returns 0 and leaves item unchanged!


### `mulle_arrayreverseenumerator_done`

```
static inline void   * mulle_arrayreverseenumerator_done( struct mulle_arrayreverseenumerator *rover)
```

Marks the end of the lifetime of the enumerator. It's use is merely conventional.
