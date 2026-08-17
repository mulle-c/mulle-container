# `mulle_pointerset` / `mulle__pointerset`

A hashtable-based set that stores unmanaged `void *` pointers. Membership is
determined by pointer equality (`==`). The sentinel (hole marker) is
`mulle_not_a_pointer` (`INTPTR_MIN`), which cannot be stored as an element.

`mulle_pointerset` carries its own `mulle_allocator *`;
`mulle__pointerset` is the minimal variant without an embedded allocator — you
pass the allocator explicitly on each call.

Include with:

```c
#include <mulle-container/mulle-container.h>
```


## Types

```c
struct mulle__pointerset
{
   void     **_storage;
   void     **_initial_storage;
   size_t   _count;
   size_t   _size;
   uintptr_t  _n_mutations;   // only in debug builds
};

struct mulle_pointerset
{
   // embeds MULLE__POINTERSET_BASE
   struct mulle_allocator   *allocator;
};
```


## `mulle_pointerset` Functions

Function names follow the pattern `mulle_pointerset_<verb>`. All functions are
NULL-safe (a NULL `set` pointer is handled gracefully). The underscore-prefixed
variants assert non-NULL `set` in debug but skip the NULL guard.

---

### Creation and Destruction

#### `mulle_pointerset_alloc`

```c
static inline struct mulle_pointerset *
   mulle_pointerset_alloc( struct mulle_allocator *allocator)
```

Allocate an uninitialised pointerset from `allocator`.


#### `mulle_pointerset_init` / `_mulle_pointerset_init`

```c
static inline void   mulle_pointerset_init( struct mulle_pointerset *set,
                                            size_t capacity,
                                            struct mulle_allocator *allocator)
```

Initialise `set` with room for at least `capacity` elements. If `capacity` is
0, the set starts empty (zero-cost until the first insertion).


#### `mulle_pointerset_init_with_static_pointers`

```c
static inline void
   mulle_pointerset_init_with_static_pointers( struct mulle_pointerset *set,
                                               void **storage,
                                               size_t capacity,
                                               struct mulle_allocator *allocator)
```

Initialise using caller-provided `storage` (e.g. a stack array). The storage
must have room for `capacity` pointers. The set will switch to heap allocation
if it outgrows the static buffer.


#### `mulle_pointerset_create`

```c
static inline struct mulle_pointerset *
   mulle_pointerset_create( struct mulle_allocator *allocator)
```

Convenience: `alloc` + `init` with capacity 0.


#### `mulle_pointerset_done` / `_mulle_pointerset_done`

```c
static inline void   mulle_pointerset_done( struct mulle_pointerset *set)
```

Release internal storage. Does **not** free `set` itself (use after stack
allocation or embedding).


#### `mulle_pointerset_destroy`

```c
static inline void   mulle_pointerset_destroy( struct mulle_pointerset *set)
```

`done` + free the `set` struct itself.

---

### Accessors

#### `mulle_pointerset_get_count`

```c
static inline size_t   mulle_pointerset_get_count( struct mulle_pointerset *set)
```

Return the number of pointers currently stored.


#### `mulle_pointerset_get_size`

```c
static inline size_t   mulle_pointerset_get_size( struct mulle_pointerset *set)
```

Return the allocated storage size (always ≥ count).


#### `mulle_pointerset_is_full`

```c
static inline int   mulle_pointerset_is_full( struct mulle_pointerset *set)
```

Returns 1 if the set has reached 75 % occupancy and would grow on the next
insertion.


#### `mulle_pointerset_is_sparse`

```c
static inline int   mulle_pointerset_is_sparse( struct mulle_pointerset *set)
```

Returns 1 if the set's storage could be halved without exceeding the load
limit. Useful to decide whether to shrink.

---

### Operations

#### `mulle_pointerset_get`

```c
static inline void *   mulle_pointerset_get( struct mulle_pointerset *set,
                                             void *p)
```

Look up `p` in the set. Returns `p` if found, `NULL` otherwise.


#### `mulle_pointerset_set`

```c
static inline void   mulle_pointerset_set( struct mulle_pointerset *set,
                                           void *p)
```

Insert or overwrite `p`. If `p` is already present, this is a no-op (pointer
equality). Grows the table as needed.


#### `mulle_pointerset_insert`

```c
static inline int   mulle_pointerset_insert( struct mulle_pointerset *set,
                                             void *p)
```

Insert `p` only if it is **not** already present. Returns 1 on successful
insert, 0 if `p` was already in the set or `set` is NULL.


#### `mulle_pointerset_register`

```c
static inline void *   mulle_pointerset_register( struct mulle_pointerset *set,
                                                  void *p)
```

If `p` is already in the set, return the existing pointer. Otherwise insert `p`
and return `p`. Useful for interning / canonicalising pointers.


#### `mulle_pointerset_remove`

```c
static inline int   mulle_pointerset_remove( struct mulle_pointerset *set,
                                             void *p)
```

Remove `p` from the set. Returns 1 if `p` was found and removed, 0 otherwise.
Does **not** automatically shrink; call `shrink_if_needed` after bulk removals.


#### `mulle__pointerset_reset`

```c
static inline void   mulle__pointerset_reset( struct mulle_pointerset *set)
```

Remove all elements but keep the allocated storage.

---

### Shrinking

#### `mulle_pointerset_shrink`

```c
static inline void   mulle_pointerset_shrink( struct mulle_pointerset *set)
```

Unconditionally reallocate storage to fit the current count.


#### `mulle_pointerset_shrink_if_needed`

```c
static inline void   mulle_pointerset_shrink_if_needed( struct mulle_pointerset *set)
```

Shrink only if `is_sparse` is true.

---

### Copying

#### `mulle_pointerset_copy`

```c
struct mulle_pointerset *   mulle_pointerset_copy( struct mulle_pointerset *set)
```

Allocate and return a new set containing all elements of `set`.


#### `mulle_pointerset_copy_items`

```c
static inline void   mulle_pointerset_copy_items( struct mulle_pointerset *dst,
                                                  struct mulle_pointerset *src)
```

Insert all items from `src` into `dst`.

---

### Enumeration

#### `mulle_pointerset_for` macro

```c
mulle_pointerset_for( set, item)
{
   // item is void * for each element
}
```

Enumerate all elements of `set`. `item` must be a `void *` variable. Do **not**
mutate the set during enumeration (asserted in debug builds).


#### `mulle_pointerset_do` macro

```c
mulle_pointerset_do( name)
{
   // 'name' is a struct mulle_pointerset * valid in this scope
   mulle_pointerset_set( name, ptr);
   // ...
}
// set is automatically done here
```

Declare a stack-scoped pointerset. Calls `mulle_pointerset_done` at scope exit.

---

## `mulle__pointerset` Functions

The double-underscore variant is identical in functionality but does **not**
embed an allocator — you pass it as an extra parameter. This is the layer used
internally by `mulle_pointerset`.

Function names follow `_mulle__pointerset_<verb>`. All require a non-NULL `set`
(asserted in debug).

| Single-underscore (convenience) | Double-underscore equivalent |
|---|---|
| `init( set, cap, alloc )` | `_mulle__pointerset_init( set, cap, alloc )` |
| `done( set )` | `_mulle__pointerset_done( set, alloc )` |
| `destroy( set )` | `_mulle__pointerset_destroy( set, alloc )` |
| `get( set, p )` | `_mulle__pointerset_get( set, p )` |
| `set( set, p )` | `_mulle__pointerset_set( set, p, alloc )` |
| `insert( set, p )` | `_mulle__pointerset_insert( set, p, alloc )` |
| `register( set, p )` | `_mulle__pointerset_register( set, p, alloc )` |
| `remove( set, p )` | `_mulle__pointerset_remove( set, p, alloc )` |
| `shrink( set )` | `_mulle__pointerset_shrink( set, alloc )` |
| `reset( set )` | `_mulle__pointerset_reset( set )` |
| `copy( set )` | `_mulle__pointerset_copy( set, alloc )` |
| `copy_items( dst, src )` | `_mulle__pointerset_copy_items( dst, src, alloc )` |

Additional accessors (no allocator needed):

```c
size_t   _mulle__pointerset_get_count( struct mulle__pointerset *set);
size_t   _mulle__pointerset_get_size( struct mulle__pointerset *set);
int      _mulle__pointerset_is_full( struct mulle__pointerset *set);
int      _mulle__pointerset_is_sparse( struct mulle__pointerset *set);
```


### `mulle__pointerset` Enumeration

#### Direct enumerator

```c
struct mulle__pointersetenumerator   rover;
void  *item;

rover = mulle__pointerset_enumerate( set);   // NULL-safe
while( _mulle__pointersetenumerator_next( &rover, &item))
{
   // use item
}
_mulle__pointersetenumerator_done( &rover);
```

#### `mulle__pointerset_for` macro

```c
mulle__pointerset_for( set, item)
{
   // item is void *
}
```

#### `mulle__pointerset_do` macro

```c
mulle__pointerset_do( name)
{
   _mulle__pointerset_set( name, ptr, NULL);
}
// automatically calls _mulle__pointerset_done( &name__container, NULL)
```

---

## Design Notes

* **Load factor**: the table grows when 75 % full and is considered "sparse"
  when a half-sized table would still stay below 75 %.
* **Hashing**: uses `mulle_pointer_hash` (MurmurHash3 avalanche on the pointer
  value itself). No user-supplied hash function.
* **Collision resolution**: open addressing with linear probing; on collision
  the next available slot is used.
* **Sentinel**: `mulle_not_a_pointer` (`INTPTR_MIN`). This value cannot be
  stored; it marks empty slots.
* **Mutation detection**: in debug builds, modification during enumeration is
  asserted via `_n_mutations`.
* **Initial storage**: `init_with_static_pointers` allows embedding the
  initial table in a stack frame or struct to avoid a heap allocation for
  small, short-lived sets.
