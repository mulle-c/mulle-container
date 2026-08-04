# `mulle_assoc`

`mulle_assoc` is an auto-sorting associative array. It uses
`mulle_container_keyvaluecallback` for the management of its elements and a
compare function to keep them sorted. Internally it uses a
`mulle_pointerpairarray`. The API is intentionally similar to `mulle_map` so
you can swap between both implementations more easily.

The advantage of `mulle_assoc` over `mulle_map` is that it does not need
"holes", therefore will use less memory in most cases. The disadvantage is
that lookup is O(log n) instead of O(1).

You can not add duplicate keys.


## Types

``` c
struct mulle_assoc
struct mulle_assocenumerator
```


## Functions

> For most functions there is an '`_`' version available, that is not
> NULL lenient.


### Setup

#### `mulle_assoc_create`

``` c
struct mulle_assoc   *mulle_assoc_create( size_t capacity,
                                          struct mulle_container_keyvaluecallback *callback,
                                          mulle_pointerpair_compare_t *compare,
                                          struct mulle_allocator *allocator)
```

Allocate and init a `mulle_assoc`. You must supply a `callback`. `compare`
may be NULL, in which case pointer key comparison is used. `allocator` may
be NULL for the default allocator.


#### `mulle_assoc_destroy`

``` c
void   mulle_assoc_destroy( struct mulle_assoc *assoc)
```

Destroy the assoc (done + free). Must have been allocated with
`mulle_assoc_create`.


#### `mulle_assoc_init`

``` c
void   mulle_assoc_init( struct mulle_assoc *assoc,
                         size_t length,
                         struct mulle_container_keyvaluecallback *callback,
                         mulle_pointerpair_compare_t *compare,
                         struct mulle_allocator *allocator)
```

Setup assoc. Typically used for stack-based assocs. `callback` must be given.
`compare` may be NULL. `allocator` may be NULL.


#### `mulle_assoc_done`

``` c
void   mulle_assoc_done( struct mulle_assoc *assoc)
```

Free internal resources of assoc.


### Accessors

#### `mulle_assoc_get_count`

``` c
size_t   mulle_assoc_get_count( struct mulle_assoc *assoc)
```

Return the number of key/value pairs in the assoc.


#### `mulle_assoc_get_size`

``` c
size_t   mulle_assoc_get_size( struct mulle_assoc *assoc)
```

Return the current capacity of the assoc.


#### `mulle_assoc_get_guaranteed_size`

``` c
size_t   mulle_assoc_get_guaranteed_size( struct mulle_assoc *assoc)
```

Return the number of elements that can be added without incurring a realloc.


#### `mulle_assoc_is_full`

``` c
int   mulle_assoc_is_full( struct mulle_assoc *assoc)
```

Returns 1 if the assoc is filled to capacity.


#### `mulle_assoc_is_sorted`

``` c
int   mulle_assoc_is_sorted( struct mulle_assoc *assoc)
```

Returns 1 if the internal array is currently sorted. Sorting is done lazily.


#### `mulle_assoc_get_notakey`

``` c
void   *mulle_assoc_get_notakey( struct mulle_assoc *assoc)
```

Convenience to get the `notakey` value from the key callback.


### Basic Element Operations

#### `mulle_assoc_set`

``` c
void   mulle_assoc_set( struct mulle_assoc *assoc, void *key, void *value)
```

Insert or overwrite a key/value pair. If the key already exists, the old
key/value is released and replaced.


#### `mulle_assoc_add`

``` c
void   mulle_assoc_add( struct mulle_assoc *assoc, void *key, void *value)
```

Add a key/value pair. Assumes the key is **not** already present. In debug
mode this is asserted.


#### `mulle_assoc_get`

``` c
void   *mulle_assoc_get( struct mulle_assoc *assoc, void *key)
```

Return the value for `key`, or NULL if not found. Uses binary search on the
sorted array.


#### `mulle_assoc_get_at_index`

``` c
struct mulle_pointerpair   mulle_assoc_get_at_index( struct mulle_assoc *assoc,
                                                     size_t index)
```

Return the key/value pair at sorted `index`.


#### `mulle_assoc_get_last`

``` c
struct mulle_pointerpair   mulle_assoc_get_last( struct mulle_assoc *assoc)
```

Return the last key/value pair.


#### `mulle_assoc_remove`

``` c
void   mulle_assoc_remove( struct mulle_assoc *assoc, void *key)
```

Remove the key/value pair for `key`. Does nothing if key is not found.


#### `mulle_assoc_remove_last`

``` c
void   mulle_assoc_remove_last( struct mulle_assoc *assoc)
```

Remove the last key/value pair.


#### `mulle_assoc_remove_in_range`

``` c
void   mulle_assoc_remove_in_range( struct mulle_assoc *assoc,
                                    struct mulle_range range)
```

Remove key/value pairs in the given index range.


#### `mulle_assoc_member`

``` c
int   mulle_assoc_member( struct mulle_assoc *assoc, void *key)
```

Returns 1 if `key` is present, 0 otherwise. Can be slower than `get` if the
array isn't internally sorted yet, but doesn't force a sort.


#### `mulle_assoc_find`

``` c
uintptr_t   mulle_assoc_find( struct mulle_assoc *assoc, void *key)
```

Find the index of `key`. Returns `mulle_not_found_e` if not found.


#### `mulle_assoc_find_in_range`

``` c
uintptr_t   mulle_assoc_find_in_range( struct mulle_assoc *assoc,
                                       void *key,
                                       struct mulle_range range)
```

Find the index of `key` within the given range.


### Array Operations

#### `mulle_assoc_reset`

``` c
void   mulle_assoc_reset( struct mulle_assoc *assoc)
```

Remove all elements.


#### `mulle_assoc_is_equal`

``` c
int   mulle_assoc_is_equal( struct mulle_assoc *assoc,
                            struct mulle_assoc *other)
```

Check if two assocs contain the same key/value pairs.


#### `mulle_assoc_copy`

``` c
struct mulle_assoc   *mulle_assoc_copy( struct mulle_assoc *assoc)
```

Create a heap-allocated copy of the assoc.


#### `mulle_assoc_add_assoc`

``` c
void   mulle_assoc_add_assoc( struct mulle_assoc *assoc,
                              struct mulle_assoc *other)
```

Add all key/value pairs from `other` to `assoc`.


#### `mulle_assoc_set_compare`

``` c
void   mulle_assoc_set_compare( struct mulle_assoc *assoc,
                                mulle_pointerpair_compare_t *compare)
```

Change the compare function. Marks the assoc as unsorted.


### Enumeration

#### `mulle_assoc_enumerate`

``` c
struct mulle_assocenumerator   mulle_assoc_enumerate( struct mulle_assoc *assoc)
```

Start enumeration of the assoc.


#### `mulle_assocenumerator_next`

``` c
int   mulle_assocenumerator_next( struct mulle_assocenumerator *rover,
                                  void **key,
                                  void **value)
```

Returns 1 and the next key/value pair. Returns 0 when done.


#### `mulle_assocenumerator_done`

``` c
void   mulle_assocenumerator_done( struct mulle_assocenumerator *rover)
```

Marks the end of the enumerator lifetime. Conventional.


### Macros

#### `mulle_assoc_do`

``` c
mulle_assoc_do( name, callback, compare)
{
   // use name as struct mulle_assoc *
}
```

Stack-allocate an assoc with automatic cleanup.


#### `mulle_assoc_for`

``` c
mulle_assoc_for( assoc, key, value)
{
   // use key and value
}
```

Enumerate all key/value pairs.


### Debugging

#### `mulle_assoc_describe`

``` c
char   *mulle_assoc_describe( struct mulle_assoc *assoc)
```

Return a string description of the assoc. Caller must free the returned string.
