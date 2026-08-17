# `mulle__rangemap` API

A sorted array of non-overlapping `mulle_range` values, each associated with a
`void *` value. Unlike `mulle__rangeset`, adjacent ranges are **not** merged —
each range/value pair is stored independently.

## Rules

1. You cannot insert a range that overlaps with an existing range (returns
   `EADDRINUSE`).
2. You can only remove ranges that **exactly** match a stored range (returns
   `ENOENT` if not found, `EACCES` if an intersecting but non-equal range
   exists).
3. NULL is not a valid value (asserted on insert). This is because `get_exact`
   uses NULL as the "not found" return.
4. Splitting, moving, or resizing ranges is the caller's responsibility — the
   API only provides insert and remove as primitives.
5. Ranges are kept sorted by location. Binary search is used for all lookups.

## Struct Layout

```c
struct mulle__rangemap
{
   void        *_storage;       // malloc'd area: ranges then values
   size_t      _length;         // number of stored range/value pairs
   size_t      _size;           // capacity of each half
#ifdef MULLE__CONTAINER_HAVE_MUTATION_COUNT
   uintptr_t   _n_mutations;   // debug mutation tracking
#endif
};
```

Storage is split into two halves: `_size` ranges followed by `_size` value
pointers. Growth reallocates and copies both halves.

---

## Creation and Destruction

#### `_mulle__rangemap_init`

```c
void   _mulle__rangemap_init( struct mulle__rangemap *map,
                              size_t capacity,
                              struct mulle_allocator *allocator);
```

Initialize `map` with space for `capacity` range/value pairs. Pass 0 for
lazy allocation on first insert.


#### `_mulle__rangemap_done`

```c
void   _mulle__rangemap_done( struct mulle__rangemap *map,
                              struct mulle_allocator *allocator);
```

Free storage. Does not release the stored values — that's the caller's job.


#### `_mulle__rangemap_reset`

```c
void   _mulle__rangemap_reset( struct mulle__rangemap *map,
                               struct mulle_allocator *allocator);
```

Remove all entries but keep allocated storage for reuse.

---

## Insertion and Removal

#### `_mulle__rangemap_insert`

```c
int   _mulle__rangemap_insert( struct mulle__rangemap *map,
                               struct mulle_range range,
                               void *value,
                               struct mulle_allocator *allocator);
```

Insert `range` with associated `value`. Returns 0 on success.

| Return | Meaning |
|--------|---------|
| 0 | Success |
| `EINVAL` | Range has zero length or is invalid |
| `EADDRINUSE` | An overlapping range already exists |

`value` must not be NULL (asserted in debug).


#### `_mulle__rangemap_remove`

```c
int   _mulle__rangemap_remove( struct mulle__rangemap *map,
                               struct mulle_range range,
                               struct mulle_allocator *allocator);
```

Remove a range. The range must exactly match a stored range.

| Return | Meaning |
|--------|---------|
| 0 | Success |
| `EINVAL` | Range has zero length or is invalid |
| `ENOENT` | No intersecting range found |
| `EACCES` | An intersecting range was found but it doesn't match exactly |

---

## Lookup

#### `_mulle__rangemap_search`

```c
uintptr_t   _mulle__rangemap_search( struct mulle__rangemap *map,
                                     uintptr_t location);
```

Find the index of the range containing `location`. Returns the index on
success (use with `_mulle__rangemap_get_range` / `_mulle__rangemap_get_value`),
or `mulle_not_found_e` if no range contains that location.


#### `_mulle__rangemap_get_exact`

```c
void   *_mulle__rangemap_get_exact( struct mulle__rangemap *map,
                                    struct mulle_range range);
```

Find the value for a range that exactly matches `range`. Returns the value on
success, or NULL on failure. Sets `errno` to distinguish failure causes:

| errno | Meaning |
|-------|---------|
| `ENOENT` | No intersecting range found |
| `EINVAL` | An intersecting range exists but doesn't match exactly |


#### `_mulle__rangemap_get_ranges_values`

```c
uintptr_t   _mulle__rangemap_get_ranges_values( struct mulle__rangemap *map,
                                                struct mulle_range range,
                                                uintptr_t max_out,
                                                struct mulle_range *out_ranges,
                                                void **out_values);
```

Find all ranges that intersect with `range`. Writes up to `max_out` entries
into `out_ranges` and/or `out_values` (either may be NULL if you don't need
that data). Returns the total number of intersecting ranges, which may exceed
`max_out`.

---

## Index-Based Access

#### `_mulle__rangemap_get_range`

```c
static inline struct mulle_range
   _mulle__rangemap_get_range( struct mulle__rangemap *map, uintptr_t index);
```

Return the range at `index`, or `mulle_range_make_invalid()` if out of bounds.


#### `_mulle__rangemap_get_value`

```c
static inline void   *_mulle__rangemap_get_value( struct mulle__rangemap *map,
                                                  uintptr_t index);
```

Return the value at `index`, or NULL if out of bounds.

---

## Query

#### `_mulle__rangemap_get_count`

```c
static inline size_t   _mulle__rangemap_get_count( struct mulle__rangemap *map);
```

Return the number of stored range/value pairs.


#### `_mulle__rangemap_is_empty`

```c
static inline int   _mulle__rangemap_is_empty( struct mulle__rangemap *map);
```

Return non-zero if the map has no entries.

---

## Enumeration

```c
struct mulle__rangemapenumerator   rover;
struct mulle_range                 range;
void                               *value;

rover = _mulle__rangemap_enumerate( map);
while( _mulle__rangemapenumerator_next( &rover, &range, &value))
{
   // use range and value
}
```

Ranges are enumerated in sorted order (by location). Do not mutate the map
during enumeration.

---

## Convenience Macro

#### `mulle__rangemap_do`

```c
mulle__rangemap_do( name)
{
   _mulle__rangemap_insert( name, mulle_range_make( 10, 5), value, NULL);
   // ...
}
// map is automatically done'd here
```

Stack-allocates a `mulle__rangemap` and calls `_mulle__rangemap_done` on scope
exit. Uses the default allocator (NULL).

---

## Debug

#### `_mulle__rangemap_print`

```c
void   _mulle__rangemap_print( struct mulle__rangemap *map, FILE *fp);
```

Print all range/value pairs to `fp` in human-readable format.

---

## Comparison with `mulle__rangeset`

| Feature | `mulle__rangeset` | `mulle__rangemap` |
|---------|-------------------|-------------------|
| Stores values | No (ranges only) | Yes (range → void *) |
| Merges adjacent | Yes | No |
| Merges overlapping | Yes (coalesces) | No (rejects overlap) |
| Remove semantics | Splits ranges | Exact match only |
| Use case | Index sets, selection tracking | Range → object mapping |
