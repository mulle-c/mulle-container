# `mulle_array`

`mulle_array` is a container that uses `mulle_container_keycallback` for
the management of it's elements. Internally it uses a C array of void * pointers, that is grown with `realloc`.

## Types

```
struct mulle_array
struct mulle_arrayenumerator
```

## Functions

### Creation / Destruction Verbs

* `alloc`
* `create`
* `destroy`
* `init`
* `done`
* `reset`

### Accessors Verbs

* `get_count`
* `is_full`
* `is_equal`

### Element Read Verbs

* `get`
* `get_intptr`
* `get_int`
* `get_last`
* `get_all`

### Element Write Verbs

* `add`
* `add_multiple`
* `add_intptr`
* `add_int`
* `add_array`
* `extract`
* `remove_in_range`
* `remove_last`
* `remove_all`
* `zero_in_range`

### Operations Verbs

* `advance`
* `guarantee`
* `compact_zeroes`
* `size_to_fit`
* `find_in_range`
* `find_in_range_identical`
* `find_in_range_identical_intptr`
* `find_in_range_identical_int`

### Enumeration Verbs

* `enumerate`



### `mulle_array_create`

```
struct mulle_array   *mulle_array_create( struct mulle_container_keycallback *callback,
                                          struct mulle_allocator *allocator);
```

Allocate and init a mulle_array. You must supply a `callback`, but you can pass NULL for `allocator`.


### `mulle_array_destroy`

```
void   mulle_array_destroy( struct mulle_array *array)
```

Destroy the array. Array must have been allocated with `mulle_array_create`
 or `mulle_array_alloc`.


### `mulle_array_done`

```
void   mulle_array_done( struct mulle_array *array)
```

Free internal resources of array. Don't use it aftewards, except for calling init again.


### `mulle_array_size_to_fit`

```
static inline int    mulle_array_size_to_fit( struct mulle_array *array)
```

Minimize memory usage of array.


### `mulle_array_remove_in_range`

```
void   mulle_array_remove_in_range( struct mulle_array *array, unsigned int location, unsigned int length)
```

Remove `length` elements at indices `location` and up,


### `mulle_array_zero_in_range`

```
static inline void   mulle_array_zero_in_range( struct mulle_array *array, unsigned int location, unsigned int length)
```

Overwrite `length` elements at indices `location` and up  with **notakey**.


### `mulle_array_compact_zeroes`

```
void   mulle_array_compact_zeroes( struct mulle_array *array)
```

Remove all elements in array, that are of value **notakey**.


### `mulle_array_remove_in_range`

```
mulle_array_remove_all( struct mulle_array *array)
```

Remove all elements in array,


### `mulle_array_init`

```
void    mulle_array_init( struct mulle_array *array,
                          unsigned int length,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator)
```

Setup array. Array will typically be stack based, if this function is called. `length` is the initial capacity of the array. `callback` must be given, but `allocator` may be NULL.


### `mulle_array_get_all`

```
static inline void   **mulle_array_get_all( struct mulle_array *array)
```

Cheating shortcut to get the internal C array of void pointers.


### `mulle_array_get_count`

```
unsigned int   mulle_array_get_count( struct mulle_array *array)
```

Return the number of elements in the array, including **notakey** values.


### `mulle_array_advance`

```
void   **mulle_array_advance( struct mulle_array *array,
                              unsigned int length)
```

This is like calling `mulle_array_next` for `length` times and ignoring the return value.

### `mulle_array_is_full`

```
int   mulle_array_is_full( struct mulle_array *array)
```

The array is filled to its capacity. That doesn't mean it can't grow by adding elements though.


### `mulle_array_is_equal `

```
inline int   mulle_array_is_equal( struct mulle_array *array, struct mulle_array *other)
```

Check if two arrays contain the same elements. The callbacks of `array` will be used for the comparison.


### `mulle_array_guaranteedsize`

```
unsigned int   mulle_array_guaranteedsize( struct mulle_array *array)
```

Return number of elements that can be added without the array having to malloc again.

### `mulle_array_guarantee`

```
void   **mulle_array_guarantee( struct mulle_array *array, unsigned int length)
```

Extend array to be able to hold at least `length` more elements.

### `mulle_array_add`

```
void    mulle_array_add( struct mulle_array *array, void  *p)
```

Add `p` to `array`. `p` must not be **notakey**, Array will grow automatically to accomodate the add.


### `mulle_array_get`

```
void   *mulle_array_get( struct mulle_array *array, unsigned int index)
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


### `mulle_array_add_multiple`

```
void   mulle_array_add_multiple( struct mulle_array *array,
                                 void **pointers,
                                 unsigned int length)
```

Add `length` elements in `pointers` to the array.


### `mulle_array_add_array`

```
void    mulle_array_add_array( struct mulle_array *array,
                               struct mulle_array *other)
```

Add contents of other to array. `other` remains unchanged.


### `mulle_array_reset`

```
void   mulle_array_reset( struct mulle_array *array)
```

Calls `mulle_array_done` and `mulle_array_init` in succession. The result is a clean slate, even cleaner than `mulle_array_remove_all`.


### `mulle_array_extract_all`

```
void   **mulle_array_extract_all( struct mulle_array *array)
```

You own the internal c-array of elements of array. `array` is empty afterwards.


### `mulle_array_find_in_range_identical`

```
unsigned long  mulle_array_find_in_range_identical( struct mulle_array *array,
                                                    void *obj,
                                                    struct mulle_range range)
```

Find index of element which is identical to `obj` in the range given by `range.location` and `range.length`.


### `mulle_array_find_in_range`

```
unsigned long  mulle_array_find_in_range( struct mulle_array *array,
                                          void *obj,
                                          struct mulle_range range)
```

Find index of element which is equal to `obj` in the range given by `range.location` and `range.length`. The equality is determined by the callback of array.



### `mulle_array_enumerate`

```
struct mulle_arrayenumerator   mulle_array_enumerate( struct mulle_array *array)
```

Start the enumeration of array. It is OK to pass NULL as array.


### `mulle_array_add_intptr `

```
void    mulle_array_add_intptr( struct mulle_array *array, int v)
```

Type convenience for `mulle_array_add` adding an **`intptr_t`** `v`.


### `mulle_array_add_int`

```
void    mulle_array_add_int( struct mulle_array *array, int v)
```

Type convenience for `mulle_array_add` adding an **`int`** `v`.



### `mulle_array_get_intptr`

```
intptr_t    mulle_array_get_intptr( struct mulle_array *array, unsigned int index)
```

Type convenience for `mulle_array_get` returning an **`intptr_t`**.


### `mulle_array_get_int`

```
int    mulle_array_get_int( struct mulle_array *array, unsigned int index)
```

Type convenience for `mulle_array_get` returning an **`int`**.


### `mulle_arrayenumerator_next_intptr`

```
intptr_t   mulle_arrayenumerator_next_intptr( struct mulle_arrayenumerator *rover)
```

Type convenience for `mulle_arrayenumerator_next` returning an **`intptr_t`**. Returns `mulle_not_found_e` when there is nothing left.


### `mulle_arrayenumerator_next_int`

```
int   mulle_arrayenumerator_next_int( struct mulle_arrayenumerator *rover)
```

Type convenience for `mulle_arrayenumerator_next` returning an **`int`**. Returns `mulle_not_found_e` when there is nothing left.


### `mulle_array_find_in_range_identical_intptr `

```
unsigned long  mulle_array_find_in_range_identical_intptr( struct mulle_array *array,
                                                           intptr_t v,
                                                           unsigned int location,
                                                           unsigned int length)
```

Type convenience for `mulle_array_find_in_range_identical` to find an **`intptr_t`** `v`. Returns `mulle_not_found_e` when nothing is found.


### `mulle_array_find_in_range_identical_int`

```
unsigned long   mulle_array_find_in_range_identical_int( struct mulle_array *array,
                                                         int v,
                                                         unsigned int location,
                                                         unsigned int length)
```

Type convenience for `mulle_array_find_in_range_identical` to find an **`int`** `v`. Returns `mulle_not_found_e` when nothing is found.

## `_mulle_arrayenumerator` Functions

### `mulle_arrayenumerator_next`

```
static inline void   *mulle_arrayenumerator_next( struct mulle_arrayenumerator *rover)
```

Return the next element in the array. Will be *notakey* if done.


### `mulle_arrayenumerator_done`

```
static inline void   * mulle_arrayenumerator_done( struct mulle_arrayenumerator *rover)
```

Marks the end of the lifetime of the enumerator. It's use is merely conventional.

