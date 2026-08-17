# `mulle_map`

`mulle_map` is a hashtable container that uses
`mulle_container_keyvaluecallback` for the management of its key/value pairs.
It uses open addressing with linear probing.

### Enumerator invalidation

The enumerator traverses the map's internal storage directly. It is invalidated
by **any** mutation: `set`, `insert`, `remove`, `shrink`, `reset`, or
`copy_items`. In debug builds, mutations are detected and trigger an assertion
failure. Do not modify the map during enumeration.

### `mulle_map_update` ownership restriction

`mulle_map_update` returns the previous value for a key. This is only safe with
**nonowned** value callbacks. With owned or copied value callbacks the old value
is released before being returned — the returned pointer is dangling. Use
`mulle_map_update` only with `mulle_container_valuecallback_nonowned_pointer`
or equivalent. In debug builds, a releasing value callback triggers an assertion.

## Types

``` c
struct mulle_map
struct mulle_mapenumerator
```


## Functions

> For most functions there is an '`_`' (underscore-prefixed) version available
> that is not NULL-lenient but asserts in debug builds.


### Setup and Teardown

#### `mulle_map_create`

``` c
struct mulle_map   *mulle_map_create( size_t capacity,
                                      struct mulle_container_keyvaluecallback *callback,
                                      struct mulle_allocator *allocator);
```

Allocate and init a `mulle_map`. `callback` must be supplied. `allocator` may
be NULL. `capacity` is the initial capacity (will be rounded up).


#### `mulle_map_init`

``` c
void   mulle_map_init( struct mulle_map *map,
                       size_t capacity,
                       struct mulle_container_keyvaluecallback *callback,
                       struct mulle_allocator *allocator)
```

Initialize a `map` to hold initially `capacity` items. `callback` must be
given, `allocator` may be NULL.

The callback is NOT copied. It's usually best to keep it in static storage.

Example:

``` c
static struct mulle_container_keyvaluecallback   callback =
{
   .keycallback   = mulle_container_keycallback_copied_cstring,
   .valuecallback = mulle_container_valuecallback_int
};

struct mulle_map   map;

mulle_map_init( &map, 0, &callback, NULL);
```

See [Callbacks](API_CONTAINER_CALLBACK.md) for predefined callbacks.


#### `mulle_map_done`

``` c
void   mulle_map_done( struct mulle_map *map)
```

Free internal resources. All keys and values are released via callback.


#### `mulle_map_destroy`

``` c
void   mulle_map_destroy( struct mulle_map *map)
```

Done + free. Use for maps allocated with `mulle_map_create`.


#### `mulle_map_reset`

``` c
void   mulle_map_reset( struct mulle_map *map)
```

Remove all key/value pairs (releasing them), reset internal storage.


#### `mulle_map_remove_all`

``` c
void   mulle_map_remove_all( struct mulle_map *map)
```

Remove all key/value pairs, same as `mulle_map_reset`.


### Accessors

#### `mulle_map_get_count`

``` c
size_t   mulle_map_get_count( struct mulle_map *map)
```

Return the number of key/value pairs stored.


#### `mulle_map_get_size`

``` c
size_t   mulle_map_get_size( struct mulle_map *map)
```

Return the current hashtable size (number of slots).


#### `mulle_map_get_callback`

``` c
struct mulle_container_keyvaluecallback   *mulle_map_get_callback( struct mulle_map *map)
```

Return the callback used by the map.


#### `mulle_map_get_allocator`

``` c
struct mulle_allocator   *mulle_map_get_allocator( struct mulle_map *map)
```

Return the allocator used by the map.


#### `mulle_map_get_notakey`

``` c
void   *mulle_map_get_notakey( struct mulle_map *map)
```

Return the `notakey` sentinel value from the map's key callback.


#### `mulle_map_is_empty`

``` c
int   mulle_map_is_empty( struct mulle_map *map)
```

Returns non-zero if the map contains no entries.


#### `mulle_map_contains`

``` c
int   mulle_map_contains( struct mulle_map *map, void *key)
```

Returns non-zero if `key` is present in the map.


### Element Access — Read

#### `mulle_map_get`

``` c
void   *mulle_map_get( struct mulle_map *map, void *key)
```

Return the value associated with `key`, or NULL if not found.

> **Note:** If you store NULL as a value, use `mulle_map_get_pair` or
> `mulle_map_contains` to distinguish "not found" from "value is NULL".


#### `mulle_map_get_pair`

``` c
struct mulle_pointerpair   *mulle_map_get_pair( struct mulle_map *map,
                                                void *key,
                                                struct mulle_pointerpair *space)
```

Look up `key` and return a pointer to a `mulle_pointerpair` containing both
key and value, or NULL if not found. `space` is used as scratch storage.


### Element Access — Write

#### `mulle_map_set`

``` c
void   mulle_map_set( struct mulle_map *map, void *key, void *value)
```

Set `key` to `value`. If `key` already exists, the old key and value are
released and the new ones are retained. If `key` does not exist, it is
inserted.


#### `mulle_map_insert`

``` c
int   mulle_map_insert( struct mulle_map *map, void *key, void *value)
```

Insert `key`/`value` only if `key` does not already exist. Returns non-zero if
the insertion happened, 0 if the key was already present (no change made).


#### `mulle_map_register`

``` c
void   *mulle_map_register( struct mulle_map *map, void *key, void *value)
```

If `key` already exists, return the existing value (no insertion). Otherwise
insert and return NULL.


#### `mulle_map_update`

``` c
void   *mulle_map_update( struct mulle_map *map, void *key, void *value)
```

If `key` exists, replace its value and return the old value. Otherwise do
nothing and return NULL.


#### `mulle_map_remove`

``` c
int   mulle_map_remove( struct mulle_map *map, void *key)
```

Remove the entry for `key`. The key and value are released via callback.
Returns non-zero if the key was found and removed.


#### `mulle_map_insert_key_values`

``` c
void   mulle_map_insert_key_values( struct mulle_map *map,
                                    void *firstkey, ...)
```

Insert multiple key/value pairs. Arguments are interleaved:
key, value, key, value, ..., terminated by the map's `notakey` sentinel.

Example:

``` c
mulle_map_insert_key_values( map,
                             "blue", (void *) 18,
                             "red", (void *) 48,
                             mulle_map_get_notakey( map));
```


### Operations

#### `mulle_map_copy`

``` c
struct mulle_map   *mulle_map_copy( struct mulle_map *map)
```

Create a heap-allocated copy. Keys and values are retained via callback.


#### `mulle_map_add_map`

``` c
void   mulle_map_add_map( struct mulle_map *map, struct mulle_map *other)
```

Insert all entries from `other` into `map`.


#### `mulle_map_shrink_if_needed`

``` c
void   mulle_map_shrink_if_needed( struct mulle_map *map)
```

Shrink the hashtable if the load factor is very low after removals.


### Debugging

#### `mulle_map_describe`

``` c
char   *mulle_map_describe( struct mulle_map *map)
```

Return a string description of the map. Caller must free the returned string.


### Enumeration

#### `mulle_map_enumerate`

``` c
struct mulle_mapenumerator   mulle_map_enumerate( struct mulle_map *map)
```

Start enumeration of the map.

Example:

``` c
struct mulle_mapenumerator   rover;
void                         *key;
void                         *value;

rover = mulle_map_enumerate( map);
while( mulle_mapenumerator_next( &rover, &key, &value))
   printf( "%s : %s\n", (char *) key, (char *) value);
mulle_mapenumerator_done( &rover);
```

Or use the convenience macro:

``` c
char   *key;
char   *value;

mulle_map_for( map, key, value)
   printf( "%s : %s\n", key, value);
```


## `mulle_mapenumerator` Functions

### `mulle_mapenumerator_next`

``` c
int   mulle_mapenumerator_next( struct mulle_mapenumerator *rover,
                                void **key,
                                void **value)
```

Returns 1 and the next key/value pair. Returns 0 when exhausted.


### `mulle_mapenumerator_next_pair`

``` c
struct mulle_pointerpair   *mulle_mapenumerator_next_pair( struct mulle_mapenumerator *rover)
```

Returns a pointer to the next key/value pair, or NULL when exhausted.


### `mulle_mapenumerator_done`

``` c
void   mulle_mapenumerator_done( struct mulle_mapenumerator *rover)
```

End enumeration. Required for mutation detection in debug builds.
