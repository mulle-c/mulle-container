# mulle-container Data Models

## Core Struct Definitions

### Pointer Pair

```c
struct mulle_pointerpair
{
   void   *key;
   void   *value;
};
```

Used throughout maps and associative arrays as the fundamental key/value unit.

### Range

```c
struct mulle_range
{
   uintptr_t   location;
   uintptr_t   length;
};
```

Identical to `NSRange`. Used by `mulle__rangeset` and `mulle__rangemap`.

## Container Structs

### Array Family

```c
// Minimal pointer array (no allocator)
struct mulle__pointerarray
{
   void       **_storage;
   void       **_curr;        // next write position
   void       **_sentinel;    // end of allocated storage
   // (debug: mutation count)
};

// Convenience pointer array (with allocator)
struct mulle_pointerarray
{
   struct mulle__pointerarray   _array;
   struct mulle_allocator       *allocator;
};

// Managed array adds callback
struct mulle__array    // same layout as mulle__pointerarray
{
   // inherits mulle__pointerarray fields
};

struct mulle_array
{
   struct mulle__array                    _array;
   struct mulle_container_keycallback     *callback;
   struct mulle_allocator                 *allocator;
};

// Struct array stores element size
struct mulle__structarray
{
   void       *_storage;
   void       *_curr;
   void       *_sentinel;
   size_t     _sizeof_struct;  // element size for proper stride
};
```

### Hashtable Family

```c
// Minimal set (pointer-based)
struct mulle__pointerset
{
   void           **_storage;
   unsigned int   _count;
   unsigned int   _size;       // always power of two
   // (debug: mutation count)
};

// Set with callbacks
struct mulle__set    // same layout as mulle__pointerset
{
   // inherits mulle__pointerset fields
};

// Minimal map — keys and values in split storage
struct mulle__pointermap
{
   void           **_storage;   // first half: keys, second half: values
   unsigned int   _count;
   unsigned int   _size;
};

struct mulle__map    // same layout as mulle__pointermap
{
   // inherits mulle__pointermap fields
};

struct mulle_map
{
   struct mulle__map                          _map;
   struct mulle_container_keyvaluecallback    *callback;
   struct mulle_allocator                     *allocator;
};
```

**Hashtable storage layout:**
```
_storage[0 .. _size-1]       = keys
_storage[_size .. 2*_size-1] = values  (maps only)
```

Holes are marked with `mulle_not_a_pointer` = `(void *) INTPTR_MIN`.

### Queue Family

```c
struct mulle__pointerqueuebucket
{
   struct mulle__pointerqueuebucket   *_next;
   void                              *_pointers[1];  // flexible array
};

struct mulle__pointerqueue
{
   struct mulle__pointerqueuebucket   *_read;
   struct mulle__pointerqueuebucket   *_write;
   unsigned short                     _read_index;
   unsigned short                     _write_index;
   unsigned short                     _bucket_size;
   unsigned int                       _count;
   struct mulle__pointerqueuebucket   *_spares;
};
```

Buckets form a singly-linked list. Read and write pointers track current
positions. Exhausted buckets are recycled into `_spares`.

### Range Structures

```c
struct mulle__rangeset
{
   struct mulle_range   *_ranges;
   unsigned int         _count;
   unsigned int         _size;
};
```

Ranges are kept sorted and auto-coalesced (no overlaps, no adjacency, no
zero-length ranges).

## Callback Structs

```c
struct mulle_container_keycallback
{
   uintptr_t   (*hash)(struct mulle_container_keycallback *callback, void *p);
   int         (*is_equal)(struct mulle_container_keycallback *callback, void *a, void *b);
   void       *(*retain)(struct mulle_container_keycallback *callback, void *p, struct mulle_allocator *allocator);
   void        (*release)(struct mulle_container_keycallback *callback, void *p, struct mulle_allocator *allocator);
   void       *(*describe)(struct mulle_container_keycallback *callback, void *p, struct mulle_allocator *allocator);
   void        *notakey;    // sentinel value for hashtable holes
   void        *userinfo;
};

struct mulle_container_valuecallback
{
   void       *(*retain)(struct mulle_container_valuecallback *callback, void *p, struct mulle_allocator *allocator);
   void        (*release)(struct mulle_container_valuecallback *callback, void *p, struct mulle_allocator *allocator);
   void       *(*describe)(struct mulle_container_valuecallback *callback, void *p, struct mulle_allocator *allocator);
   void        *userinfo;
};

struct mulle_container_keyvaluecallback
{
   struct mulle_container_keycallback     keycallback;
   struct mulle_container_valuecallback   valuecallback;
};
```

## Enumerator Structs

All enumerators follow the same pattern — a small struct that tracks iteration
state:

```c
struct mulle__pointerarrayenumerator
{
   void   **_curr;
   void   **_sentinel;
};

struct mulle__mapenumerator
{
   void           **_keys;
   unsigned int   _i;
   unsigned int   _size;
};

struct mulle__pointerqueueenumerator
{
   struct mulle__pointerqueuebucket   *_bucket;
   unsigned short                     _index;
};
```

Enumerators are always stack-allocated and short-lived. The `_done()` function
is typically a no-op but must be called for forward compatibility.

## Write Mode Enum

```c
enum mulle_container_write_mode
{
   mulle_container_overwrite_e,   // set: overwrite existing
   mulle_container_insert_e,      // insert: fail if exists
   mulle_container_update_e       // update: overwrite and return old
};
```

Used internally by the generic hashtable write functions to distinguish
between `set`, `insert`, `register`, and `update` operations.
