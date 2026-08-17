# `mulle_assoc` and `mulle__assoc`

`mulle_assoc` is an auto-sorting associative array. It uses
`mulle_container_keyvaluecallback` for the management of its elements and a
compare function to keep them sorted. Internally it uses a
`mulle_pointerpairarray`. The API is intentionally similar to `mulle_map` so
you can swap between both implementations more easily.

The advantage of `mulle_assoc` over `mulle_map` is that it does not need
"holes", therefore will use less memory in most cases. The disadvantage is
that lookup is O(log n) instead of O(1).

You can not add duplicate keys. The userinfo for the compare function is the
assoc itself.

### Pointer stability

`mulle_assoc` uses contiguous storage (`mulle_pointerpairarray`). Pointers
obtained from `guarantee` or direct storage access are invalidated by any
operation that may trigger a realloc (add, set, grow). The enumerator is
invalidated by any mutation (add, remove, set, reset) — these may realloc or
memmove the internal array. Do not mutate during enumeration.

`mulle__assoc` is the double-underscore variant without embedded `allocator`,
`callback`, or `compare`. It is the low-level implementation that `mulle_assoc`
delegates to.


## Types

``` c
struct mulle_pointerpair
{
   void   *key;
   void   *value;
};

typedef int   mulle_pointerpair_compare_t( struct mulle_pointerpair *,
                                           struct mulle_pointerpair *,
                                           void *);

struct mulle__assoc
{
   MULLE__POINTERPAIRARRAY_BASE;
   int    _is_sorted;
};

struct mulle_assoc
{
   MULLE__POINTERPAIRARRAY_BASE;
   int                                       _is_sorted;
   struct mulle_container_keyvaluecallback   *callback;
   mulle_pointerpair_compare_t              *compare;
   struct mulle_allocator                   *allocator;
};

struct mulle__assocenumerator
{
   struct mulle__pointerpairarrayenumerator   base;
   void                                      *notakey;
};

struct mulle_assocenumerator
{
   MULLE__ASSOCENUMERATOR_BASE;
};
```


## Data Initializer

#### `MULLE_ASSOC_DATA`

``` c
#define MULLE_ASSOC_DATA( xcallback, xcompare, xallocator)
```

Static/compound-literal initializer for a `struct mulle_assoc`.


## `mulle_assoc` Functions

> For most functions there is an underscore-prefixed version available that is not
> NULL-safe but asserts its first parameter in debug builds.


### Setup and Teardown

#### `mulle_assoc_init`

``` c
void   mulle_assoc_init( struct mulle_assoc *assoc,
                         size_t length,
                         struct mulle_container_keyvaluecallback *callback,
                         mulle_pointerpair_compare_t *compare,
                         struct mulle_allocator *allocator)
```

Initialize `assoc` for stack or pre-allocated use. `callback` must not be NULL.
`compare` may be NULL (defaults to `_mulle_pointerpair_compare_pointer_key`).
`allocator` may be NULL.


#### `mulle_assoc_done`

``` c
void   mulle_assoc_done( struct mulle_assoc *assoc)
```

Release all retained elements and free internal storage.


#### `mulle_assoc_create`

``` c
struct mulle_assoc   *mulle_assoc_create( size_t capacity,
                                          struct mulle_container_keyvaluecallback *callback,
                                          mulle_pointerpair_compare_t *compare,
                                          struct mulle_allocator *allocator)
```

Heap-allocate and init a `mulle_assoc`.


#### `mulle_assoc_destroy`

``` c
void   mulle_assoc_destroy( struct mulle_assoc *assoc)
```

`done` + `free`. Use only for assocs created with `mulle_assoc_create`.


### Accessors

#### `mulle_assoc_get_count` / `_mulle_assoc_get_count`

``` c
size_t   mulle_assoc_get_count( struct mulle_assoc *assoc)
size_t   _mulle_assoc_get_count( struct mulle_assoc *assoc)
```

Return the number of key/value pairs stored.


#### `mulle_assoc_get_size` / `_mulle_assoc_get_size`

``` c
size_t   mulle_assoc_get_size( struct mulle_assoc *assoc)
size_t   _mulle_assoc_get_size( struct mulle_assoc *assoc)
```

Return the allocated capacity (number of pairs that fit before realloc).


#### `mulle_assoc_get_guaranteed_size` / `_mulle_assoc_get_guaranteed_size`

``` c
size_t   mulle_assoc_get_guaranteed_size( struct mulle_assoc *assoc)
size_t   _mulle_assoc_get_guaranteed_size( struct mulle_assoc *assoc)
```

Return the number of elements that can be added without triggering a realloc.


#### `mulle_assoc_is_full` / `_mulle_assoc_is_full`

``` c
int   mulle_assoc_is_full( struct mulle_assoc *assoc)
int   _mulle_assoc_is_full( struct mulle_assoc *assoc)
```

Returns 1 if the assoc is at capacity.


#### `mulle_assoc_is_sorted` / `_mulle_assoc_is_sorted`

``` c
int   mulle_assoc_is_sorted( struct mulle_assoc *assoc)
int   _mulle_assoc_is_sorted( struct mulle_assoc *assoc)
```

Returns 1 if the internal array is currently sorted. A NULL assoc is considered
sorted.


#### `mulle_assoc_get_callback` / `_mulle_assoc_get_callback`

``` c
struct mulle_container_keyvaluecallback   *mulle_assoc_get_callback( struct mulle_assoc *assoc)
struct mulle_container_keyvaluecallback   *_mulle_assoc_get_callback( struct mulle_assoc *assoc)
```

Return the callback pointer.


#### `mulle_assoc_get_compare` / `_mulle_assoc_get_compare`

``` c
mulle_pointerpair_compare_t   *mulle_assoc_get_compare( struct mulle_assoc *assoc)
mulle_pointerpair_compare_t   *_mulle_assoc_get_compare( struct mulle_assoc *assoc)
```

Return the compare function pointer.


#### `mulle_assoc_get_allocator` / `_mulle_assoc_get_allocator`

``` c
struct mulle_allocator   *mulle_assoc_get_allocator( struct mulle_assoc *assoc)
struct mulle_allocator   *_mulle_assoc_get_allocator( struct mulle_assoc *assoc)
```

Return the allocator pointer.


#### `mulle_assoc_set_compare` / `_mulle_assoc_set_compare`

``` c
void   mulle_assoc_set_compare( struct mulle_assoc *assoc,
                                mulle_pointerpair_compare_t *compare)
void   _mulle_assoc_set_compare( struct mulle_assoc *assoc,
                                 mulle_pointerpair_compare_t *compare)
```

Change the compare function. Marks the assoc as unsorted. If `compare` is
NULL, defaults to `_mulle_pointerpair_compare_pointer_key`.


#### `mulle_assoc_get_notakey` / `_mulle_assoc_get_notakey`

``` c
void   *mulle_assoc_get_notakey( struct mulle_assoc *assoc)
void   *_mulle_assoc_get_notakey( struct mulle_assoc *assoc)
```

Convenience to get the `notakey` sentinel from the key callback.


### Element Operations

#### `mulle_assoc_set` / `_mulle_assoc_set`

``` c
void   mulle_assoc_set( struct mulle_assoc *assoc, void *key, void *value)
void   _mulle_assoc_set( struct mulle_assoc *assoc, void *key, void *value)
```

Insert or overwrite a key/value pair. If the key already exists, the old
key/value is released and replaced.


#### `mulle_assoc_add` / `_mulle_assoc_add`

``` c
void   mulle_assoc_add( struct mulle_assoc *assoc, void *key, void *value)
void   _mulle_assoc_add( struct mulle_assoc *assoc, void *key, void *value)
```

Add a key/value pair. Assumes the key is **not** already present. In debug
builds this is asserted.


#### `mulle_assoc_get` / `_mulle_assoc_get`

``` c
void   *mulle_assoc_get( struct mulle_assoc *assoc, void *key)
void   *_mulle_assoc_get( struct mulle_assoc *assoc, void *key)
```

Return the value for `key`, or NULL if not found. Sorts lazily if needed,
then uses binary search.


#### `mulle_assoc_get_at_index`

``` c
struct mulle_pointerpair   mulle_assoc_get_at_index( struct mulle_assoc *assoc,
                                                     size_t index)
```

Return the key/value pair at sorted `index`. Sorts lazily if needed.


#### `mulle_assoc_set_at_index`

``` c
void   mulle_assoc_set_at_index( struct mulle_assoc *assoc,
                                 size_t index,
                                 void *key,
                                 void *value)
```

Replace the key/value pair at sorted `index`.


#### `mulle_assoc_get_in_range` / `_mulle_assoc_get_in_range`

``` c
size_t   mulle_assoc_get_in_range( struct mulle_assoc *assoc,
                                   struct mulle_range range,
                                   struct mulle_pointerpair *buf)
size_t   _mulle_assoc_get_in_range( struct mulle_assoc *assoc,
                                    struct mulle_range range,
                                    struct mulle_pointerpair *buf)
```

Copy key/value pairs in the given index range into `buf`. Returns the number
of pairs copied.


#### `mulle_assoc_get_last`

``` c
struct mulle_pointerpair   mulle_assoc_get_last( struct mulle_assoc *assoc)
```

Return the last key/value pair (unsorted order).


#### `mulle_assoc_remove` / `_mulle_assoc_remove`

``` c
void   mulle_assoc_remove( struct mulle_assoc *assoc, void *key)
void   _mulle_assoc_remove( struct mulle_assoc *assoc, void *key)
```

Remove the key/value pair for `key`. Does nothing if key is not found.


#### `mulle_assoc_remove_in_range`

``` c
void   mulle_assoc_remove_in_range( struct mulle_assoc *assoc,
                                    struct mulle_range range)
```

Remove key/value pairs in the given index range.


#### `mulle_assoc_remove_last`

``` c
void   mulle_assoc_remove_last( struct mulle_assoc *assoc)
```

Remove the last key/value pair.


#### `mulle_assoc_member`

``` c
int   mulle_assoc_member( struct mulle_assoc *assoc, void *key)
```

Returns 1 if `key` is present, 0 otherwise. Uses linear search via the
key callback's `is_equal`, so it does not require a sorted state.


#### `mulle_assoc_find` / `mulle_assoc_find_in_range`

``` c
uintptr_t   mulle_assoc_find( struct mulle_assoc *assoc, void *key)
uintptr_t   mulle_assoc_find_in_range( struct mulle_assoc *assoc,
                                       void *key,
                                       struct mulle_range range)
```

Find the index of `key`. Returns `mulle_not_found_e` if not found.


### Bulk Operations

#### `mulle_assoc_reset` / `_mulle_assoc_reset`

``` c
void   mulle_assoc_reset( struct mulle_assoc *assoc)
void   _mulle_assoc_reset( struct mulle_assoc *assoc)
```

Remove all elements, releasing each one. Keeps the allocation.


#### `mulle_assoc_is_equal`

``` c
int   mulle_assoc_is_equal( struct mulle_assoc *assoc,
                            struct mulle_assoc *other)
```

Check if two assocs contain the same key/value pairs. Keys are compared using
the callback's `is_equal`. Values are compared using the compare function if
provided, otherwise by pointer equality.


#### `mulle_assoc_copy`

``` c
struct mulle_assoc   *mulle_assoc_copy( struct mulle_assoc *assoc)
```

Create a heap-allocated deep copy of the assoc (elements are retained via
callbacks).


#### `mulle_assoc_add_assoc`

``` c
void   mulle_assoc_add_assoc( struct mulle_assoc *assoc,
                              struct mulle_assoc *other)
```

Add all key/value pairs from `other` to `assoc`.


#### `mulle_assoc_add_assoc_range`

``` c
void   mulle_assoc_add_assoc_range( struct mulle_assoc *assoc,
                                    struct mulle_assoc *other,
                                    struct mulle_range range)
```

Add key/value pairs from `other` within the given range to `assoc`.


### Key Remapping

#### `mulle_assoc_remap_intptr_key_range` / `_mulle_assoc_remap_intptr_key_range`

``` c
void   mulle_assoc_remap_intptr_key_range( struct mulle_assoc *assoc,
                                           struct mulle_range range,
                                           intptr_t offset)
void   _mulle_assoc_remap_intptr_key_range( struct mulle_assoc *assoc,
                                            struct mulle_range range,
                                            intptr_t offset)
```

Add `offset` to all intptr keys in the given range.


#### `mulle_assoc_move_intptr_key_range` / `_mulle_assoc_move_intptr_key_range`

``` c
void   mulle_assoc_move_intptr_key_range( struct mulle_assoc *assoc,
                                          struct mulle_range range,
                                          intptr_t offset)
void   _mulle_assoc_move_intptr_key_range( struct mulle_assoc *assoc,
                                           struct mulle_range range,
                                           intptr_t offset)
```

Move keys in range by `offset`.


#### `mulle_assoc_assert_no_intptr_key_dupes`

``` c
void   mulle_assoc_assert_no_intptr_key_dupes( struct mulle_assoc *assoc)
```

In debug builds, asserts that no duplicate intptr keys exist.


### Enumeration

#### `mulle_assoc_enumerate` / `_mulle_assoc_enumerate`

``` c
struct mulle_assocenumerator   mulle_assoc_enumerate( struct mulle_assoc *assoc)
struct mulle_assocenumerator   _mulle_assoc_enumerate( struct mulle_assoc *assoc)
```

Start enumeration. The assoc is sorted lazily before enumeration begins.
Do not modify the assoc during enumeration.


#### `mulle_assocenumerator_next` / `_mulle_assocenumerator_next`

``` c
int   mulle_assocenumerator_next( struct mulle_assocenumerator *rover,
                                  void **key,
                                  void **value)
int   _mulle_assocenumerator_next( struct mulle_assocenumerator *rover,
                                   void **key,
                                   void **value)
```

Returns 1 and sets `*key`/`*value` to the next pair. Returns 0 when done.


#### `mulle_assocenumerator_done` / `_mulle_assocenumerator_done`

``` c
void   mulle_assocenumerator_done( struct mulle_assocenumerator *rover)
void   _mulle_assocenumerator_done( struct mulle_assocenumerator *rover)
```

Marks the end of the enumerator lifetime. No-op currently.


### Debugging

#### `mulle_assoc_describe` / `_mulle_assoc_describe`

``` c
char   *mulle_assoc_describe( struct mulle_assoc *assoc)
char   *_mulle_assoc_describe( struct mulle_assoc *assoc)
```

Return a malloc'd string description of the assoc. Caller must free.


### Macros

#### `mulle_assoc_do`

``` c
mulle_assoc_do( name, callback, compare)
{
   // use name as struct mulle_assoc *
}
```

Stack-allocate an assoc with automatic cleanup on scope exit.


#### `mulle_assoc_for`

``` c
mulle_assoc_for( assoc, key, value)
{
   // key and value are set each iteration
}
```

Enumerate all key/value pairs with a for-each loop.


---

## `mulle__assoc` Functions

The double-underscore variant stores no allocator, callback, or compare
internally. You must pass them to every call. This is the low-level layer that
`mulle_assoc` delegates to.


### Setup and Teardown

#### `_mulle__assoc_init`

``` c
void   _mulle__assoc_init( struct mulle__assoc *assoc,
                           size_t capacity,
                           struct mulle_allocator *allocator)
```

Initialize an assoc with given capacity.


#### `_mulle__assoc_done`

``` c
void   _mulle__assoc_done( struct mulle__assoc *assoc,
                           struct mulle_container_keyvaluecallback *callback,
                           struct mulle_allocator *allocator)
```

Release all elements and free internal storage.


#### `mulle__assoc_create`

``` c
struct mulle__assoc   *mulle__assoc_create( struct mulle_allocator *allocator)
```

Heap-allocate and zero-init a `mulle__assoc`.


#### `_mulle__assoc_destroy`

``` c
void   _mulle__assoc_destroy( struct mulle__assoc *assoc,
                              struct mulle_container_keyvaluecallback *callback,
                              struct mulle_allocator *allocator)
```

`done` + `free`.


### Accessors

#### `_mulle__assoc_get_count` / `mulle__assoc_get_count`

``` c
size_t   _mulle__assoc_get_count( struct mulle__assoc *assoc)
size_t   mulle__assoc_get_count( struct mulle__assoc *assoc)
```


#### `_mulle__assoc_get_size` / `mulle__assoc_get_size`

``` c
size_t   _mulle__assoc_get_size( struct mulle__assoc *assoc)
size_t   mulle__assoc_get_size( struct mulle__assoc *assoc)
```


#### `_mulle__assoc_get_guaranteed_size` / `mulle__assoc_get_guaranteed_size`

``` c
size_t   _mulle__assoc_get_guaranteed_size( struct mulle__assoc *assoc)
size_t   mulle__assoc_get_guaranteed_size( struct mulle__assoc *assoc)
```


#### `_mulle__assoc_is_full` / `mulle__assoc_is_full`

``` c
int   _mulle__assoc_is_full( struct mulle__assoc *assoc)
int   mulle__assoc_is_full( struct mulle__assoc *assoc)
```


#### `_mulle__assoc_is_sorted` / `mulle__assoc_is_sorted`

``` c
int   _mulle__assoc_is_sorted( struct mulle__assoc *assoc)
int   mulle__assoc_is_sorted( struct mulle__assoc *assoc)
```


#### `_mulle__assoc_set_unsorted`

``` c
void   _mulle__assoc_set_unsorted( struct mulle__assoc *assoc)
```

Force the sorted flag to false.


### Sorting

#### `_mulle__assoc_qsort_r` / `mulle__assoc_qsort_r`

``` c
void   _mulle__assoc_qsort_r( struct mulle__assoc *assoc,
                               mulle_pointerpair_compare_t *compare,
                               void *userinfo)
void   mulle__assoc_qsort_r( struct mulle__assoc *assoc,
                              mulle_pointerpair_compare_t *compare,
                              void *userinfo)
```

Sort the assoc. Sets `_is_sorted` to 1.


#### `_mulle__assoc_qsort_r_if_needed` / `mulle__assoc_qsort_if_needed`

``` c
void   _mulle__assoc_qsort_r_if_needed( struct mulle__assoc *assoc,
                                         mulle_pointerpair_compare_t *compare,
                                         void *userinfo)
void   mulle__assoc_qsort_if_needed( struct mulle__assoc *assoc,
                                     mulle_pointerpair_compare_t *compare,
                                     void *userinfo)
```

Sort only if the assoc is not already sorted.


### Element Operations

#### `_mulle__assoc_add` / `mulle__assoc_add`

``` c
void   _mulle__assoc_add( struct mulle__assoc *assoc,
                          void *key,
                          void *value,
                          struct mulle_container_keyvaluecallback *callback,
                          struct mulle_allocator *allocator)
void   mulle__assoc_add( struct mulle__assoc *assoc,
                         void *key,
                         void *value,
                         struct mulle_container_keyvaluecallback *callback,
                         struct mulle_allocator *allocator)
```

Add a retained key/value pair. Adding `notakey` aborts. Marks unsorted.


#### `_mulle__assoc_add_guaranteed`

``` c
void   _mulle__assoc_add_guaranteed( struct mulle__assoc *assoc,
                                     void *key,
                                     void *value,
                                     struct mulle_container_keyvaluecallback *callback,
                                     struct mulle_allocator *allocator)
```

Like `_mulle__assoc_add` but caller guarantees enough space exists.


#### `_mulle__assoc_set`

``` c
void   _mulle__assoc_set( struct mulle__assoc *assoc,
                          void *key,
                          void *value,
                          struct mulle_container_keyvaluecallback *callback,
                          mulle_pointerpair_compare_t *compare,
                          void *userinfo,
                          struct mulle_allocator *allocator)
```

Insert or overwrite a key/value pair.


#### `mulle__assoc_set`

``` c
void   mulle__assoc_set( struct mulle__assoc *assoc,
                         void *key,
                         void *value,
                         mulle_pointerpair_compare_t *compare,
                         void *userinfo,
                         struct mulle_container_keyvaluecallback *callback,
                         struct mulle_allocator *allocator)
```

NULL-safe wrapper for `_mulle__assoc_set`.


#### `_mulle__assoc_get` / `mulle__assoc_get`

``` c
void   *_mulle__assoc_get( struct mulle__assoc *assoc,
                           void *key,
                           mulle_pointerpair_compare_t *compare,
                           void *userinfo)
void   *mulle__assoc_get( struct mulle__assoc *assoc,
                          void *key,
                          mulle_pointerpair_compare_t *compare,
                          void *userinfo)
```

Return the value for `key`, or NULL. Sorts lazily, then binary searches.


#### `_mulle__assoc_get_at_index` / `mulle__assoc_get_at_index`

``` c
struct mulle_pointerpair   _mulle__assoc_get_at_index( struct mulle__assoc *assoc,
                                                       size_t index,
                                                       mulle_pointerpair_compare_t *compare,
                                                       void *userinfo)
struct mulle_pointerpair   mulle__assoc_get_at_index( struct mulle__assoc *assoc,
                                                      size_t i,
                                                      mulle_pointerpair_compare_t *compare,
                                                      void *userinfo)
```

Return the pair at sorted index. Sorts lazily if needed.


#### `_mulle__assoc_set_at_index`

``` c
void   _mulle__assoc_set_at_index( struct mulle__assoc *assoc,
                                   size_t i,
                                   void *key,
                                   void *value,
                                   struct mulle_container_keyvaluecallback *callback,
                                   mulle_pointerpair_compare_t *compare,
                                   void *userinfo,
                                   struct mulle_allocator *allocator)
```

Replace the pair at sorted index `i`.


#### `_mulle__assoc_get_in_range` / `mulle__assoc_get_in_range`

``` c
size_t   _mulle__assoc_get_in_range( struct mulle__assoc *assoc,
                                     struct mulle_range range,
                                     struct mulle_pointerpair *buf)
size_t   mulle__assoc_get_in_range( struct mulle__assoc *assoc,
                                    struct mulle_range range,
                                    struct mulle_pointerpair *buf)
```

Copy pairs in range into `buf`. Returns count copied.


#### `_mulle__assoc_get_last`

``` c
struct mulle_pointerpair   _mulle__assoc_get_last( struct mulle__assoc *assoc)
```

Return the last pair.


#### `_mulle__assoc_remove`

``` c
void   _mulle__assoc_remove( struct mulle__assoc *assoc,
                             void *key,
                             struct mulle_container_keyvaluecallback *callback,
                             mulle_pointerpair_compare_t *compare,
                             void *userinfo,
                             struct mulle_allocator *allocator)
```

Remove the pair for `key`. Uses find, then remove-in-range.


#### `_mulle__assoc_remove_in_range`

``` c
void   _mulle__assoc_remove_in_range( struct mulle__assoc *assoc,
                                      struct mulle_range range,
                                      struct mulle_container_keyvaluecallback *callback,
                                      struct mulle_allocator *allocator)
```

Remove pairs in the given index range.


#### `_mulle__assoc_remove_last`

``` c
void   _mulle__assoc_remove_last( struct mulle__assoc *assoc,
                                  struct mulle_container_keyvaluecallback *callback,
                                  struct mulle_allocator *allocator)
```

Pop and release the last pair.


#### `_mulle__assoc_reset`

``` c
void   _mulle__assoc_reset( struct mulle__assoc *assoc,
                            struct mulle_container_keyvaluecallback *callback,
                            struct mulle_allocator *allocator)
```

Remove all elements, keeps allocation.


#### `_mulle__assoc_member`

``` c
int   _mulle__assoc_member( struct mulle__assoc *assoc,
                            void *key,
                            struct mulle_container_keyvaluecallback *callback)
```

Linear membership test using callback's `is_equal`. Does not require sorted
state.


### Search

#### `_mulle__assoc_find_in_range` / `mulle__assoc_find_in_range`

``` c
uintptr_t   _mulle__assoc_find_in_range( struct mulle__assoc *assoc,
                                         void *key,
                                         struct mulle_range range,
                                         struct mulle_container_keyvaluecallback *callback,
                                         mulle_pointerpair_compare_t *compare,
                                         void *userinfo)
uintptr_t   mulle__assoc_find_in_range( struct mulle__assoc *assoc,
                                        void *key,
                                        struct mulle_range range,
                                        struct mulle_container_keyvaluecallback *callback,
                                        mulle_pointerpair_compare_t compare,
                                        void *userinfo)
```

Find the index of `key` within `range`. Returns `mulle_not_found_e` on miss.


#### `_mulle__assoc_find` / `mulle__assoc_find`

``` c
uintptr_t   _mulle__assoc_find( struct mulle__assoc *assoc,
                                void *key,
                                struct mulle_container_keyvaluecallback *callback,
                                mulle_pointerpair_compare_t compare,
                                void *userinfo)
uintptr_t   mulle__assoc_find( struct mulle__assoc *assoc,
                               void *key,
                               struct mulle_container_keyvaluecallback *callback,
                               mulle_pointerpair_compare_t compare,
                               void *userinfo)
```

Find the index of `key` in the full range.


#### `_mulle__assoc_find_callback` / `mulle__assoc_find_callback`

``` c
uintptr_t   _mulle__assoc_find_callback( struct mulle__assoc *assoc,
                                         void *key,
                                         struct mulle_container_keyvaluecallback *callback,
                                         mulle_pointerpair_compare_t *compare,
                                         void *userinfo)
uintptr_t   mulle__assoc_find_callback( struct mulle__assoc *assoc,
                                        void *key,
                                        struct mulle_container_keyvaluecallback *callback,
                                        mulle_pointerpair_compare_t *compare,
                                        void *userinfo)
```

Sort if needed, then search using the callback's equality function.


#### `_mulle__assoc_find_compare` / `mulle__assoc_find_compare`

``` c
uintptr_t   _mulle__assoc_find_compare( struct mulle__assoc *assoc,
                                        void *key,
                                        mulle_pointerpair_compare_t *compare,
                                        void *userinfo)
uintptr_t   mulle__assoc_find_compare( struct mulle__assoc *assoc,
                                       void *key,
                                       mulle_pointerpair_compare_t *compare,
                                       void *userinfo)
```

Search using only the compare function (no callback equality).


### Bulk Operations

#### `_mulle__assoc_is_equal`

``` c
int   _mulle__assoc_is_equal( struct mulle__assoc *assoc,
                              struct mulle__assoc *other,
                              struct mulle_container_keyvaluecallback *callback,
                              mulle_pointerpair_compare_t *compare,
                              void *userinfo)
```

Compare two assocs for equality.


#### `_mulle__assoc_add_assoc_range` / `mulle__assoc_add_assoc`

``` c
void   _mulle__assoc_add_assoc_range( struct mulle__assoc *assoc,
                                      struct mulle__assoc *other,
                                      struct mulle_range range,
                                      struct mulle_container_keyvaluecallback *callback,
                                      struct mulle_allocator *allocator)
void   mulle__assoc_add_assoc( struct mulle__assoc *assoc,
                               struct mulle__assoc *other,
                               struct mulle_container_keyvaluecallback *callback,
                               struct mulle_allocator *allocator)
```

Add pairs from `other` (full range or sub-range) into `assoc`.


#### `_mulle__assoc_copy_items`

``` c
void   _mulle__assoc_copy_items( struct mulle__assoc *dst,
                                 struct mulle__assoc *src,
                                 struct mulle_container_keyvaluecallback *callback,
                                 struct mulle_allocator *allocator)
```

Copy (retain) all items from `src` into `dst`.


### Key Remapping

#### `_mulle__assoc_remap_intptr_key_range` / `mulle__assoc_remap_intptr_key_range`

``` c
void   _mulle__assoc_remap_intptr_key_range( struct mulle__assoc *assoc,
                                             struct mulle_range range,
                                             intptr_t offset,
                                             struct mulle_container_keyvaluecallback *callback,
                                             mulle_pointerpair_compare_t *compare,
                                             void *userinfo)
void   mulle__assoc_remap_intptr_key_range( struct mulle__assoc *assoc,
                                            struct mulle_range range,
                                            intptr_t offset,
                                            struct mulle_container_keyvaluecallback *callback,
                                            mulle_pointerpair_compare_t *compare,
                                            void *userinfo)
```


#### `_mulle__assoc_move_intptr_key_range` / `mulle__assoc_move_intptr_key_range`

``` c
void   _mulle__assoc_move_intptr_key_range( struct mulle__assoc *assoc,
                                            struct mulle_range range,
                                            intptr_t index,
                                            struct mulle_container_keyvaluecallback *callback,
                                            mulle_pointerpair_compare_t *compare,
                                            void *userinfo)
void   mulle__assoc_move_intptr_key_range( struct mulle__assoc *assoc,
                                           struct mulle_range range,
                                           intptr_t index,
                                           struct mulle_container_keyvaluecallback *callback,
                                           mulle_pointerpair_compare_t *compare,
                                           void *userinfo)
```


### Enumeration

#### `_mulle__assoc_enumerate` / `mulle__assoc_enumerate`

``` c
struct mulle__assocenumerator   _mulle__assoc_enumerate( struct mulle__assoc *assoc,
                                                         struct mulle_container_keyvaluecallback *callback,
                                                         mulle_pointerpair_compare_t *compare,
                                                         void *userinfo)
struct mulle__assocenumerator   mulle__assoc_enumerate( struct mulle__assoc *assoc,
                                                        struct mulle_container_keyvaluecallback *callback,
                                                        mulle_pointerpair_compare_t *compare,
                                                        void *userinfo)
```

Start enumeration. Sorts if needed. `callback` provides `notakey`.


#### `_mulle__assocenumerator_next` / `mulle__assocenumerator_next`

``` c
int   _mulle__assocenumerator_next( struct mulle__assocenumerator *rover,
                                    void **key,
                                    void **value)
int   mulle__assocenumerator_next( struct mulle__assocenumerator *rover,
                                   void **key,
                                   void **value)
```

Returns 1 and sets `*key`/`*value`. Returns 0 when exhausted.


#### `_mulle__assocenumerator_done` / `mulle__assocenumerator_done`

``` c
void   _mulle__assocenumerator_done( struct mulle__assocenumerator *rover)
void   mulle__assocenumerator_done( struct mulle__assocenumerator *rover)
```

No-op. Conventional end-of-enumeration marker.


### Debugging

#### `_mulle__assoc_describe` / `mulle__assoc_describe`

``` c
char   *_mulle__assoc_describe( struct mulle__assoc *assoc,
                                struct mulle_container_keyvaluecallback *callback,
                                mulle_pointerpair_compare_t *compare,
                                void *userinfo,
                                struct mulle_allocator *allocator)
char   *mulle__assoc_describe( struct mulle__assoc *assoc,
                               struct mulle_container_keyvaluecallback *callback,
                               mulle_pointerpair_compare_t *compare,
                               void *userinfo,
                               struct mulle_allocator *allocator)
```

Return a malloc'd string description. `allocator` may be NULL for the default.


### Macros

#### `mulle__assoc_do`

``` c
mulle__assoc_do( name, callback)
{
   // use name as struct mulle__assoc *
}
```

Stack-allocate with automatic cleanup.


#### `mulle__assoc_for`

``` c
mulle__assoc_for( name, callback, key, value)
{
   // key and value set each iteration
}
```

For-each enumeration loop.


---

## Predefined Compare Functions

``` c
int   _mulle_pointerpair_compare_pointer_key( struct mulle_pointerpair *a,
                                              struct mulle_pointerpair *b,
                                              void *userinfo)
```

Compares keys by pointer value (subtraction). Default when no compare is given.

``` c
int   _mulle_pointerpair_compare_intptr_key( struct mulle_pointerpair *a,
                                             struct mulle_pointerpair *b,
                                             void *userinfo)
```

Compares keys as `intptr_t` values.

``` c
int   _mulle_pointerpair_compare_string_key( struct mulle_pointerpair *a,
                                             struct mulle_pointerpair *b,
                                             void *userinfo)
```

Compares keys as C strings (char-by-char comparison).


---

## `mulle_pointerpair` Utility Functions

``` c
struct mulle_pointerpair   mulle_pointerpair_make( void *key, void *value)
struct mulle_pointerpair   mulle_pointerpair_make_invalid( void)
int                        mulle_pointerpair_is_invalid( struct mulle_pointerpair pair)
struct mulle_pointerpair   mulle_pointerpair_retain( struct mulle_pointerpair pair,
                                                     struct mulle_container_keyvaluecallback *callback,
                                                     struct mulle_allocator *allocator)
void                       mulle_pointerpair_release( struct mulle_pointerpair pair,
                                                      struct mulle_container_keyvaluecallback *callback,
                                                      struct mulle_allocator *allocator)
```

#### `_mulle_pointerpair_bsearch`

``` c
int   _mulle_pointerpair_bsearch( struct mulle_pointerpair *buf,
                                  size_t n,
                                  struct mulle_pointerpair search,
                                  mulle_pointerpair_compare_t *compare,
                                  void *userinfo)
```

Binary search in a sorted array of pairs. Returns index or -1.


#### `_mulle_pointerpair_bsearch_or_less`

``` c
int   _mulle_pointerpair_bsearch_or_less( struct mulle_pointerpair *buf,
                                          size_t n,
                                          struct mulle_pointerpair search,
                                          mulle_pointerpair_compare_t *compare,
                                          void *userinfo)
```

Binary search returning the matching index or the closest lesser index.


#### `mulle_pointerpair_qsort_r`

``` c
void   mulle_pointerpair_qsort_r( struct mulle_pointerpair *buf,
                                  size_t n,
                                  mulle_pointerpair_compare_t *compare,
                                  void *userinfo)
```

Sort an array of pointer pairs.


---

## `mulle_pointerpairarray` and `mulle__pointerpairarray`

These are the underlying ordered key/value pair arrays that `mulle_assoc`
builds upon. They provide index-based access to `struct mulle_pointerpair`
elements without automatic sorting.


### `mulle__pointerpairarray` Functions

#### Setup

``` c
void   _mulle__pointerpairarray_init( struct mulle__pointerpairarray *array,
                                      size_t capacity,
                                      struct mulle_allocator *allocator)
void   _mulle__pointerpairarray_done( struct mulle__pointerpairarray *array,
                                      struct mulle_allocator *allocator)
struct mulle__pointerpairarray   *mulle__pointerpairarray_create( struct mulle_allocator *allocator)
void   _mulle__pointerpairarray_destroy( struct mulle__pointerpairarray *array,
                                         struct mulle_allocator *allocator)
void   mulle__pointerpairarray_reset( struct mulle__pointerpairarray *array)
```

#### Accessors

``` c
size_t   _mulle__pointerpairarray_get_size( struct mulle__pointerpairarray *array)
size_t   _mulle__pointerpairarray_get_count( struct mulle__pointerpairarray *array)
size_t   _mulle__pointerpairarray_get_guaranteed_size( struct mulle__pointerpairarray *array)
int      _mulle__pointerpairarray_is_full( struct mulle__pointerpairarray *array)
```

#### Operations

``` c
void   _mulle__pointerpairarray_add( struct mulle__pointerpairarray *array,
                                     struct mulle_pointerpair pair,
                                     struct mulle_allocator *allocator)
void   _mulle__pointerpairarray_add_guaranteed( struct mulle__pointerpairarray *array,
                                                struct mulle_pointerpair pair)
void   _mulle__pointerpairarray_grow( struct mulle__pointerpairarray *array,
                                      struct mulle_allocator *allocator)
struct mulle_pointerpair   *_mulle__pointerpairarray_guarantee( struct mulle__pointerpairarray *array,
                                                                size_t length,
                                                                struct mulle_allocator *allocator)
struct mulle_pointerpair   _mulle__pointerpairarray_get( struct mulle__pointerpairarray *array,
                                                         size_t i)
struct mulle_pointerpair   _mulle__pointerpairarray_get_last( struct mulle__pointerpairarray *array)
struct mulle_pointerpair   _mulle__pointerpairarray_set( struct mulle__pointerpairarray *array,
                                                         size_t i,
                                                         struct mulle_pointerpair pair)
struct mulle_pointerpair   _mulle__pointerpairarray_pop( struct mulle__pointerpairarray *array)
size_t   _mulle__pointerpairarray_get_in_range( struct mulle__pointerpairarray *array,
                                                struct mulle_range range,
                                                struct mulle_pointerpair *buf)
void   _mulle__pointerpairarray_remove_in_range( struct mulle__pointerpairarray *array,
                                                 struct mulle_range range)
```

#### Search

``` c
uintptr_t   _mulle__pointerpairarray_find_in_range( struct mulle__pointerpairarray *array,
                                                    struct mulle_pointerpair search,
                                                    struct mulle_range range)
uintptr_t   _mulle__pointerpairarray_find( struct mulle__pointerpairarray *array,
                                           struct mulle_pointerpair search)
uintptr_t   _mulle__pointerpairarray_find_callback( struct mulle__pointerpairarray *array,
                                                    struct mulle_pointerpair search,
                                                    struct mulle_container_keyvaluecallback *callback)
uintptr_t   _mulle__pointerpairarray_find_compare( struct mulle__pointerpairarray *array,
                                                   struct mulle_pointerpair search,
                                                   mulle_pointerpair_compare_t *compare,
                                                   void *userinfo)
void   mulle__pointerpairarray_qsort_r( struct mulle__pointerpairarray *array,
                                        mulle_pointerpair_compare_t *compare,
                                        void *userinfo)
```

#### Enumeration

``` c
struct mulle__pointerpairarrayenumerator   _mulle__pointerpairarray_enumerate( struct mulle__pointerpairarray *array)
int   _mulle__pointerpairarrayenumerator_next( struct mulle__pointerpairarrayenumerator *rover,
                                               struct mulle_pointerpair *pair)
void   _mulle__pointerpairarrayenumerator_done( struct mulle__pointerpairarrayenumerator *rover)
```

#### Macros

``` c
mulle__pointerpairarray_do( name) { ... }
mulle__pointerpairarray_for( name, pair) { ... }
```


### `mulle_pointerpairarray` Functions

A convenience wrapper adding an embedded `allocator` field.

#### Setup

``` c
void   _mulle_pointerpairarray_init( struct mulle_pointerpairarray *array,
                                     size_t capacity,
                                     struct mulle_allocator *allocator)
void   _mulle_pointerpairarray_done( struct mulle_pointerpairarray *array)
struct mulle_pointerpairarray   *mulle_pointerpairarray_create( struct mulle_allocator *allocator)
void   mulle_pointerpairarray_destroy( struct mulle_pointerpairarray *array)
void   mulle_pointerpairarray_reset( struct mulle_pointerpairarray *array)
```

#### Operations

``` c
void   _mulle_pointerpairarray_add( struct mulle_pointerpairarray *array,
                                    struct mulle_pointerpair pair)
void   _mulle_pointerpairarray_add_guaranteed( struct mulle_pointerpairarray *array,
                                               struct mulle_pointerpair pair)
void   _mulle_pointerpairarray_add_kv( struct mulle_pointerpairarray *array,
                                       void *key,
                                       void *value)
void   mulle_pointerpairarray_add( struct mulle_pointerpairarray *array,
                                   struct mulle_pointerpair pair)
void   mulle_pointerpairarray_add_kv( struct mulle_pointerpairarray *array,
                                      void *key,
                                      void *value)
struct mulle_pointerpair   _mulle_pointerpairarray_get( struct mulle_pointerpairarray *array,
                                                        size_t i)
struct mulle_pointerpair   mulle_pointerpairarray_get( struct mulle_pointerpairarray *array,
                                                       size_t i)
struct mulle_pointerpair   mulle_pointerpairarray_get_last( struct mulle_pointerpairarray *array)
struct mulle_pointerpair   mulle_pointerpairarray_pop( struct mulle_pointerpairarray *array)
void   mulle_pointerpairarray_set( struct mulle_pointerpairarray *array,
                                   size_t i,
                                   struct mulle_pointerpair pair)
size_t   _mulle_pointerpairarray_get_in_range( struct mulle_pointerpairarray *array,
                                               struct mulle_range range,
                                               void *buf)
size_t   mulle_pointerpairarray_get_in_range( struct mulle_pointerpairarray *array,
                                              struct mulle_range range,
                                              void *buf)
```

#### Search

``` c
uintptr_t   _mulle_pointerpairarray_find( struct mulle_pointerpairarray *array,
                                          struct mulle_pointerpair search)
uintptr_t   _mulle_pointerpairarray_find_in_range( struct mulle_pointerpairarray *array,
                                                   struct mulle_pointerpair search,
                                                   struct mulle_range range)
uintptr_t   _mulle_pointerpairarray_find_callback( struct mulle_pointerpairarray *array,
                                                   struct mulle_pointerpair search,
                                                   struct mulle_container_keyvaluecallback *callback)
uintptr_t   _mulle_pointerpairarray_find_compare( struct mulle_pointerpairarray *array,
                                                  struct mulle_pointerpair search,
                                                  mulle_pointerpair_compare_t *compare,
                                                  void *userinfo)
void   mulle_pointerpairarray_qsort_r( struct mulle_pointerpairarray *array,
                                       mulle_pointerpair_compare_t *compare,
                                       void *userinfo)
void   mulle_pointerpairarray_qsort( struct mulle_pointerpairarray *array,
                                     mulle_pointerpair_compare_t *compare)
```

#### Enumeration

``` c
struct mulle_pointerpairarrayenumerator   _mulle_pointerpairarray_enumerate( struct mulle_pointerpairarray *array)
struct mulle_pointerpairarrayenumerator   mulle_pointerpairarray_enumerate( struct mulle_pointerpairarray *array)
int   _mulle_pointerpairarrayenumerator_next( struct mulle_pointerpairarrayenumerator *rover,
                                              struct mulle_pointerpair *pair)
int   mulle_pointerpairarrayenumerator_next( struct mulle_pointerpairarrayenumerator *rover,
                                             struct mulle_pointerpair *pair)
void   _mulle_pointerpairarrayenumerator_done( struct mulle_pointerpairarrayenumerator *rover)
void   mulle_pointerpairarrayenumerator_done( struct mulle_pointerpairarrayenumerator *rover)
```

#### Macros

``` c
mulle_pointerpairarray_do( name) { ... }
mulle_pointerpairarray_for( name, pair) { ... }
```
