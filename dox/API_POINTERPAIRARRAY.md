# `mulle_pointerpairarray`

`mulle_pointerpairarray` is a growing array of pointer pairs (key/value). It
works like an associative array with preserved insertion order. You can also use
it as a stack. For membership testing of larger collections, use a map instead.

**Caveat: `mulle_pointerpairarray` does no range checking when compiled with asserts disabled.**

A pointerpair with key equal to `mulle_pointerpair_notakey` (`mulle_not_a_pointer`) is
considered "not found" / invalid.


## Types

### `mulle_pointerpairarray`

```c
struct mulle_pointerpairarray
```

The array container. Stores a contiguous block of `mulle_pointerpair` elements
and an allocator.


### `mulle_pointerpair`

```c
struct mulle_pointerpair
{
   void   *key;
   void   *value;
};
```

A key/value pair of unmanaged pointers.


### `mulle_pointerpairarrayenumerator`

```c
struct mulle_pointerpairarrayenumerator
```

Enumerator for iterating over a `mulle_pointerpairarray`.


### `mulle_pointerpair_compare_t`

```c
typedef int   mulle_pointerpair_compare_t( struct mulle_pointerpair *a,
                                           struct mulle_pointerpair *b,
                                           void *userinfo);
```

Comparison function type used for sorting and searching. Returns negative if
a < b, zero if equal, positive if a > b.


## Functions


### Creation and Destruction


#### `mulle_pointerpairarray_alloc`

```c
struct mulle_pointerpairarray *
   mulle_pointerpairarray_alloc( struct mulle_allocator *allocator)
```

Allocate a zeroed `mulle_pointerpairarray` from `allocator`. Returns the
allocated array.


#### `mulle_pointerpairarray_create`

```c
struct mulle_pointerpairarray *
   mulle_pointerpairarray_create( struct mulle_allocator *allocator)
```

Allocate and initialize a `mulle_pointerpairarray` with default capacity.


#### `mulle_pointerpairarray_destroy`

```c
void   mulle_pointerpairarray_destroy( struct mulle_pointerpairarray *array)
```

Release storage and free `array`. NULL-safe.


#### `mulle_pointerpairarray_reset`

```c
void   mulle_pointerpairarray_reset( struct mulle_pointerpairarray *array)
```

Remove all elements but keep the allocated storage. NULL-safe.


### Accessors


#### `mulle_pointerpairarray_get_size`

```c
size_t   mulle_pointerpairarray_get_size( struct mulle_pointerpairarray *array)
```

Return the allocated capacity (number of pairs that fit without growth).
NULL-safe, returns 0 for NULL.


#### `mulle_pointerpairarray_get_count`

```c
size_t   mulle_pointerpairarray_get_count( struct mulle_pointerpairarray *array)
```

Return the number of pairs currently stored. NULL-safe, returns 0 for NULL.


#### `mulle_pointerpairarray_get_guaranteed_size`

```c
size_t   mulle_pointerpairarray_get_guaranteed_size( struct mulle_pointerpairarray *array)
```

Return the number of pairs that can be added without triggering a reallocation.
NULL-safe, returns 0 for NULL.


#### `mulle_pointerpairarray_is_full`

```c
int   mulle_pointerpairarray_is_full( struct mulle_pointerpairarray *array)
```

Return non-zero if the array has no remaining guaranteed space. NULL-safe,
returns 1 for NULL.


### Element Operations


#### `mulle_pointerpairarray_add`

```c
void   mulle_pointerpairarray_add( struct mulle_pointerpairarray *array,
                                   struct mulle_pointerpair pair)
```

Append `pair` to `array`, growing storage if needed. NULL-safe (does nothing
for NULL). The key must not be `mulle_pointerpair_notakey`.


#### `mulle_pointerpairarray_add_kv`

```c
void   mulle_pointerpairarray_add_kv( struct mulle_pointerpairarray *array,
                                      void *key,
                                      void *value)
```

Convenience function to append a key/value pair without constructing a
`mulle_pointerpair` struct. NULL-safe.


#### `mulle_pointerpairarray_get`

```c
struct mulle_pointerpair
   mulle_pointerpairarray_get( struct mulle_pointerpairarray *array,
                               size_t i)
```

Return the pair at index `i`. Returns an invalid pair (key ==
`mulle_pointerpair_notakey`) if `array` is NULL.


#### `mulle_pointerpairarray_get_last`

```c
struct mulle_pointerpair
   mulle_pointerpairarray_get_last( struct mulle_pointerpairarray *array)
```

Return the last element. Returns an invalid pair if the array is empty or NULL.


#### `mulle_pointerpairarray_get_in_range`

```c
size_t   mulle_pointerpairarray_get_in_range( struct mulle_pointerpairarray *array,
                                              struct mulle_range range,
                                              void *buf)
```

Copy pairs in `range` into `buf`. Returns the number of pairs copied.
NULL-safe, returns 0 for NULL.


#### `mulle_pointerpairarray_set`

```c
void   mulle_pointerpairarray_set( struct mulle_pointerpairarray *array,
                                   size_t i,
                                   struct mulle_pointerpair pair)
```

Overwrite the element at index `i` with `pair`. Unlike `add`, you may set a
pair with key == `mulle_pointerpair_notakey`.


#### `mulle_pointerpairarray_pop`

```c
struct mulle_pointerpair
   mulle_pointerpairarray_pop( struct mulle_pointerpairarray *array)
```

Remove and return the last element. Returns an invalid pair if the array is
empty or NULL.


### Search and Sort


#### `mulle_pointerpairarray_find`

```c
uintptr_t   mulle_pointerpairarray_find( struct mulle_pointerpairarray *array,
                                         struct mulle_pointerpair search)
```

Linear search for `search` using pointer equality on both key and value.
Returns the index, or `mulle_not_found_e` if not found. NULL-safe.


#### `mulle_pointerpairarray_find_in_range`

```c
uintptr_t   mulle_pointerpairarray_find_in_range( struct mulle_pointerpairarray *array,
                                                  struct mulle_pointerpair search,
                                                  struct mulle_range range)
```

Linear search within `range` using pointer equality. Returns the index, or
`mulle_not_found_e` if not found. NULL-safe.


#### `mulle_pointerpairarray_find_callback`

```c
uintptr_t   mulle_pointerpairarray_find_callback( struct mulle_pointerpairarray *array,
                                                  struct mulle_pointerpair search,
                                                  struct mulle_container_keyvaluecallback *callback)
```

Linear search using `callback` for equality comparison. Returns the index, or
`mulle_not_found_e` if not found. NULL-safe.


#### `mulle_pointerpairarray_find_compare`

```c
uintptr_t   mulle_pointerpairarray_find_compare( struct mulle_pointerpairarray *array,
                                                 struct mulle_pointerpair search,
                                                 mulle_pointerpair_compare_t *compare,
                                                 void *userinfo)
```

Linear search using a custom `compare` function. Returns the index, or
`mulle_not_found_e` if not found. NULL-safe.


#### `mulle_pointerpairarray_qsort_r`

```c
void   mulle_pointerpairarray_qsort_r( struct mulle_pointerpairarray *array,
                                       mulle_pointerpair_compare_t *compare,
                                       void *userinfo)
```

Sort the array in place using `compare` with `userinfo` context. NULL-safe.


#### `mulle_pointerpairarray_qsort`

```c
void   mulle_pointerpairarray_qsort( struct mulle_pointerpairarray *array,
                                     mulle_pointerpair_compare_t *compare)
```

Sort the array in place using `compare`. The array pointer is passed as
`userinfo` to the compare function. NULL-safe.


### Enumeration


#### `mulle_pointerpairarray_enumerate`

```c
struct mulle_pointerpairarrayenumerator
   mulle_pointerpairarray_enumerate( struct mulle_pointerpairarray *array)
```

Return an enumerator to iterate over all pairs in `array`. NULL-safe (returns
an empty enumerator for NULL).


#### `mulle_pointerpairarrayenumerator_next`

```c
int   mulle_pointerpairarrayenumerator_next( struct mulle_pointerpairarrayenumerator *rover,
                                             struct mulle_pointerpair *pair)
```

Advance the enumerator. Stores the next pair in `*pair` and returns 1, or
returns 0 when exhausted. NULL-safe.


#### `mulle_pointerpairarrayenumerator_done`

```c
void   mulle_pointerpairarrayenumerator_done( struct mulle_pointerpairarrayenumerator *rover)
```

Mark enumeration as finished. Currently a no-op but should always be called for
forward compatibility. NULL-safe.


### Macros


#### `mulle_pointerpairarray_do`

```c
mulle_pointerpairarray_do( name)
```

Scoped array on the stack. Declares a `mulle_pointerpairarray` named `name`
that is automatically cleaned up at the end of the enclosing scope.

```c
mulle_pointerpairarray_do( pairs)
{
   mulle_pointerpairarray_add_kv( pairs, key, value);
   // ...
}  // pairs is done here
```


#### `mulle_pointerpairarray_for`

```c
mulle_pointerpairarray_for( name, pair)
```

Convenience enumeration loop. Iterates over all pairs in the array `name`,
storing each in the local variable `pair`.

```c
struct mulle_pointerpair   pair;

mulle_pointerpairarray_for( array, pair)
{
   // use pair.key, pair.value
}
```
