# mulle-container Library Documentation for AI
<!-- Keywords: array, hashtable, map, set, queue, rangeset, allocator -->

## 1. Introduction & Purpose

`mulle-container` is a C11 library of dynamic data structures: arrays, hashtables, queues, sets, and range structures. It is the container layer of the mulle-c platform and is a component of `mulle-core`. It depends on `mulle-allocator` (all memory allocation) and `mulle-data` (the `struct mulle_range` type and range algorithms).

It solves the problem of storing integers, strings, pointers, and arbitrary structs in growable collections without pulling in a heavyweight framework. Key features:

- **Two-tier API**: every structure exists as a single-underscore variant (`mulle_array`, embeds allocator + callbacks) and a double-underscore variant (`mulle__array`, caller passes allocator/callbacks explicitly).
- **Stack or heap allocation**: every structure supports `init`/`done` (stack) and `create`/`destroy` (heap).
- **Callback-driven ownership**: key/value callbacks control hash, equality, retain (copy), release (free), and describe; this makes values "nonowned", "owned" or "copied".
- **Sentinel-based "not found"**: hash-table slots use a `notakey` sentinel (`mulle_not_a_pointer`, `(void *) INTPTR_MIN` by default); lookups that miss return `callback->notakey` or `NULL` depending on the container.
- **NULL-safe convenience layer**: single-underscore public functions tolerate a `NULL` container argument (returning early / `NULL` / `0` as applicable). In debug builds, `_`-prefixed internal helpers assert instead.
- **Not thread-safe**: none of the containers are thread-safe; the caller must provide external locking.

## 2. Key Concepts & Design Philosophy

### Two-tier naming pattern

Every data structure has two layers:

| Variant | Example | Stores allocator/callback | Use case |
|---------|---------|--------------------------|----------|
| Single underscore | `mulle_array` | Yes | Convenience — fewer args per call |
| Double underscore | `mulle__array` | No | Minimal overhead — caller passes allocator |

The single-underscore functions are thin static-inline wrappers that delegate to the double-underscore layer. The double-underscore struct type is layout-compatible at the base (e.g. `struct mulle_array { MULLE__ARRAY_BASE; struct mulle_container_keycallback *callback; struct mulle_allocator *allocator; }`), so a `struct mulle_array *` can be cast to `struct mulle__array *` and vice versa.

### Callback mechanism (key/value lifecycle)

Callbacks are the heart of ownership semantics. A caller-supplied `mulle_container_keycallback` (or `mulle_container_valuecallback`) decides how elements are hashed, compared, copied, freed, and described:

- "nonowned" → don't copy or free
- "owned" → don't copy but free
- "copied" → copy and free

Any key or value pointer passed into a container is subject to the callback's `retain` (copy on insert) and `release` (free on remove/destroy). Pre-baked callbacks are provided (see `mulle-container-callback-global.h`): `mulle_container_keycallback_int`, `_intptr`, `_copied_cstring`, `_nonowned_cstring`, `_owned_cstring`, `_nonowned_pointer`, `_owned_pointer`, `mulle_container_keycallback_nonowned_pointer_or_null` (notakey = `mulle_not_a_pointer`), and matching value callbacks `mulle_container_valuecallback_int`, `_intptr`, `_copied_cstring`, `_nonowned_cstring`, `_owned_cstring`, `_nonowned_pointer`, `_owned_pointer`.

### Sentinel policy

`mulle-container-operation.h` defines the sentinels and documents the policy:

```c
#define mulle_not_a_pointer   ((void *) INTPTR_MIN)

// Use this for "notakey" pointer to differentiate valid int keys
#define mulle_not_an_int      ((void *) INTPTR_MIN)

// Use this for "notakey" pointer to differentiate valid intptr keys
#define mulle_not_an_intptr   ((void *) INTPTR_MIN)
```

- **notakey** (default `mulle_not_a_pointer`): marks empty hash-table slots; cannot be stored as a key/element. Configurable per-instance through `callback->notakey`.
- **`mulle_not_found_e`** (`((uintptr_t) INTPTR_MAX)`, defined in `mulle-data`): out-of-band "not found" sentinel used by scan/search APIs in `mulle_*array`, `mulle__rangeset` and `mulle__rangemap`. Because locations only go up to `mulle_range_location_max == mulle_not_found_e - 1`, it is unambiguous.
- Containers without sentinels (arrays, queues, pointerpairarray) accept any value including `NULL`.

### Integer boxing helpers

The convention for storing `int`/`intptr_t` values is to box them into `void *` and back:

```c
static inline void   *mulle_int_as_pointer( int a)
{
   return( (void *) (intptr_t) a);
}

static inline int   mulle_pointer_as_int( void *a)
{
   return( (int) (intptr_t) a);
}

static inline void   *mulle_intptr_as_pointer( intptr_t a)
{
   return( (void *) a);
}

static inline intptr_t   mulle_pointer_as_intptr( void *a)
{
   return( (intptr_t) a);
}
```

### Write modes

`enum mulle_container_write_mode` defines the three insert-modes used by the internal generic layers (`mulle_container_insert_e`, `mulle_container_overwrite_e`, `mulle_container_update_e`). Exported as:

```c
enum mulle_container_write_mode
{
   mulle_container_insert_e,
   mulle_container_overwrite_e,
   mulle_container_update_e
};
```

### Stack-based RAII-ish macros (`_do`, `_for`)

Every container has macros for stack lifetime and iteration:

- `mulle_<container>_do( name, ...)`: declares a container on the stack, runs the loop body, and calls `done` when the loop exits (break-safe via a confined inner loop).
- `mulle_<container>_for( name, item[, ...])`: iterates all elements without manual enumerator management.

These are ideal for short-lived, scoped use.

## 3. Core API & Data Structures

All signatures below are copied verbatim from the public headers. The single-underscore API is documented; `_`-prefixed functions (e.g. `_mulle_array_get_size`) are non-NULL-safe internal helpers and are only listed by name here. `MULLE__CONTAINER_GLOBAL` and `MULLE_C_NONNULL_*` are compile-time annotation macros, not part of the calling convention.

### 3.1. `mulle-container-callback.h` — Key/Value callbacks

#### `struct mulle_container_keycallback`

- **Purpose:** Controls hashing, equality, retain (copy), release (free) and describe of keys. Its `notakey` field is the "equal to nothing" marker returned by missing-key lookups.
- **Fields:**

```c
struct mulle_container_keycallback
{
   MULLE_CONTAINER_CALLBACK_BASE( key);

   // layout up till here identical to mulle_container_valuecallback FWIW
   void                                     *notakey;   // don't make it INTPTR_MAX as that's mulle_not_found_e
   mulle_container_keycallback_hash_t       *hash;
   mulle_container_keycallback_is_equal_t   *is_equal;
};
```

where `MULLE_CONTAINER_CALLBACK_BASE( type)` expands to the fields:

```c
#define MULLE_CONTAINER_CALLBACK_BASE( type)                      \
   mulle_container_ ## type ## callback_retain_t     *retain;     \
   mulle_container_ ## type ## callback_release_t    *release;    \
   mulle_container_ ## type ## callback_describe_t   *describe;   \
   void                                              *userinfo
```

- **Callback typedefs:**

```c
typedef uintptr_t
   mulle_container_keycallback_hash_t( const struct mulle_container_keycallback *callback,
                                       const void *p);
typedef int
   mulle_container_keycallback_is_equal_t( const struct mulle_container_keycallback *callback,
                                           const void *p,
                                           const void *q);
typedef void *
   mulle_container_keycallback_retain_t( const struct mulle_container_keycallback *callback,
                                         void *p,
                                         struct mulle_allocator *allocator);
typedef void
   mulle_container_keycallback_release_t( const struct mulle_container_keycallback *callback,
                                          void *p,
                                          struct mulle_allocator *allocator);
typedef char *
   mulle_container_keycallback_describe_t( const struct mulle_container_keycallback *callback,
                                           void *p,
                                           struct mulle_allocator **p_allocator);
```

If `describe` returns a static string, `*p_allocator` is set to `NULL` (auto-release); otherwise the string was allocated with `*p_allocator`.

- **Accessors:** `_mulle_container_keycallback_get_notakey`, `mulle_container_keycallback_get_notakey`, `_mulle_container_keycallback_get_userinfo`, `mulle_container_keycallback_get_userinfo`, plus `assert_mulle_container_keycallback` (debug: asserts hash/is_equal/retain/release/describe are set).
- **Standalone key callback functions:**
  - `mulle_container_keycallback_intptr_is_equal(...)`
  - `mulle_container_keycallback_pointer_hash(...)`
  - `mulle_container_keycallback_pointer_is_equal(...)`
  - `_mulle_container_keycallback_pointer_free(...)`
  - `mulle_container_keycallback_cstring_hash(...)`
  - `mulle_container_keycallback_cstring_is_equal(...)`

#### `struct mulle_container_valuecallback`

- **Purpose:** Controls retain/release/describe of values. Same base layout as the keycallback (without `notakey`/`hash`/`is_equal`).
- **Fields:** only the `MULLE_CONTAINER_CALLBACK_BASE( value)` fields (`retain`, `release`, `describe`, `userinfo`).
- **Typedefs:** `mulle_container_valuecallback_retain_t`, `mulle_container_valuecallback_release_t`, `mulle_container_valuecallback_describe_t` (same shapes as the key variants).

```c
typedef void *
   mulle_container_valuecallback_retain_t( const struct mulle_container_valuecallback *callback,
                                           void *p,
                                           struct mulle_allocator *allocator);
typedef void
   mulle_container_valuecallback_release_t( const struct mulle_container_valuecallback *callback,
                                            void *p,
                                            struct mulle_allocator *allocator);

typedef char *
   mulle_container_valuecallback_describe_t( const struct mulle_container_valuecallback *callback,
                                             void *p,
                                             struct mulle_allocator **allocator);
```

- **Global value callback functions:**
  - `mulle_container_valuecallback_self` (retain = identity)
  - `mulle_container_valuecallback_nop` (release = no-op)
  - `mulle_container_valuecallback_no_description`
  - `mulle_container_valuecallback_pointer_free`
  - `mulle_container_callback_int_describe`, `mulle_container_callback_intptr_describe`, `mulle_container_callback_pointer_describe`, `mulle_container_callback_cstring_describe`
  - `mulle_container_callback_cstring_copy(...)`

#### `struct mulle_container_keyvaluecallback`

- **Purpose:** Combines a key and a value callback for containers that store pairs (map, assoc).

```c
struct mulle_container_keyvaluecallback
{
   struct mulle_container_keycallback     keycallback;
   struct mulle_container_valuecallback   valuecallback;
};
```

- **Shortcut queries:** `mulle_container_keyvaluecallback_retains( callback)` and `mulle_container_keyvaluecallback_releases( callback)` return nonzero if either sub-callback actually retains/releases (lets callers skip per-element callback overhead in hot loops).

### 3.2. `mulle-container-operation.h` and support headers

- `mulle-container-operation.h` — `enum mulle_container_write_mode`, the notakey sentinels (`mulle_not_a_pointer`, `mulle_not_an_int`, `mulle_not_an_intptr`) and the int-boxing helpers `mulle_int_as_pointer`, `mulle_pointer_as_int`, `mulle_intptr_as_pointer`, `mulle_pointer_as_intptr` (all shown in Section 2).
- `mulle-container-math.h` — power-of-two helpers:
  - `mulle_is_pow2( size_t x)`
  - `mulle_pow2roundup_32( uint32_t v)`, `mulle_pow2roundup_64( uint64_t v)`, `mulle_pow2roundup( size_t v)`
  - `mulle_pow2rounddown_32( uint32_t v)`, `mulle_pow2rounddown_64( uint64_t v)`, `mulle_pow2rounddown( size_t v)`
  - compatibility names `mulle_pow2round_32/64/size_t` (`roundup` aliases). `0` promotes to `1` for roundup purposes.
- `mulle-container-callback-global.h` — declares the pre-baked key value callback instances listed in Section 2.
- `mulle-container-compiler-attributes.h` — placeholder for `MULLE_C_NONNULL_FIRST`; effectively empty.
- `mulle-container.h` — the umbrella header; defines `MULLE__CONTAINER_VERSION` and includes `include.h` and the version check. Users typically `#include <mulle-container/mulle-container.h>`.

### 3.3. `mulle-array.h` — `mulle_array` (managed pointer array)

- **Purpose:** A growable array of `void *` that applies a key callback on every insert/remove (hence "managed": it can deep-copy `copied_cstring` values on add and free them on remove/destroy). Index-based access. Cannot store `callback->notakey` (assert in debug); a missing value is reported as `callback->notakey`.

```c
struct mulle_array
{
   MULLE_ARRAY_BASE;
};
```

`MULLE_ARRAY_BASE` expands to `MULLE__ARRAY_BASE` (the `mulle__pointerarray` storage fields `_storage`, `_curr`, `_sentinel`, `_initial_storage` and, unless `MULLE__CONTAINER_MISER_MODE`, a mutation counter `_n_mutations`), plus:

```c
#define MULLE_ARRAY_BASE                                         \
   MULLE__ARRAY_BASE;                                            \
   struct mulle_container_keycallback   *callback;               \
   struct mulle_allocator               *allocator
```

- **Lifecycle:**

```c
MULLE_C_NONNULL_THIRD
static inline void    mulle_array_init( struct mulle_array *array,
                                        size_t length,
                                        struct mulle_container_keycallback *callback,
                                        struct mulle_allocator *allocator)
```

```c
#define mulle_array_init_default( array, callback)   \
   mulle_array_init( array, 8, callback, NULL)
```

```c
static inline void   mulle_array_done( struct mulle_array *array)
```

```c
MULLE__CONTAINER_GLOBAL
struct mulle_array    *
   mulle_array_create( size_t capacity,
                       struct mulle_container_keycallback *callback,
                       struct mulle_allocator *allocator);
```

```c
#define mulle_array_create_default( callback)   \
   mulle_array_create( 8, callback, NULL)
```

```c
static inline void   mulle_array_destroy( struct mulle_array *array)
```

- **Core operations:**

```c
static inline void    mulle_array_add( struct mulle_array *array,
                                       void  *p)
```

```c
static inline void    mulle_array_set( struct mulle_array *array,
                                       size_t i,
                                       void  *p)
```

```c
static inline void   *mulle_array_get( struct mulle_array *array, size_t index)
```

```c
static inline void   *mulle_array_get_last( struct mulle_array *array)
```

```c
static inline void   mulle_array_remove_last( struct mulle_array *array)
```

```c
static inline void   mulle_array_remove( struct mulle_array *array,
                                         void *item)
```

```c
// Remove first occurence of item. Other occurences of item will remain.
// Will remove from back to front
static inline void   mulle_array_remove_unique( struct mulle_array *array,
                                              void *item)
```

```c
static inline void   mulle_array_remove_in_range( struct mulle_array *array,
                                                  struct mulle_range range)
```

```c
static inline void   mulle_array_reset( struct mulle_array *array)
```

```c
static inline void
   mulle_array_add_array( struct mulle_array *array,
                          struct mulle_array *other,
                          struct mulle_range range)
```

```c
static inline struct mulle_array   *mulle_array_copy( struct mulle_array *array)
```

`mulle_array_set`/`mulle_array_add` apply the callback's `retain` (so managed copies happen automatically); `remove`/`reset`/`done`/`destroy` apply `release`.

- **Inspection:**

```c
static inline size_t
   mulle_array_get_size( struct mulle_array *array)
```

```c
static inline size_t
	mulle_array_get_guaranteed_size( struct mulle_array *array)
```

```c
static inline size_t
   mulle_array_get_count( struct mulle_array *array)
```

```c
static inline int
   mulle_array_is_full( struct mulle_array *array)
```

```c
static inline struct mulle_container_keycallback *
   mulle_array_get_callback( struct mulle_array *array)
```

```c
static inline struct mulle_allocator *
   mulle_array_get_allocator( struct mulle_array *array)
```

```c
static inline size_t
   mulle_array_get_in_range( struct mulle_array *array,
                             struct mulle_range range,
                             void *buf)
```

- **Memory ops:**

```c
static inline void **
   mulle_array_guarantee( struct mulle_array *array,
                          size_t length)
```

```c
static inline void
   mulle_array_grow( struct mulle_array *array)
```

- **Search / sorting:**

```c
static inline uintptr_t
   mulle_array_find( struct mulle_array *array, void *obj)
```

```c
static inline uintptr_t
   mulle_array_find_in_range( struct mulle_array *array,
                              void *obj,
                              struct mulle_range range)
```

```c
static inline uintptr_t
   mulle_array_find_in_range_identical( struct mulle_array *array,
                                        void *obj,
                                        struct mulle_range range)
```

```c
static inline int   mulle_array_member( struct mulle_array *array,
                                        void *p)
```

```c
// will use callbacks of array to determine equality
static inline int   mulle_array_is_equal( struct mulle_array *array,
                                          struct mulle_array *other)
```

```c
static inline void
   mulle_array_qsort_r_inline( struct mulle_array *array,
                               mulle_pointerarray_cmp_t *compare,
                               void *userinfo)
```

`find` and friends return `mulle_not_found_e` (`(uintptr_t) INTPTR_MAX`) when the item is missing.

- **Debug:** `mulle_array_describe( struct mulle_array *array)` — returns a newly allocated (default allocator) description string.

- **Enumeration:**

```c
struct mulle_arrayenumerator
{
   MULLE_ARRAYENUMERATOR_BASE;
};
```

```c
static inline struct mulle_arrayenumerator
   mulle_array_enumerate( struct mulle_array *array)
```

```c
// item is unchanged if rval is 0 (!)
static inline int
   mulle_arrayenumerator_next( struct mulle_arrayenumerator *rover, void **item)
```

```c
static inline void   mulle_arrayenumerator_done( struct mulle_arrayenumerator *rover)
```

Reverse enumeration uses `mulle_arrayreverseenumerator` with `mulle_array_reverseenumerate`, `mulle_arrayreverseenumerator_next`, `mulle_arrayreverseenumerator_done`. Enumerations are invalidated by any mutation of the array (asserted in mutation-count builds); newly added elements during iteration are not retrieved.

- **Macros:** `mulle_array_do( name, callback)`, `mulle_array_do_flexible( name, stackcount, callback)` (stack storage), `mulle_array_for( name, item)`, `mulle_array_for_reverse( name, item)`. Also `MULLE_ARRAY_DATA( storage, count, callback, allocator)` for explicit stack storage.

The base `mulle__array` (in `mulle--array.h`) provides the equivalent double-underscore API (`mulle__array_init( array, length, allocator)`, `mulle__array_add( array, p, callback, allocator)`, `mulle__array_done( array, callback, allocator)`, etc.) with the callback/allocator passed per call.

### 3.4. `mulle-pointerarray.h` — `mulle_pointerarray` (unmanaged pointer array)

- **Purpose:** A growable array of `void *` with no copy/free callbacks — the raw storage layer. Stores pointers as-is (including `NULL`). All indices are `size_t`.

```c
struct mulle_pointerarray
{
   MULLE_POINTERARRAY_BASE;
};
```

`MULLE_POINTERARRAY_BASE` expands to `MULLE__POINTERARRAY_BASE` (fields `_storage`, `_curr`, `_sentinel`, `_initial_storage` plus the mutation counter) and an allocator:

```c
#define MULLE_POINTERARRAY_BASE          \
   MULLE__POINTERARRAY_BASE;             \
   struct mulle_allocator   *allocator    // public...
```

- **Lifecycle:**

```c
static inline void   mulle_pointerarray_init( struct mulle_pointerarray *array,
                                              size_t capacity,
                                              struct mulle_allocator *allocator)
```

```c
#define mulle_pointerarray_init_default( array)  \
   mulle_pointerarray_init( array, 8, NULL)
```

```c
static inline void
   mulle_pointerarray_init_with_static_pointer( struct mulle_pointerarray *array,
                                                void **pointers,
                                                size_t capacity,
                                                struct mulle_allocator *allocator)
```

```c
static inline void   mulle_pointerarray_done( struct mulle_pointerarray *array)
```

```c
static inline struct mulle_pointerarray *
   mulle_pointerarray_create( struct mulle_allocator *allocator)
```

```c
#define mulle_pointerarray_create_default()  \
   mulle_pointerarray_create( NULL)
```

```c
static inline void   mulle_pointerarray_destroy( struct mulle_pointerarray *array)
```

```c
static inline void   mulle_pointerarray_absorb_array( struct mulle_pointerarray *array,
                                                      struct mulle_pointerarray *victim)
```

- **Core operations:**

```c
static inline void
   mulle_pointerarray_add( struct mulle_pointerarray *array, void  *p)
```

```c
static inline void  *
   mulle_pointerarray_get( struct mulle_pointerarray *array, size_t i)
```

```c
static inline void  *
   mulle_pointerarray_get_last( struct mulle_pointerarray *array)
```

```c
static inline void *
   mulle_pointerarray_pop( struct mulle_pointerarray *array)
```

```c
static inline void   *mulle_pointerarray_set( struct mulle_pointerarray *array,
                                              size_t i,
                                              void *p)
```

```c
static inline void   mulle_pointerarray_set_zeroing( struct mulle_pointerarray *array,
                                                     size_t i,
                                                     void *value)
```

```c
static inline void   *mulle_pointerarray_get_zeroing( struct mulle_pointerarray *array,
                                                      size_t i)
```

```c
static inline void
   mulle_pointerarray_remove( struct mulle_pointerarray *array,
                              void *p)
```

```c
static inline
void   mulle_pointerarray_remove_in_range( struct mulle_pointerarray *array,
                                           struct mulle_range range)
```

```c
static inline
void   mulle_pointerarray_remove_at_index( struct mulle_pointerarray *array,
                                           uintptr_t location)
```

```c
static inline
void   *mulle_pointerarray_remove_unique( struct mulle_pointerarray *array,
                                          void *p)
```

```c
static inline
void   mulle_pointerarray_insert_in_range( struct mulle_pointerarray *array,
                                           struct mulle_range range,
                                           void **pointers)
```

```c
static inline void
   mulle_pointerarray_insert( struct mulle_pointerarray *array,
                              uintptr_t location,
                              void *p)
```

```c
static inline void
   mulle_pointerarray_add_pointerarray( struct mulle_pointerarray *array,
                                        struct mulle_pointerarray *other,
                                        struct mulle_range range)
```

```c
static inline void
   mulle_pointerarray_reset( struct mulle_pointerarray *array)
```

```c
static inline int   mulle_pointerarray_is_equal( struct mulle_pointerarray *array,
                                                 struct mulle_pointerarray *other)
```

- **Inspection / memory:**

```c
static inline size_t
   mulle_pointerarray_get_size( struct mulle_pointerarray *array)
```

```c
static inline size_t
   mulle_pointerarray_get_count( struct mulle_pointerarray *array)
```

```c
static inline size_t
	mulle_pointerarray_get_guaranteed_size( struct mulle__pointerarray *array)
```

```c
static inline int
   mulle_pointerarray_is_full( struct mulle_pointerarray *array)
```

```c
static inline struct mulle_allocator *
   mulle_pointerarray_get_allocator( struct mulle_pointerarray *array)
```

```c
static inline void **
   mulle_pointerarray_guarantee( struct mulle_pointerarray *array,
                                 size_t length)
```

```c
static inline void
   mulle_pointerarray_grow( struct mulle_pointerarray *array)
```

```c
static inline void
   mulle_pointerarray_compact( struct mulle_pointerarray *array,
                               void *notakey)
```

- **Search / sort:**

```c
static inline uintptr_t
   mulle_pointerarray_find( struct mulle_pointerarray *array, void *p)
```

```c
static inline int   
  mulle_pointerarray_member( struct mulle_pointerarray *array, void *p)
```

```c
static inline void
   mulle_pointerarray_qsort_r( struct mulle_pointerarray *array,
                               mulle_pointerarray_cmp_t *compare,
                               void *userinfo)
```

```c
static inline void
   mulle_pointerarray_qsort_r_inline( struct mulle_pointerarray *array,
                                      mulle_pointerarray_cmp_t *compare,
                                      void *userinfo)
```

with comparator type:

```c
typedef int   mulle_pointerarray_cmp_t( void **, void **, void *);
```

- **Enumeration & macros:** `mulle_pointerarrayenumerator` / `mulle_pointerarrayreverseenumerator` with the usual `mulle_pointerarray_enumerate`, `mulle_pointerarrayenumerator_next`, `mulle_pointerarrayenumerator_done` (and reverse variants); macros `mulle_pointerarray_do( name)`, `mulle_pointerarray_do_flexible( name, stackcount)`, `mulle_pointerarray_for( name, item)`, `mulle_pointerarray_for_reverse( name, item)`.
- **Helper:** `struct mulle_pointers { void **pointers; size_t count; }` with `mulle_pointers_make( void **pointers, size_t count)` and `mulle_pointers_done( struct mulle_pointers p, struct mulle_allocator *allocator)` in the double-underscore layer.

### 3.5. `mulle-structarray.h` — `mulle_structarray` (array of arbitrary structs)

- **Purpose:** A growable array that stores raw bytes of an arbitrary C type (raw `memcpy` semantics, not pointer-managed). The element type is described by `sizeof`/`alignof`; elements are stored aligned (`MULLE__STRUCTARRAY_ALIGNED_SIZE`).

```c
struct mulle_structarray
{
   MULLE__STRUCTARRAY_BASE;
   struct mulle_allocator   *allocator;
};
```

`MULLE__STRUCTARRAY_BASE` expands to `void *_storage; void *_curr; void *_sentinel; void *_initial_storage; size_t _sizeof_struct; size_t _copy_sizeof_struct;` plus the mutation counter.

- **Lifecycle:**

```c
static inline void   mulle_structarray_init( struct mulle_structarray *array,
                                             size_t sizeof_struct,
                                             size_t alignof_struct,
                                             size_t capacity,
                                             struct mulle_allocator *allocator)
```

```c
#define mulle_structarray_init_default( array, type)   \
   mulle_structarray_init( array, sizeof( type), alignof( type), 8, NULL)
```

```c
static inline struct mulle_structarray *
   mulle_structarray_create( size_t sizeof_struct,
                             size_t alignof_struct,
                             size_t capacity,
                             struct mulle_allocator *allocator)
```

```c
#define mulle_structarray_create_default( type)   \
   mulle_structarray_create( sizeof( type), alignof( type), 8, NULL)
```

```c
static inline void  mulle_structarray_done( struct mulle_structarray *array)
```

```c
static inline void  mulle_structarray_destroy( struct mulle_structarray *array)
```

```c
static inline void
   mulle_structarray_init_with_static_storage( struct mulle_structarray *array,
                                               size_t sizeof_struct,
                                               size_t alignof_struct,
                                               size_t count,
                                               void  *storage,
                                               struct mulle_allocator *allocator)
```

- **Core operations:**

```c
static inline void
   mulle_structarray_add( struct mulle_structarray *array, void *item)
```

```c
static inline int
   mulle_structarray_pop( struct mulle_structarray *array, void *item)
```

```c
static inline void
   mulle_structarray_set( struct mulle_structarray *array, size_t i, void *item)
```

```c
static inline void *
   mulle_structarray_get( struct mulle_structarray *array, size_t i)
```

```c
// lenient will return NULL if index is out of bounds
static inline void *
   mulle_structarray_get_lenient( struct mulle_structarray *array, size_t i)
```

```c
static inline void *
   _mulle_structarray_get_last( struct mulle_structarray *array)
```

```c
static inline void *
   mulle_structarray_reserve( struct mulle_structarray *array)
```

Note: `mulle_structarray_reserve` returns a pointer to a reserved (uninitialized) element slot; `mulle_structarray_add` copies into the next slot. `get` returns a pointer to the element at index `i`. Elements are copied with `memcpy` using `_copy_sizeof_struct`.

- **Bulk / index helpers:**

```c
static inline void
   mulle_structarray_add_structarray( struct mulle_structarray *array,
                                      struct mulle_structarray *other,
                                      struct mulle_range range)
```

```c
static inline struct mulle_structarray *
   mulle_structarray_copy( struct mulle_structarray *other,
                           struct mulle_allocator *allocator)
```

```c
static inline void   _mulle_structarray_set_count( struct mulle_structarray *array,
                                                   size_t count)
```

```c
static inline void   _mulle_structarray_zero_to_count( struct mulle_structarray *array)
```

```c
static inline void   _mulle_structarray_remove_in_range( struct mulle_structarray *array,
                                                         struct mulle_range range)
```

```c
static inline void   _mulle_structarray_remove_at_index( struct mulle_structarray *array,
                                                         uintptr_t location)
```

```c
static inline void   _mulle_structarray_remove_last( struct mulle_structarray *array)
```

```c
static inline void   _mulle_structarray_insert_in_range( struct mulle_structarray *array,
                                                         struct mulle_range range,
                                                         void *buf)
```

```c
static inline void   _mulle_structarray_insert( struct mulle_structarray *array,
                                                uintptr_t location,
                                                void *item)
```

```c
static inline void   _mulle_structarray_size_to_fit( struct mulle_structarray *array)
```

- **Inspection:**

```c
static inline size_t
   mulle_structarray_get_count( struct mulle_structarray *array)
```

```c
static inline size_t
   _mulle_structarray_get_used_as_length( struct mulle_structarray *array)
```

```c
static inline size_t
   mulle_structarray_get_size( struct mulle_structarray *array)
```

```c
static inline size_t
   mulle_structarray_get_size_as_length( struct mulle_structarray *array)
```

```c
static inline size_t
   mulle_structarray_get_element_size( struct mulle_structarray *array)
```

```c
static inline size_t
   mulle_structarray_get_struct_size( struct mulle_structarray *array)
```

```c
static inline struct mulle_allocator  *
   mulle_structarray_get_allocator( struct mulle_structarray *array)
```

```c
static inline void *
   mulle_structarray_get_current( struct mulle_structarray *array)
```

```c
static inline void *
   mulle_structarray_get_first( struct mulle_structarray *array)
```

- **Sorting:** `mulle_structarray_qsort_r_inline( array, mulle_structarray_cmp_t *compare, void *userinfo)` and `mulle_structarray_qsort_r(...)` with `typedef int mulle_structarray_cmp_t( void *, void *, void *);`.

- **Enumeration & macros:** `mulle_structarrayenumerator` / `mulle_structarrayreverseenumerator`, `mulle_structarray_enumerate`, `mulle_structarrayenumerator_next`, `mulle_structarrayenumerator_done` (+ reverse), and macros `mulle_structarray_do( name, type)`, `mulle_structarray_do_flexible( name, stackcount, type)`, `mulle_structarray_for( name, item)`, `mulle_structarray_for_reverse( name, item)`.

### 3.6. `mulle-assoc.h` — `mulle_assoc` (sorted key/value pairs)

- **Purpose:** A sorted array of key/value pairs kept (lazily) ordered by a comparison function; lookup uses binary search. It is a `mulle_pointerpairarray` with a keyvalue callback, compare function and allocator embedded.

```c
struct mulle_assoc
{
   MULLE_ASSOC_BASE;
};
```

`MULLE_ASSOC_BASE` expands to the pointerpairarray storage, an `int _is_sorted` flag, and:

```c
#define MULLE_ASSOC_BASE                                  \
   MULLE__ASSOC_BASE;                                     \
   struct mulle_container_keyvaluecallback   *callback;   \
   mulle_pointerpair_compare_t               *compare;    \
   struct mulle_allocator                    *allocator
```

The pair type is `struct mulle_pointerpair { void *key; void *value; };`.

- **Lifecycle:**

```c
MULLE_C_NONNULL_THIRD
static inline void    mulle_assoc_init( struct mulle_assoc *assoc,
                                        size_t length,
                                        struct mulle_container_keyvaluecallback *callback,
                                        mulle_pointerpair_compare_t *compare,
                                        struct mulle_allocator *allocator)
```

```c
static inline void   mulle_assoc_done( struct mulle_assoc *assoc)
```

```c
MULLE__CONTAINER_GLOBAL
struct mulle_assoc    *
   mulle_assoc_create( size_t capacity,
                       struct mulle_container_keyvaluecallback *callback,
                       mulle_pointerpair_compare_t *compare,
                       struct mulle_allocator *allocator);
```

```c
static inline void   mulle_assoc_destroy( struct mulle_assoc *assoc)
```

- **Core operations:**

```c
static inline
void   *mulle_assoc_get( struct mulle_assoc *assoc, const void *key)
```

```c
static inline void
   mulle_assoc_set( struct mulle_assoc *assoc, void *key, void *value)
```

```c
static inline void
   mulle_assoc_add( struct mulle_assoc *assoc, void *key, void *value)
```

```c
static inline void   mulle_assoc_remove( struct mulle_assoc *assoc,
                                         const void *key)
```

```c
static inline void   mulle_assoc_remove_last( struct mulle_assoc *assoc)
```

```c
static inline void   mulle_assoc_reset( struct mulle_assoc *assoc)
```

```c
static inline
struct mulle_pointerpair  mulle_assoc_get_at_index( struct mulle_assoc *assoc,
                                                    size_t index)
```

```c
static inline
void  mulle_assoc_set_at_index( struct mulle_assoc *assoc,
                                size_t index,
                                void *key,
                                void *value)
```

```c
static inline struct mulle_pointerpair
   mulle_assoc_get_last( struct mulle_assoc *assoc)
```

```c
static inline void
   mulle_assoc_add_assoc( struct mulle_assoc *assoc,
                          struct mulle_assoc *other)
```

```c
static inline struct mulle_assoc   *mulle_assoc_copy( struct mulle_assoc *assoc)
```

```c
static inline int   mulle_assoc_member( struct mulle_assoc *assoc,
                                        const void *key)
```

```c
static inline int   mulle_assoc_is_equal( struct mulle_assoc *assoc,
                                          struct mulle_assoc *other)
```

```c
static inline void
   mulle_assoc_set_compare( struct mulle_assoc *assoc,
                             mulle_pointerpair_compare_t *compare)
```

- **Search (binary search on sorted keys):**

```c
static inline uintptr_t
   mulle_assoc_find( struct mulle_assoc *assoc, const void *key)
```

```c
static inline uintptr_t
   mulle_assoc_find_in_range( struct mulle_assoc *assoc,
                              const void *key,
                              struct mulle_range range)
```

- **Inspection:**

```c
static inline size_t
   mulle_assoc_get_count( struct mulle_assoc *assoc)
```

```c
static inline size_t
   mulle_assoc_get_size( struct mulle_assoc *assoc)
```

```c
static inline int
   mulle_assoc_is_sorted( struct mulle_assoc *assoc)
```

```c
static inline struct mulle_container_keyvaluecallback *
   mulle_assoc_get_callback( struct mulle_assoc *assoc)
```

```c
static inline mulle_pointerpair_compare_t *
   mulle_assoc_get_compare( struct mulle_assoc *assoc)
```

```c
static inline struct mulle_allocator *
   mulle_assoc_get_allocator( struct mulle_assoc *assoc)
```

```c
static inline void   *mulle_assoc_get_notakey( struct mulle_assoc *assoc)
```

- **Enumeration:** `mulle_assocenumerator`, `mulle_assoc_enumerate( assoc)`, `mulle_assocenumerator_next( rover, &key, &value)`, `mulle_assocenumerator_done( rover)`; macro `mulle_assoc_for( name, key, value)`.

`mulle_assoc` lazily resorts (via its compare) before binary-search operations; the double-underscore `mulle--assoc.h` exposes `mulle__assoc_qsort_r`, `mulle__assoc_find_compare`, etc. when an explicit compare is needed.

### 3.7. `mulle-pointerpairarray.h` — `mulle_pointerpairarray` and `mulle_pointerpair`

- **Purpose:** A growable array of `(key, value)` pairs (order of insertion). Pairs are plain data — no ownership — but `find` variants can take a callback or compare function.

```c
struct mulle_pointerpairarray
{
   MULLE__POINTERPAIRARRAY_BASE;
   struct mulle_allocator      *allocator;
};
```

```c
struct mulle_pointerpair
{
   void   *key;
   void   *value;
};
```

- **Lifecycle:** `mulle_pointerpairarray_alloc( allocator)`, `mulle_pointerpairarray_init( array, capacity, allocator)`, `mulle_pointerpairarray_done( array)`, `mulle_pointerpairarray_create( allocator)`, `mulle_pointerpairarray_destroy( array)`, `mulle_pointerpairarray_reset( array)`.
- **Core operations:**

```c
static inline void
   mulle_pointerpairarray_add( struct mulle_pointerpairarray *array,
                               struct mulle_pointerpair pair)
```

```c
static inline void
   mulle_pointerpairarray_add_kv( struct mulle_pointerpairarray *array,
                                  void *key,
                                  void *value)
```

```c
static inline struct mulle_pointerpair
   mulle_pointerpairarray_get( struct mulle_pointerpairarray *array,
                               size_t i)
```

```c
static inline struct mulle_pointerpair
	mulle_pointerpairarray_get_last( struct mulle_pointerpairarray *array)
```

```c
static inline struct mulle_pointerpair
	mulle_pointerpairarray_pop( struct mulle_pointerpairarray *array)
```

```c
static inline void
	mulle_pointerpairarray_set( struct mulle_pointerpairarray *array,
                               size_t i,
                               struct mulle_pointerpair pair)
```

- **Search / sort (see `mulle-pointerpair.h` for `mulle_pointerpair_compare_t`):**

```c
static inline uintptr_t
   mulle_pointerpairarray_find( struct mulle_pointerpairarray *array,
                                struct mulle_pointerpair search)
```

```c
static inline uintptr_t
   mulle_pointerpairarray_find_compare( struct mulle_pointerpairarray *array,
                                        struct mulle_pointerpair search,
                                        mulle_pointerpair_compare_t *compare,
                                        void *userinfo)
```

```c
static inline uintptr_t
   mulle_pointerpairarray_find_callback( struct mulle_pointerpairarray *array,
                                         struct mulle_pointerpair search,
                                         const struct mulle_container_keyvaluecallback *callback)
```

```c
static inline void   mulle_pointerpairarray_qsort_r( struct mulle_pointerpairarray *array,
                                                     mulle_pointerpair_compare_t *compare,
                                                     void *userinfo)
```

- **Enumeration:** `mulle_pointerpairarray_enumerate( array)`, `mulle_pointerpairarrayenumerator_next( rover, &pair)`, `mulle_pointerpairarrayenumerator_done( rover)`; macro `mulle_pointerpairarray_for( name, pair)`.

### 3.8. `mulle-map.h` — `mulle_map` (hashtable key/value)

- **Purpose:** The classic string/pointer-key dictionary. Uses a key callback for hash+equality and a value callback for ownership. Keys are hashed into storage; the map auto-grows when 75% full and can shrink when sparse.

```c
struct mulle_map
{
   MULLE_MAP_BASE;
};
```

`MULLE_MAP_BASE` expands to the `mulle__pointermap` storage (`void **_storage; size_t _count; size_t _size;` + mutation counter) and:

```c
#define MULLE_MAP_BASE                                   \
   MULLE__MAP_BASE;                                      \
   struct mulle_container_keyvaluecallback   *callback;  \
   struct mulle_allocator                    *allocator
```

- **Lifecycle:**

```c
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_SECOND
struct mulle_map   *mulle_map_create( size_t capacity,
                                      struct mulle_container_keyvaluecallback *callback,
                                      struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_THIRD
void   _mulle_map_init( struct mulle_map *map,
                        size_t capacity,
                        struct mulle_container_keyvaluecallback *callback,
                        struct mulle_allocator *allocator);

static inline void   mulle_map_init( struct mulle_map *map,
                                     size_t capacity,
                                     struct mulle_container_keyvaluecallback *callback,
                                     struct mulle_allocator *allocator)

MULLE_C_NONNULL_FIRST
static inline void    _mulle_map_done( struct mulle_map *map)

static inline void   mulle_map_done( struct mulle_map *map)

MULLE_C_NONNULL_FIRST
static inline void   _mulle_map_destroy( struct mulle_map *map)

static inline void   mulle_map_destroy( struct mulle_map *map)
```

- **Lookup:**

```c
MULLE_C_NONNULL_FIRST
static inline void   *_mulle_map_get( struct mulle_map *map, const void *key)

static inline void   *mulle_map_get( struct mulle_map *map, const void *key)
```

`mulle_map_get` returns the **value** for a key, or `NULL` when the key is absent. (`NULL` is therefore an ambiguous "not found" value — use `mulle_map_contains` to disambiguate.)

```c
static inline int   mulle_map_contains( struct mulle_map *map, const void *key)
```

```c
MULLE_C_NONNULL_FIRST_THIRD
static inline struct mulle_pointerpair   *
   _mulle_map_get_pair( struct mulle_map *map,
                        const void *key,
                        struct mulle_pointerpair *space)

MULLE_C_NONNULL_THIRD
static inline struct mulle_pointerpair   *
   mulle_map_get_pair( struct mulle_map *map,
                       const void *key,
                       struct mulle_pointerpair *space)
```

`mulle_map_get_pair` fills a caller-provided `space` (a `struct mulle_pointerpair`) and returns a pointer to it on success, `NULL` on failure.

- **Insertion / update / register:**

Semantics differ per owner-style verb:
  - `mulle_map_set( map, key, value)` — void; inserts or overwrites the value.
  - `mulle_map_insert( map, key, value)` — returns `1` if the key was newly inserted, `0` if it already existed.
  - `mulle_map_register( map, key, value)` — returns the surviving value (the new value on insert, the existing value on duplicate).
  - `mulle_map_update( map, key, value)` — overwrites and returns the old value.

```c
static inline void
   _mulle_map_set( struct mulle_map *map, void *key, void *value)

static inline
void   mulle_map_set( struct mulle_map *map, void *key, void *value)

static inline int
   mulle_map_insert( struct mulle_map *map, void *key, void *value)

static inline void   *
   _mulle_map_register( struct mulle_map *map, void *key, void *value)

static inline void   *
   mulle_map_register( struct mulle_map *map, void *key, void *value)

static inline void   *
   _mulle_map_update( struct mulle_map *map, void *key, void *value)

static inline void   *
   mulle_map_update( struct mulle_map *map, void *key, void *value)
```

Varargs bulk insert (terminate with `NULL`):

```c
static inline void
   mulle_map_insert_key_values( struct mulle_map *map,
                                void *firstkey, ...)
```

- **Removal / reset / shrink:**

```c
MULLE_C_NONNULL_FIRST
static inline int   _mulle_map_remove( struct mulle_map *map, const void *key)

static inline int   mulle_map_remove( struct mulle_map *map, const void *key)
```

```c
static inline void   mulle_map_remove_all( struct mulle_map *map)
```

```c
MULLE_C_NONNULL_FIRST
static inline void   _mulle_map_reset( struct mulle_map *map)

static inline void    mulle_map_reset( struct mulle_map *map)
```

```c
MULLE_C_NONNULL_FIRST
static inline void   _mulle_map_shrink_if_needed( struct mulle_map *map)

static inline void   mulle_map_shrink_if_needed( struct mulle_map *map)
```

Call `mulle_map_shrink_if_needed` after heavy removal to compact storage and speed up later enumeration.

- **Inspection:**

```c
static inline int   mulle_map_is_empty( struct mulle_map *map)

static inline size_t   mulle_map_get_count( struct mulle_map *map)

static inline size_t   mulle_map_get_size( struct mulle_map *map)

static inline void   *mulle_map_get_notakey( struct mulle_map *map)

static inline struct mulle_container_keyvaluecallback *
   mulle_map_get_callback( struct mulle_map *map)

static inline struct mulle_allocator *
   mulle_map_get_allocator( struct mulle_map *map)
```

- **Copy / merge:**

```c
static inline struct mulle_map   *mulle_map_copy( struct mulle_map *map)
```

```c
MULLE__CONTAINER_GLOBAL
void   mulle_map_add_map( struct mulle_map *map, struct mulle_map *other);
```

- **Enumeration:**

```c
struct mulle_mapenumerator
{
   MULLE_MAPENUMERATOR_BASE;
};
```

```c
static inline struct mulle_mapenumerator
   mulle_map_enumerate( struct mulle_map *map)

static inline int
   mulle_mapenumerator_next( struct mulle_mapenumerator *rover,
                             void **key,
                             void **value)

static inline struct mulle_pointerpair *
   mulle_mapenumerator_next_pair( struct mulle_mapenumerator *rover)

static inline void   mulle_mapenumerator_done( struct mulle_mapenumerator *rover)
```

Macro: `mulle_map_for( map, key, value)`.

- **Tiny enumerator (private optimization interface):** `mulle_maptinyenumerator`, `mulle_map_tinyenumerate_nil( map)`, `mulle_maptinyenumerator_next( rover, &key, &value)`, `mulle_maptinyenumerator_done( rover)` — used by ObjC dictionary integration; does its own mutation checks.

The double-underscore `mulle--map.h` provides the callback/allocator-passing equivalents: `mulle__map_init`, `mulle__map_done`, `mulle__map_get( map, key, callback)`, `mulle__map__get( ... )` (no quick pointer-equality check), `_mulle__map__get_knownhash( ... )`, `_mulle__map_set`, `_mulle__map_insert`, `_mulle__map_register`, `_mulle__map_update`, `_mulle__map_remove`, `_mulle__map_reset`, `mulle__map_enumerate( map, callback)`, `_mulle__map_count_collisions(...)`. A "known hash" variant skips the pointer-equality fast path — faster when keys are rarely pointer-identical.

#### `mulle__pointermap` (pointer-equality map, `mulle--pointermap.h`)

- **Purpose:** A stripped down `mulle__map` without callbacks: maps unique pointers to pointers (and optionally `extra` storage). Keys use pointer identity; no hashing/equality functions needed. A map of `{0,a},{1,b},{2,c}` is stored as keys array `[0,1,2]` and values array `[a,b,c]` (values at offset `+_size`).

```c
struct mulle__pointermap
{
   MULLE__POINTERMAP_BASE;
};
```

- **Lifecycle:** `mulle__pointermap_create( size_t capacity, size_t extra, struct mulle_allocator *allocator)`, `mulle__pointermap_destroy( map, allocator)`, `_mulle__pointermap_init( map, capacity, allocator)`, `_mulle__pointermap_done( map, allocator)`, `_mulle__pointermap_reset( map, allocator)`.
- **Operations:** `mulle__pointermap_set( map, key, value, allocator)`, `mulle__pointermap_register( map, key, value, allocator)`, `mulle__pointermap_insert( map, key, value, allocator)`, `mulle__pointermap_update( map, key, value, allocator)`, `mulle__pointermap_remove( map, key, allocator)`, `_mulle__pointermap_set_pair`/`_..._pair` variants, `_mulle__pointermap_shrink_if_needed`.
- **Lookup:** `mulle__pointermap_get( map, key)` returns the value or `NULL` (the not-found *value*); `mulle__pointermap_find_by_value( map, value)` scans for a value and returns a `struct mulle_pointerpair` (check `mulle_pointerpair_is_invalid`).
- **Enumeration:** `mulle__pointermap_enumerate( map)`, `_mulle__pointermapenumerator_next( rover, &key, &value)`, `_mulle__pointermapenumerator_next_pair( rover, &pair)`, `mulle__pointermapenumerator_done( rover)`; macros `mulle__pointermap_do( name)`, `mulle__pointermap_for( map, key, value)`.
- The global `mulle__pointermap_keyvaluecallback` is provided so a `mulle__pointermap` can be presented where a keyvalue callback is expected.

### 3.9. `mulle-set.h` — `mulle_set` (hashtable membership)

- **Purpose:** A hash set for membership tests. Stores only keys (no values). Uses a key callback for hash/equality/ownership. The storage is a `mulle__pointerset` plus callback and allocator.

```c
struct mulle_set
{
   MULLE_SET_BASE;
};
```

```c
#define MULLE_SET_BASE                             \
   MULLE__SET_BASE;                                \
   struct mulle_container_keycallback   *callback; \
   struct mulle_allocator               *allocator
```

- **Lifecycle:**

```c
MULLE__CONTAINER_GLOBAL
struct mulle_set   *mulle_set_create( size_t capacity,
                                      struct mulle_container_keycallback *callback,
                                      struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
void   mulle_set_init( struct mulle_set *set,
                       size_t capacity,
                       struct mulle_container_keycallback *callback,
                       struct mulle_allocator *allocator);
```

```c
static inline void   mulle_set_done( struct mulle_set *set)
```

```c
static inline void   mulle_set_destroy( struct mulle_set *set)
```

- **Core operations:**

```c
static inline void   mulle_set_set( struct mulle_set *set, void *p)
```

```c
static inline int   mulle_set_insert( struct mulle_set *set, void *p)
```

```c
static inline void   *mulle_set_register( struct mulle_set *set, void *p)
```

```c
static inline void   mulle_set_remove( struct mulle_set *set, const void *p)
```

```c
static inline void   mulle_set_reset( struct mulle_set *set)
```

```c
static inline void   _mulle_set_shrink_if_needed( struct mulle_set *set)
```

`set` overwrites unconditionally, `insert` returns `1` if actually inserted (key was new), `register` returns the stored element (the parameter on insert, the pre-existing equal element on duplicate).

- **Lookup / membership:**

```c
static inline void   *mulle_set_get( struct mulle_set *set, const void *p)
```

`mulle_set_get` returns the stored (retained) key — i.e. the canonical equal element — or `NULL`/`callback->notakey` semantics per callback. `mulle_set_member` returns `1`/`0`:

```c
static inline int
   mulle_set_member( struct mulle_set *set, const void *p)
```

- **Set algebra:**

```c
static inline void
   mulle_set_union( struct mulle_set *dst,
                    struct mulle_set *a,
                    struct mulle_set *b)
```

```c
static inline void
   mulle_set_intersect( struct mulle_set *dst,
                        struct mulle_set *a,
                        struct mulle_set *b)
```

```c
MULLE__CONTAINER_GLOBAL
void   mulle_set_add_set( struct mulle_set *set, struct mulle_set *other);
```

```c
static inline struct mulle_set   *mulle_set_copy( struct mulle_set *set)
```

- **Inspection:**

```c
static inline size_t   mulle_set_get_count( struct mulle_set *set)
```

```c
static inline size_t   mulle_set_get_size( struct mulle_set *set)
```

```c
static inline struct mulle_allocator   *mulle_set_get_allocator( struct mulle_set *set)
```

```c
static inline struct mulle_container_keycallback   *
   mulle_set_get_keycallback( struct mulle_set *set)
```

- **Enumeration:** `mulle_setenumerator`, `mulle_set_enumerate( set)`, `mulle_setenumerator_next( rover, &item)`, `mulle_setenumerator_done( rover)`; macro `mulle_set_for( set, item)`.

- **Debug:** `mulle_set_describe( struct mulle_set *set)`.

#### `mulle_pointerset` (pointer-identity membership, `mulle-pointerset.h`)

Same shape, no callbacks: membership by pointer identity. `mulle_pointerset_init( set, capacity, allocator)`, `mulle_pointerset_create( allocator)`, `mulle_pointerset_done`/`_destroy`, `mulle_pointerset_get( set, p)`, `mulle_pointerset_set( set, p)`, `mulle_pointerset_insert( set, p)`, `mulle_pointerset_register( set, p)`, `mulle_pointerset_remove( set, p)`, `mulle_pointerset_shrink( set)` / `_shrink_if_needed`, `mulle_pointerset_copy_items( dst, src)`, `mulle_pointerset_copy( set)`, `mulle_pointerset_get_count( set)`, `mulle_pointerset_get_size( set)`, `mulle_pointerset_is_full( set)`, `mulle_pointerset_is_sparse( set)`. The double-underscore layer (`mulle--pointerset.h`) uses `mulle_not_a_pointer` as the hard-coded notakey and the nonowned-pointer callback internally.

### 3.10. `mulle-pointerqueue.h` — `mulle_pointerqueue` (FIFO queue)

- **Purpose:** A first-in-first-out queue of pointers built from a linked list of fixed-size "buckets". No realloc of existing elements ever happens — pushed pointers stay stable. Bucket size and spare allowance are configuration knobs.

```c
struct mulle_pointerqueue
{
   MULLE__POINTERQUEUE_BASE;
   struct mulle_allocator   *allocator;   // public...
};
```

`MULLE__POINTERQUEUE_BASE` expands to bucket list pointers (`_spares`, `_read`, `_write`), `_count`, `_read_index`, `_write_index`, `_bucket_size` (unsigned short) and `_spare_allowance` (unsigned short). The bucket type:

```c
struct mulle__pointerqueuebucket
{
   struct mulle__pointerqueuebucket  *_next;
   void                              *_storage[ 1];
};
```

- **Lifecycle:**

```c
static inline void  mulle_pointerqueue_init( struct mulle_pointerqueue *queue,
                                             unsigned short bucket_size,
                                             unsigned short spare_allowance,
                                              struct mulle_allocator *allocator)
```

```c
static inline void  mulle_pointerqueue_init_default( struct mulle_pointerqueue *queue,
                                                     struct mulle_allocator *allocator)
```

```c
MULLE__CONTAINER_GLOBAL
struct mulle_pointerqueue *
   mulle_pointerqueue_create( unsigned short bucket_size,
                              unsigned short spare_allowance,
                              struct mulle_allocator *allocator);
```

```c
static inline
void   mulle_pointerqueue_destroy( struct mulle_pointerqueue *queue)
```

```c
static inline
void   mulle_pointerqueue_done( struct mulle_pointerqueue *queue)
```

`mulle_pointerqueue_create( 0, 0, NULL)` picks sensible defaults (0 means "auto"). Passing `allocator == NULL` uses the global default allocator.

- **Enqueue / dequeue:**

```c
static inline
void   mulle_pointerqueue_push( struct mulle_pointerqueue *queue, void *p)
```

```c
// aliases for orthogonality with mulle_pointerarray
static inline
void   mulle_pointerqueue_add( struct mulle_pointerqueue *queue, void *p)
```

```c
static inline
void   *mulle_pointerqueue_pop( struct mulle_pointerqueue *queue)
```

`pop` returns the oldest element and removes it; it returns `NULL` when the queue is empty (there is no dedicated peek API).

- **Reset / removal:**

```c
static inline
void   mulle_pointerqueue_reset( struct mulle_pointerqueue *queue)
```

```c
static inline
void   mulle_pointerqueue_removeall( struct mulle_pointerqueue *queue)
```

```c
static inline
void   mulle_pointerqueue_grow( struct mulle_pointerqueue *queue)
```

```c
static inline
void   mulle_pointerqueue_shrink( struct mulle_pointerqueue *queue)
```

- **Enumeration:**

```c
struct mulle_pointerqueueenumerator
{
   MULLE_POINTERQUEUEENUMERATOR_BASE;
};
```

```c
static inline struct mulle_pointerqueueenumerator
   mulle_pointerqueue_enumerate( struct mulle_pointerqueue *queue)
```

```c
static inline int
   _mulle_pointerqueueenumerator_next( struct mulle_pointerqueueenumerator *rover,
                                       void **item)
```

```c
static inline void   mulle_pointerqueueenumerator_done( struct mulle_pointerqueueenumerator *rover)
```

Macros: `mulle_pointerqueue_do( name)`, `mulle_pointerqueue_for( name, item)`.

### 3.11. `mulle-structqueue.h` — `mulle_structqueue` (FIFO for structs)

- **Purpose:** A FIFO queue of raw struct values (like `mulle_structarray` but queue-ordered). `push` copies a struct into a reserve slot; `pop` copies the oldest struct into a caller buffer. Pointers returned by `reserve` are **stable** — buckets are never reallocated, so a reserved pointer stays valid until `done`/`destroy`/`reset`.

```c
struct mulle_structqueue
{
   MULLE__STRUCTQUEUE_BASE;
   struct mulle_allocator   *allocator;   // public...
};
```

`MULLE__STRUCTQUEUE_BASE` adds `_sizeof_struct` / `_copy_sizeof_struct` to the pointerqueue base fields.

- **Lifecycle:**

```c
static inline void  mulle_structqueue_init( struct mulle_structqueue *queue,
                                            size_t sizeof_struct,
                                            size_t alignof_struct,
                                            unsigned short bucket_size,
                                            unsigned short spare_allowance,
                                            struct mulle_allocator *allocator)
```

```c
static inline void
   mulle_structqueue_init_default( struct mulle_structqueue *queue,
                                   size_t sizeof_struct,
                                   size_t alignof_struct,
                                   struct mulle_allocator *allocator)
```

```c
MULLE__CONTAINER_GLOBAL
struct mulle_structqueue *
   mulle_structqueue_create( size_t sizeof_struct,
                             size_t alignof_struct,
                             unsigned short bucket_size,
                             unsigned short spare_allowance,
                             struct mulle_allocator *allocator);
```

```c
static inline struct mulle_structqueue *
   mulle_structqueue_create_default( size_t sizeof_struct,
                                     size_t alignof_struct,
                                     struct mulle_allocator *allocator)
```

```c
static inline
void   mulle_structqueue_destroy( struct mulle_structqueue *queue)
```

- **Operations:**

```c
static inline
void   mulle_structqueue_push( struct mulle_structqueue *queue, void *p)
```

```c
static inline void
   *mulle_structqueue_reserve( struct mulle_structqueue *queue)
```

```c
// returns 1 on success, copies struct into 'p' (which must be large
// enough) if p is != NULL
static inline
int   mulle_structqueue_pop( struct mulle_structqueue *queue, void *p)
```

```c
// aliases for orthogonality with mulle_structarray
static inline
void   mulle_structqueue_add( struct mulle_structqueue *queue, void *p)
```

```c
// returns 1 on success, copies struct into 'p' ...
static inline
int  mulle_structqueue_pop( struct mulle_structqueue *queue, void *p)
```

- **Inspection:** `mulle_structqueue_get_count( queue)`, `mulle_structqueue_get_bucketsize( queue)`, `mulle_structqueue_get_element_size( queue)`, `mulle_structqueue_get_allocator( queue)`.
- **Enumeration:** `mulle_structqueueenumerator` with `mulle_structqueue_enumerate`, `mulle_structqueueenumerator_next( rover, &item)`, `mulle_structqueueenumerator_done`; macros `mulle_structqueue_do( name, type)`, `mulle_structqueue_for( name, item)`.

### 3.12. `mulle--rangeset.h` — `mulle__rangeset` (non-overlapping ranges)

- **Purpose:** Stores a set of non-overlapping, non-adjacent, non-empty, sorted ranges (`struct mulle_range { uintptr_t location; uintptr_t length; }` comes from `mulle-data`). Inserting a range coalesces with overlapping/adjacent ranges; removing a range splits. There are no callbacks — allocator only.

```c
struct mulle__rangeset
{
   struct mulle_range   *_ranges;
   struct mulle_range   *_initial_storage; // for use with static storage
   size_t                _length;
   size_t                _size;
};
```

The functions are all `_mulle__rangeset_*` (this container only exists in the double-underscore layer; all `_mulle_`/`mulle_` wrappers are absent).

- **Lifecycle / capacity:**

```c
MULLE_C_NONNULL_FIRST
static inline void   _mulle__rangeset_init( struct mulle__rangeset *p,
                                            size_t capacity,
                                            struct mulle_allocator *allocator)
```

```c
MULLE_C_NONNULL_FIRST_SECOND
static inline void   _mulle__rangeset_init_with_static_ranges( struct mulle__rangeset *p,
                                                               struct mulle_range *storage,
                                                               size_t capacity)
```

```c
MULLE_C_NONNULL_FIRST
static inline void   _mulle__rangeset_done( struct mulle__rangeset *p,
                                            struct mulle_allocator *allocator)
```

```c
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__rangeset_reset( struct mulle__rangeset *p,
                               struct mulle_allocator *allocator);
```

```c
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__rangeset_grow( struct mulle__rangeset *p,
                              struct mulle_allocator *allocator);
```

```c
MULLE_C_NONNULL_FIRST
static inline void   _mulle__rangeset_shrink_to_fit( struct mulle__rangeset *p,
                                                     struct mulle_allocator *allocator)
```

- **Core operations (add / remove, coalescing):**

```c
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__rangeset_insert( struct mulle__rangeset *p,
                                struct mulle_range range,
                                struct mulle_allocator *allocator);
```

```c
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__rangeset_remove( struct mulle__rangeset *p,
                                struct mulle_range range,
                                struct mulle_allocator *allocator);
```

```c
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__rangeset_insert_ranges( struct mulle__rangeset *p,
                                       struct mulle_range *ranges,
                                       uintptr_t n,
                                       struct mulle_allocator *allocator);
```

```c
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__rangeset_remove_ranges( struct mulle__rangeset *p,
                                       struct mulle_range *ranges,
                                       uintptr_t n,
                                       struct mulle_allocator *allocator);
```

```c
MULLE_C_NONNULL_FIRST
static inline void   _mulle__rangeset_insert_rangeset( struct mulle__rangeset *p,
                                                       struct mulle__rangeset *other,
                                                       struct mulle_allocator *allocator)
```

```c
MULLE_C_NONNULL_FIRST
static inline void   _mulle__rangeset_remove_rangeset( struct mulle__rangeset *p,
                                                       struct mulle__rangeset *other,
                                                       struct mulle_allocator *allocator)
```

```c
// shift will destroy indexes that are then outside of the 0-mulle_max_range
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void  _mulle__rangeset_shift( struct mulle__rangeset *p,
                              uintptr_t location,
                              intptr_t delta,
                              struct mulle_allocator *allocator);
```

- **Inspection / primitive access:**

```c
MULLE_C_NONNULL_FIRST
static inline uintptr_t   _mulle__rangeset_get_first( struct mulle__rangeset *p)
```

```c
MULLE_C_NONNULL_FIRST
static inline struct mulle_range   _mulle__rangeset_get( struct mulle__rangeset *p,
                                                         size_t i)
```

```c
MULLE_C_NONNULL_FIRST
static inline uintptr_t   _mulle__rangeset_get_last( struct mulle__rangeset *p)
```

```c
MULLE_C_NONNULL_FIRST
static inline uintptr_t   _mulle__rangeset_get_rangecount( struct mulle__rangeset *p)
```

```c
MULLE_C_NONNULL_FIRST
static inline struct mulle_range  _mulle__rangeset_get_range( struct mulle__rangeset *p,
                                                              uintptr_t i)
```

```c
MULLE_C_NONNULL_FIRST
static inline uintptr_t   _mulle__rangeset_sum_lengths( struct mulle__rangeset *p)
```

- **Search:**

```c
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
int   _mulle__rangeset_contains( struct mulle__rangeset *p,
                                 struct mulle_range range);
```

```c
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
int   _mulle__rangeset_intersects( struct mulle__rangeset *p,
                                   struct mulle_range range);
```

Relational search relative to a location (choices: `mulle_rangeset_equal`, `less_than`, `less_than_or_equal`, `greater_than`, `greater_than_or_equal`):

```c
enum mulle_rangeset_searchoperation
{
   mulle_rangeset_equal                 = 0x1, // equal bit
   mulle_rangeset_less_than             = 0x2, // < bit
   mulle_rangeset_less_than_or_equal    = 0x3, // eq bit + < bit
   mulle_rangeset_greater_than          = 0x4, // > bit
   mulle_rangeset_greater_than_or_equal = 0x5  // eq bit + > bit
};
```

```c
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
uintptr_t   _mulle__rangeset_search_location( struct mulle__rangeset *p,
                                              uintptr_t location,
                                              enum mulle_rangeset_searchoperation searchOp);
```

```c
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
struct mulle_range
   _mulle__rangeset_search_range( struct mulle__rangeset *p,
                                  uintptr_t location,
                                  enum mulle_rangeset_searchoperation searchOp);
```

```c
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
struct mulle_range
   _mulle__rangeset_search_nearest( struct mulle__rangeset *p,
                                    uintptr_t location);
```

```c
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
struct mulle_range
   _mulle__rangeset_search_exact( struct mulle__rangeset *p,
                                  uintptr_t location);
```

- **Print:** `_mulle__rangeset_fprint( struct mulle__rangeset *set, FILE *fp)`, `_mulle__rangeset_print( struct mulle__rangeset *set)`; macros `mulle__rangeset_do( name)`, `mulle__rangeset_do_flexible( name, stackcount)`. Enumeration is by index over `_ranges` via `get_range`/`get_rangecount`.

### 3.13. `mulle--rangemap.h` — `mulle__rangemap` (range-to-value map)

- **Purpose:** Maps ranges to `void *` values. Ranges must be non-overlapping; insertion fails (returns nonzero) if the range overlaps an existing one. Lookup is by location via binary search.

```c
struct mulle__rangemap
{
   void        *_storage;    // the malloc'd area
   size_t      _length;
   size_t      _size;        // size of each half
#ifdef MULLE__CONTAINER_HAVE_MUTATION_COUNT
   uintptr_t   _n_mutations;
#endif
};
```

- **Lifecycle:** `_mulle__rangemap_init( map, capacity, allocator)`, `_mulle__rangemap_done( map, allocator)`, `_mulle__rangemap_reset( map, allocator)`.

- **Insert / remove:**

```c
// returns 0 on success
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
int   _mulle__rangemap_insert( struct mulle__rangemap *map,
                               struct mulle_range range,
                               void *value,
                               struct mulle_allocator *allocator);
```

```c
// the range must match exactly
// returns 0 on success
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
int    _mulle__rangemap_remove( struct mulle__rangemap *map,
                                struct mulle_range range,
                                struct mulle_allocator *allocator);
```

- **Lookup / query:**

```c
// returns index to be used with _mulle__rangemap_get_range
// or _mulle__rangemap_get_value
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
uintptr_t  _mulle__rangemap_search( struct mulle__rangemap *map,
                                    uintptr_t location);
```

```c
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   *_mulle__rangemap_get_exact( struct mulle__rangemap *map,
                                    struct mulle_range range);
```

`_mulle__rangemap_get_exact` returns the value for the exact matching range, or `NULL`; it sets `errno` to `ENOENT` if absent and `EINVAL` if an intersecting-but-unequal range is found.

```c
// returns the number of ranges that intersect with the given range.
// out_ranges and out_values may be NULL, if the respective data is
// not wanted. At most max_out entries are written to each array,
// the return value is the number of intersecting ranges (which may
// be larger than max_out).
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
uintptr_t   _mulle__rangemap_get_ranges_values( struct mulle__rangemap *map,
                                                struct mulle_range range,
                                                uintptr_t max_out,
                                                struct mulle_range *out_ranges,
                                                void **out_values);
```

- **Inspection:** `_mulle__rangemap_get_count( map)`, `_mulle__rangemap_is_empty( map)`, `_mulle__rangemap_get_range( map, index)`, `_mulle__rangemap_get_value( map, index)`, `_mulle__rangemap_get_ranges( map)`, `_mulle__rangemap_get_values( map)`.
- **Enumeration:**

```c
struct mulle__rangemapenumerator
{
   struct mulle_range   *_curr;
   struct mulle_range   *_sentinel;
   void                 **_values;
};
```

```c
static inline struct mulle__rangemapenumerator
   _mulle__rangemap_enumerate( struct mulle__rangemap *map)

static inline int
   _mulle__rangemapenumerator_next( struct mulle__rangemapenumerator *rover,
                                    struct mulle_range *range,
                                    void **value)
```

Macro: `mulle__rangemap_do( name)`. Print: `_mulle__rangemap_print( map, FILE *fp)`.

## 4. Performance Characteristics

| Structure | Insert | Lookup/Find | Remove | Notes |
|-----------|--------|-------------|--------|-------|
| `mulle_array` / `mulle_pointerarray` | amortized O(1) append, O(n) in-place insert/remove (memmove) | O(n) linear scan; `find` returns index | O(n) | Very low overhead; ideal default for small n |
| `mulle_structarray` | amortized O(1) append (memcpy) | O(n) scan | O(n) | Stores raw bytes; no hash overhead |
| `mulle_assoc` | O(log n) amortized (lazy sort + binary search) | O(log n) binary search | O(log n) | Requires a compare function; keeps pairs sorted |
| `mulle_pointerpairarray` | O(1) append | O(n) scan (compare/callback) | O(n) | For small (<~100) key/value sets |
| `mulle_map` / `mulle__pointermap` | O(1) average (hash; auto-grow at 75% fill) | O(1) average | O(1) average | Internal `_size` is a power of two (min 4); hashing masks with `_size - 2` so odd slots act as overflow |
| `mulle_set` / `mulle__pointerset` | O(1) average | O(1) average membership | O(1) average | Same power-of-two hash storage as the map |
| `mulle_pointerqueue` / `mulle_structqueue` | O(1) (bucket append) | — | O(1) (bucket pop) | Buckets avoid realloc; stable element pointers |
| `mulle__rangeset` | O(n) worst (overlapping ranges) amortized O(log n) locality | O(log n) binary search (search ops) | O(n) worst | Coalesces on insert; keeps ranges compact |
| `mulle__rangemap` | O(n) on overlap (shift) | O(log n) binary search | O(n) | Values stored in a parallel half-array |

General notes:

- **Auto-grow / shrink:** all resizeable containers grow geometrically (doubling / `mulle_pow2roundup`) through the shared `mulle_allocator`. Hash tables trigger growth at 75% fill and can shrink when the count drops below ~half the (masked) capacity; call `*_shrink_if_needed` explicitly after heavy removal to keep enumeration fast and memory low.
- **Memory trade-off:** the managed containers (with callbacks) may deep-copy on insert; unmanaged ones store raw pointers. Use `mulle_structarray` when elements are structs/floats — far more compact than boxing them in `mulle_array`.
- **Mutation counting:** in non-NDEBUG builds, enumerators snapshot a mutation counter and assert if the container is modified during enumeration (`MULLE__CONTAINER_HAVE_MUTATION_COUNT`). Defining `MULLE__CONTAINER_MISER_MODE` drops this bookkeeping for minimal size.
- **Thread-safety:** none of the containers are thread-safe; concurrent reads/writes require external synchronization.

## 5. AI Usage Recommendations & Patterns

### Best practices

- Always pair lifecycle calls: `init` ↔ `done`, `create` ↔ `destroy`. For short-lived scoped use, prefer the `_do` macros (`mulle_map_do( map, &callback) { ... }`) which guarantee `done` even on `break`/`return` via `MULLE_C_CONFINED_LOOP`. Similarly prefer `_for` macros over manual enumerators for iteration.
- Choose managed vs unmanaged deliberately: `mulle_array`/`mulle_map`/`mulle_set`/`mulle_assoc` take callbacks and will retain/release elements; `mulle_pointerarray`, `mulle__pointermap`, `mulle__pointerset`, `mulle_pointerqueue`, `mulle_structarray`, `mulle_structqueue` do not copy or free anything.
- Use the pre-baked callbacks: `mulle_container_keycallback_copied_cstring`, `_nonowned_pointer`, `_owned_pointer`, `mulle_container_keycallback_int`, etc., and combine them into a `mulle_container_keyvaluecallback` for maps/assocs.
- Store `int` keys/values via `mulle_int_as_pointer` / `mulle_pointer_as_int` (requires the `*_int` callback whose notakey is `mulle_not_an_int`, outside the valid int range trick).
- Pass `NULL` as allocator to use the default global allocator; the `*_default` macros (`mulle_array_create_default( callback)`, `mulle_structarray_create_default( type)`, ...) do exactly this.
- After heavy `remove` on hash tables, call `mulle_map_shrink_if_needed`, `mulle_set_shrink_if_needed`, or `mulle_pointerset_shrink` before enumerating.
- For queuing with stable pointer identity (e.g. storing references to elements), use `mulle_structqueue` with `reserve` — the returned pointer never moves.

### Common pitfalls

- Do not store `callback->notakey` in hash containers or arrays with equality: inserting it asserts in debug and corrupts the table in release. The default notakey (`mulle_not_a_pointer` = `INTPTR_MIN`) is an invalid address on supported platforms.
- `mulle_map_get` returns `NULL` for both "missing key" and "stored NULL value" — use `mulle_map_contains` when `NULL` is a legitimate value. (In `mulle_set`, `mulle_set_get` returns the retained key, not a bool.)
- `mulle_array_find`/`find_in_range` return `mulle_not_found_e` (`INTPTR_MAX`), *not* `-1`/`NULL`; compare against `mulle_not_found_e`. The identical-variant compares raw pointers; the plain variant uses the array's key callback.
- `mulle_pointerqueue_pop` returns `NULL` on empty — verify a `NULL` element cannot be legitimately queued (or track count) before relying on it. `mulle_structqueue_pop` returns an `int` status into a caller buffer, so `NULL` elements are fine there.
- `mulle__pointermap`/`mulle__pointerset` use pointer identity — two distinct string objects with equal contents are different members. Use `mulle_map`/`mulle_set` with a `_cstring`-style callback for value equality.
- Do not depend on iteration order: `mulle_map`, `mulle_set`, `mulle__pointermap` and `mulle__pointerset` are hash tables (order unspecified, and it changes across shrink/grow). `mulle_array` and `mulle_assoc` preserve order (assoc sorted). Do not mutate a container during enumeration — debug builds assert, release builds may misbehave or corrupt.
- The `_`-prefixed functions (`_mulle_array_get_size`, `_mulle_map_done`, ...) are internal, non-NULL-safe, and assert in debug; use the public single-underscore names for production code.
- `mulle_assoc` lazily re-sorts; repeated `set`/`add` followed by a burst of `get` is the intended (amortized) pattern. Do not assume the pair array stays sorted between an `add` and a `find` without the compare being set.

### Idiomatic usage

- Box small integers with the pointer-identity helpers and the `*_int` callbacks instead of heap-allocating.
- Combine containers: e.g. a `mulle_set` for membership plus a `mulle_map` for payload, or a `mulle_array` for ordered processing with `mulle_map` for O(1) lookup.
- Use `mulle_structarray` for float/struct vectors; use `mulle__rangeset` to track "which indices are free/used" and `mulle__rangemap` to attach data to those ranges.

## 6. Integration Examples

### Example 1: Creating, populating and iterating a `mulle_array` of ints

Complete form (as used by the test suite):

```c
#include <mulle-container/mulle-container.h>
#include <stdio.h>


static void  loops( void)
{
   struct mulle_array                     *array;
   struct mulle_arrayenumerator           rover;
   void                                   *item;
   int                                    i;

   array = mulle_array_create( 0, &mulle_container_keycallback_int, NULL);

   for( i = -100; i <= 100; i++)
      mulle_array_add( array, mulle_int_as_pointer( i));

   for( i = 0; i <= 200; i++)
      printf( "%d ", mulle_pointer_as_int( mulle_array_get( array, i)));
   printf( "\n");

   rover = mulle_array_enumerate( array);
   while( mulle_arrayenumerator_next( &rover, &item))
      printf( "%d ", mulle_pointer_as_int( item));
   mulle_arrayenumerator_done( &rover);
   printf( "\n");

   // automatic iteration with the for-macro
   mulle_array_for( array, item)
      printf( "%d ", mulle_pointer_as_int( item));
   printf( "\n");

   mulle_array_destroy( array);
}


int   main( void)
{
   loops();
   return( 0);
}
```

### Example 2: A `mulle_map` of copied strings (managed ownership)

```c
#include <mulle-container/mulle-container.h>

#include <stdio.h>
#include <string.h>


static void  simple( void)
{
   struct mulle_container_keyvaluecallback   callback;
   char                                      onstack[] = { 'V', 'f', 'L', 0 };

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   mulle_map_do( map, &callback)
   {
      assert( mulle_map_get_count( map) == 0);
      assert( ! mulle_map_get( map, "VfL"));

      mulle_map_set( map, "VfL", "VFL");
      assert( ! strcmp( "VFL", mulle_map_get( map, "VfL")));

      mulle_map_set( map, "VfL", "BOCHUM");          // overwrite
      assert( ! strcmp( "BOCHUM", mulle_map_get( map, "VfL")));

      // remove (also removes key/value via callback)
      mulle_map_remove( map, onstack);
      assert( ! mulle_map_get( map, "VfL"));

      // enumeration
      mulle_map_for( map, key, value)
         printf( "%s : %s\n", key, value);
   }
}


int   main( void)
{
   simple();
   return( 0);
}
```

(Equivalent heap lifetime: `mulle_map_create( 0, &callback, NULL)` ... `mulle_map_destroy( map)`.)

### Example 3: A `mulle_pointerqueue` FIFO round-trip

```c
#include <mulle-container/mulle-container.h>

#include <stdio.h>


static void  simple_queue( void)
{
   mulle__pointerqueue_do( queue)
   {
      printf( "%s\n", _mulle__pointerqueue_pop( queue, NULL) ? "???" : "NULL");
      _mulle__pointerqueue_push( queue, "1", NULL);
      _mulle__pointerqueue_push( queue, "2", NULL);
      _mulle__pointerqueue_push( queue, "3", NULL);
      _mulle__pointerqueue_push( queue, "4", NULL);
      _mulle__pointerqueue_push( queue, "5", NULL);

      printf( "%s\n", (char *) _mulle__pointerqueue_pop( queue, NULL));
      printf( "%s\n", (char *) _mulle__pointerqueue_pop( queue, NULL));
      printf( "%s\n", (char *) _mulle__pointerqueue_pop( queue, NULL));
      printf( "%s\n", (char *) _mulle__pointerqueue_pop( queue, NULL));
      printf( "%s\n", (char *) _mulle__pointerqueue_pop( queue, NULL));
      printf( "%s\n", _mulle__pointerqueue_pop( queue, NULL) ? "???" : "NULL");
   }
}
```

Single-underscore push of the same queue: `mulle__pointerqueue_create( 0, 0, NULL)`, `mulle_pointerqueue_push( queue, p)`, `mulle_pointerqueue_pop( queue)`, `mulle_pointerqueue_destroy( queue)`.

### Example 4: Set union and membership tests

```c
#include <mulle-container/mulle-container.h>

#include <stdio.h>


int main( void)
{
   struct mulle_set                        *set1;
   struct mulle_set                        *set2;
   struct mulle_container_keycallback      callback;

   callback = mulle_container_keycallback_copied_cstring;

   set1 = mulle_set_create( 0, &callback, NULL);
   set2 = mulle_set_create( 0, &callback, NULL);

   mulle_set_set( set1, "VfL");
   mulle_set_set( set1, "Bochum");
   mulle_set_set( set1, "1848");
   printf( "Set1 count: %zu\n", mulle_set_get_count( set1));

   mulle_set_set( set2, "VfL");
   mulle_set_set( set2, "Bochum");
   mulle_set_set( set2, "Football");
   printf( "Set2 count: %zu\n", mulle_set_get_count( set2));

   mulle_set_union( set1, set1, set2);
   printf( "Union count: %zu\n", mulle_set_get_count( set1));
   printf( "Contains VfL: %d\n", mulle_set_member( set1, "VfL"));
   printf( "Contains Football: %d\n", mulle_set_member( set1, "Football"));

   mulle_set_destroy( set1);
   mulle_set_destroy( set2);
   return( 0);
}
```

### Example 5: Range-set insert / query with coalescing

```c
#include <mulle-container/mulle-container.h>

#include <stdio.h>


static void   test_boundaries( void)
{
   struct mulle__rangeset   set;
   struct mulle_range        range;

   _mulle__rangeset_init( &set, 1, NULL);

   range = mulle_range_make( mulle_range_location_max, 1);
   assert( mulle_range_is_valid( range));
   _mulle__rangeset_insert( &set, range, NULL);
   assert( _mulle__rangeset_get_first( &set) == mulle_range_location_max);
   assert( _mulle__rangeset_get_last( &set) == mulle_range_location_max);
   assert( _mulle__rangeset_contains( &set, range));

   range = mulle_range_make( mulle_range_location_max, 2);
   assert( ! mulle_range_is_valid( range));   // exceeds the addressable range
   _mulle__rangeset_insert( &set, range, NULL);
   assert( _mulle__rangeset_get_rangecount( &set) == 1);

   _mulle__rangeset_remove( &set,
                            mulle_range_make( mulle_range_location_max, 1),
                            NULL);
   assert( _mulle__rangeset_get_rangecount( &set) == 0);
   _mulle__rangeset_done( &set, NULL);
}
```

## 7. Dependencies

Direct `mulle-sde` library dependencies (from `.mulle/etc/sourcetree/config`):

- `mulle-allocator` — all memory allocation; every container takes a `struct mulle_allocator *` (or embeds one).
- `mulle-data` — provides `struct mulle_range`, `mulle_not_found_e`, and the range arithmetic (`mulle_range_make`, `mulle_range_union`, `mulle_range_subtract`, ...) that `mulle__rangeset`, `mulle__rangemap`, and the `*_in_range` operations rely on.

The callback types reference `struct mulle_allocator`; the pre-baked callbacks and containers are otherwise self-contained. `mulle-container` is a core component of the wider `mulle-core` platform.