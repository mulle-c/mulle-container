# `mulle_set`

`mulle_set` is a hashtable container for storing keys only (membership testing).
To store key/value pairs, use `mulle_map` instead. It uses open addressing with
linear probing, same as `mulle_map`.

### Enumerator invalidation

The enumerator traverses the set's internal storage directly. It is invalidated
by **any** mutation: `add`, `remove`, `shrink`, `reset`, or bulk operations
(union, intersection). In debug builds, mutations are detected and trigger an
assertion failure. Do not modify the set during enumeration.


## Types

``` c
struct mulle_set
struct mulle_setenumerator
```


## Functions

> For most functions there is an '`_`' (underscore-prefixed) version available
> that is not NULL-lenient but asserts in debug builds.


### Setup and Teardown

#### `mulle_set_create`

``` c
struct mulle_set   *mulle_set_create( size_t capacity,
                                      struct mulle_container_keycallback *callback,
                                      struct mulle_allocator *allocator);
```

Allocate and init a `mulle_set`. `callback` must be supplied. `allocator` may
be NULL. `capacity` is the initial capacity.


#### `mulle_set_init`

``` c
void   mulle_set_init( struct mulle_set *set,
                       size_t capacity,
                       struct mulle_container_keycallback *callback,
                       struct mulle_allocator *allocator);
```

Initialize a `set` (typically stack-allocated). `callback` must be given,
`allocator` may be NULL.


#### `mulle_set_done`

``` c
void   mulle_set_done( struct mulle_set *set)
```

Free internal resources. All keys are released via callback.


#### `mulle_set_destroy`

``` c
void   mulle_set_destroy( struct mulle_set *set)
```

Done + free. Use for sets allocated with `mulle_set_create`.


#### `mulle_set_reset`

``` c
void   mulle_set_reset( struct mulle_set *set)
```

Remove and release all keys, reset internal storage.


### Accessors

#### `mulle_set_get_count`

``` c
size_t   mulle_set_get_count( struct mulle_set *set)
```

Return the number of keys stored.


#### `mulle_set_get_size`

``` c
size_t   mulle_set_get_size( struct mulle_set *set)
```

Return the current hashtable size (number of slots).


#### `mulle_set_get_allocator`

``` c
struct mulle_allocator   *mulle_set_get_allocator( struct mulle_set *set)
```

Return the allocator.


#### `mulle_set_get_keycallback`

``` c
struct mulle_container_keycallback   *mulle_set_get_keycallback( struct mulle_set *set)
```

Return the key callback.


### Element Operations

#### `mulle_set_get`

``` c
void   *mulle_set_get( struct mulle_set *set, void *p)
```

Look up `p` in the set. Returns the stored key that is equal to `p` (which may
be a different pointer if equality is determined by callback), or NULL if not
found.


#### `mulle_set_member`

``` c
int   mulle_set_member( struct mulle_set *set, void *p)
```

Returns non-zero if `p` is present in the set.


#### `mulle_set_insert`

``` c
int   mulle_set_insert( struct mulle_set *set, void *p)
```

Insert `p` only if it is not already present. Returns non-zero if insertion
happened, 0 if `p` was already a member.


#### `mulle_set_register`

``` c
void   *mulle_set_register( struct mulle_set *set, void *p)
```

If `p` is already present, return the existing stored key (no insertion).
Otherwise insert `p` and return NULL.


#### `mulle_set_set`

``` c
void   mulle_set_set( struct mulle_set *set, void *p)
```

Insert or replace. If `p` already exists, the old key is released and `p` is
retained. If not present, `p` is simply inserted.


#### `mulle_set_remove`

``` c
void   mulle_set_remove( struct mulle_set *set, void *p)
```

Remove `p` from the set. The stored key is released via callback.


### Set Operations

#### `mulle_set_copy`

``` c
struct mulle_set   *mulle_set_copy( struct mulle_set *set)
```

Create a heap-allocated copy. Keys are retained via callback.


#### `mulle_set_add_set`

``` c
void   mulle_set_add_set( struct mulle_set *set, struct mulle_set *other)
```

Insert all keys from `other` into `set`.


#### `mulle_set_union`

``` c
void   mulle_set_union( struct mulle_set *dst,
                        struct mulle_set *a,
                        struct mulle_set *b)
```

Store the union of `a` and `b` into `dst`.


#### `mulle_set_intersect`

``` c
void   mulle_set_intersect( struct mulle_set *dst,
                            struct mulle_set *a,
                            struct mulle_set *b)
```

Store the intersection of `a` and `b` into `dst`.


#### `mulle_set_shrink_if_needed`

``` c
void   mulle_set_shrink_if_needed( struct mulle_set *set)
```

Shrink the hashtable if the load factor is very low after removals.


### Debugging

#### `mulle_set_describe`

``` c
char   *mulle_set_describe( struct mulle_set *set)
```

Return a string description of the set. Caller must free the returned string.


### Enumeration

#### `mulle_set_enumerate`

``` c
struct mulle_setenumerator   mulle_set_enumerate( struct mulle_set *set)
```

Start enumeration of the set.

Example:

``` c
struct mulle_setenumerator   rover;
void                         *key;

rover = mulle_set_enumerate( set);
while( mulle_setenumerator_next( &rover, &key))
   printf( "%s\n", (char *) key);
mulle_setenumerator_done( &rover);
```

Or use the convenience macro:

``` c
char   *key;

mulle_set_for( set, key)
   printf( "%s\n", key);
```


## `mulle_setenumerator` Functions

### `mulle_setenumerator_next`

``` c
int   mulle_setenumerator_next( struct mulle_setenumerator *rover, void **item)
```

Returns 1 and the next key in `item`. Returns 0 when exhausted.


### `mulle_setenumerator_done`

``` c
void   mulle_setenumerator_done( struct mulle_setenumerator *rover)
```

End enumeration. Required for mutation detection in debug builds.


## Macros

### `mulle_set_do`

``` c
mulle_set_do( name, callback)
{
   // use name as struct mulle_set *
}
```

Stack-allocate a set with automatic cleanup.


### `mulle_set_for`

``` c
mulle_set_for( set, item)
{
   // item is void *
}
```

Enumerate all keys in the set.
