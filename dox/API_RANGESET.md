# `mulle__rangeset`

`mulle__rangeset` is an ordered collection of non-overlapping `mulle_range`
elements. It automatically coalesces overlapping and adjacent ranges and
removes zero-length ranges. It could be the basis for an `NSIndexSet`.

This is a double-underscore container — it does not store an allocator or
callbacks. You must pass the allocator to each function call that needs one.

The strategy of this container is:

* no overlapping ranges (they are coalesced)
* no adjacent ranges (they are coalesced)
* no ranges of size 0 (they are removed)
* always sorted

The sentinel value for "not found" is `mulle_not_found_e` (`INTPTR_MAX`).


## Types

``` c
struct mulle__rangeset
{
   struct mulle_range   *_ranges;
   struct mulle_range   *_initial_storage;
   size_t                _length;
   size_t                _size;
};
```


## Macros

### `MULLE__RANGESET_DATA`

``` c
#define MULLE__RANGESET_DATA( storage, size)
```

Static initializer for a `struct mulle__rangeset` using pre-allocated
`storage` of `size` elements.


### `mulle__rangeset_do`

``` c
mulle__rangeset_do( name)
{
   // use name as struct mulle__rangeset *
}
```

Stack-allocate a rangeset scoped to the enclosing block. Automatically calls
`_mulle__rangeset_done` at scope exit with a NULL allocator.


### `mulle__rangeset_do_flexible`

``` c
mulle__rangeset_do_flexible( name, stackcount)
{
   // uses stack storage for up to stackcount ranges
}
```

Stack-allocate a rangeset with pre-allocated static storage of `stackcount`
ranges on the stack. Automatically calls `_mulle__rangeset_done` at scope exit.


## Functions


### Setup and Teardown

#### `_mulle__rangeset_init`

``` c
static inline void   _mulle__rangeset_init( struct mulle__rangeset *p,
                                            size_t capacity,
                                            struct mulle_allocator *allocator)
```

Initialize a `mulle__rangeset`. `capacity` is the initial number of ranges to
allocate space for. Pass 0 to start without allocation. `allocator` is used
for the initial malloc if `capacity` > 0.


#### `_mulle__rangeset_init_with_static_ranges`

``` c
static inline void   _mulle__rangeset_init_with_static_ranges( struct mulle__rangeset *p,
                                                               struct mulle_range *storage,
                                                               size_t capacity)
```

Initialize with pre-allocated static storage. The rangeset will use `storage`
(of `capacity` elements) until it needs to grow beyond that size, at which
point it allocates from the heap.


#### `_mulle__rangeset_done`

``` c
static inline void   _mulle__rangeset_done( struct mulle__rangeset *p,
                                            struct mulle_allocator *allocator)
```

Free internal resources. Does not free initial static storage. In DEBUG builds
the struct is overwritten with a sentinel pattern.


#### `_mulle__rangeset_reset`

``` c
void   _mulle__rangeset_reset( struct mulle__rangeset *p,
                               struct mulle_allocator *allocator)
```

Remove all ranges and shrink the internal allocation.


### Accessors

#### `_mulle__rangeset_get_rangecount`

``` c
static inline uintptr_t   _mulle__rangeset_get_rangecount( struct mulle__rangeset *p)
```

Return the number of stored ranges (not the number of individual locations).


#### `_mulle__rangeset_get`

``` c
static inline struct mulle_range   _mulle__rangeset_get( struct mulle__rangeset *p,
                                                         size_t i)
```

Return the range at index `i`. Returns an invalid range
(`mulle_range_make_invalid()`) if `i` is out of bounds.


#### `_mulle__rangeset_get_range`

``` c
static inline struct mulle_range   _mulle__rangeset_get_range( struct mulle__rangeset *p,
                                                               uintptr_t i)
```

Return the range at index `i`. Returns `{ mulle_not_found_e, 0 }` if `i` is
out of bounds.


#### `__mulle__rangeset_get_range`

``` c
static inline struct mulle_range   __mulle__rangeset_get_range( struct mulle__rangeset *p,
                                                                uintptr_t i)
```

Return the range at index `i` without bounds checking. For use when you have
already validated the index.


#### `_mulle__rangeset_get_first`

``` c
static inline uintptr_t   _mulle__rangeset_get_first( struct mulle__rangeset *p)
```

Return the first (lowest) location in the rangeset, or `mulle_not_found_e` if
empty.


#### `_mulle__rangeset_get_last`

``` c
static inline uintptr_t   _mulle__rangeset_get_last( struct mulle__rangeset *p)
```

Return the last (highest) location in the rangeset, or `mulle_not_found_e` if
empty.


#### `_mulle__rangeset_sum_lengths`

``` c
static inline uintptr_t   _mulle__rangeset_sum_lengths( struct mulle__rangeset *p)
```

Return the total number of individual locations covered by all ranges
(sum of all `range.length` values).


#### `_mulle__rangeset_sum_lengths_range`

``` c
uintptr_t   _mulle__rangeset_sum_lengths_range( struct mulle__rangeset *p,
                                                struct mulle_range range)
```

Return the total number of locations covered by the intersection of the
rangeset with `range`.


### Insert and Remove

#### `_mulle__rangeset_insert`

``` c
void   _mulle__rangeset_insert( struct mulle__rangeset *p,
                                struct mulle_range range,
                                struct mulle_allocator *allocator)
```

Insert a range. Overlapping and adjacent ranges are automatically coalesced.
Zero-length or invalid ranges are ignored.


#### `__mulle__rangeset_insert`

``` c
void   __mulle__rangeset_insert( struct mulle__rangeset *p,
                                 struct mulle_range range,
                                 struct mulle_allocator *allocator)
```

Low-level insert without the zero-length/validity check. The caller must
ensure `range.length > 0` and that the range is valid.


#### `_mulle__rangeset_remove`

``` c
void   _mulle__rangeset_remove( struct mulle__rangeset *p,
                                struct mulle_range range,
                                struct mulle_allocator *allocator)
```

Remove a range. Existing ranges may be split or shortened as needed.
Zero-length or invalid ranges are ignored.


#### `__mulle__rangeset_remove`

``` c
void   __mulle__rangeset_remove( struct mulle__rangeset *p,
                                 struct mulle_range range,
                                 struct mulle_allocator *allocator)
```

Low-level remove without the zero-length/validity check. The caller must
ensure `range.length > 0` and that the range is valid.


#### `_mulle__rangeset_insert_ranges`

``` c
void   _mulle__rangeset_insert_ranges( struct mulle__rangeset *p,
                                       struct mulle_range *ranges,
                                       uintptr_t n,
                                       struct mulle_allocator *allocator)
```

Insert `n` ranges at once from the array `ranges`.


#### `_mulle__rangeset_remove_ranges`

``` c
void   _mulle__rangeset_remove_ranges( struct mulle__rangeset *p,
                                       struct mulle_range *ranges,
                                       uintptr_t n,
                                       struct mulle_allocator *allocator)
```

Remove `n` ranges at once from the array `ranges`.


#### `_mulle__rangeset_insert_rangeset`

``` c
static inline void   _mulle__rangeset_insert_rangeset( struct mulle__rangeset *p,
                                                       struct mulle__rangeset *other,
                                                       struct mulle_allocator *allocator)
```

Insert all ranges from another rangeset `other` into `p`.


#### `_mulle__rangeset_remove_rangeset`

``` c
static inline void   _mulle__rangeset_remove_rangeset( struct mulle__rangeset *p,
                                                       struct mulle__rangeset *other,
                                                       struct mulle_allocator *allocator)
```

Remove all ranges present in `other` from `p`.


### Query

#### `_mulle__rangeset_contains`

``` c
int   _mulle__rangeset_contains( struct mulle__rangeset *p,
                                 struct mulle_range range)
```

Returns 1 if the rangeset fully contains the given range, 0 otherwise.
A zero-length range always returns 0.


#### `_mulle__rangeset_intersects`

``` c
int   _mulle__rangeset_intersects( struct mulle__rangeset *p,
                                   struct mulle_range range)
```

Returns 1 if the rangeset intersects with the given range, 0 otherwise.
A zero-length range always returns 0.


### Search

#### `enum mulle_rangeset_searchoperation`

``` c
enum mulle_rangeset_searchoperation
{
   mulle_rangeset_equal                 = 0x1,
   mulle_rangeset_less_than             = 0x2,
   mulle_rangeset_less_than_or_equal    = 0x3,
   mulle_rangeset_greater_than          = 0x4,
   mulle_rangeset_greater_than_or_equal = 0x5
};
```

Search operations used by the search functions. The values are bitmasks:
`equal` = 0x1, `less_than` = 0x2, `greater_than` = 0x4. Combined operations
OR the bits together.


#### `mulle_rangeset_searchoperation_utf8_string`

``` c
static inline char  *mulle_rangeset_searchoperation_utf8_string( enum mulle_rangeset_searchoperation mode)
```

Return a UTF-8 string representation of a search operation (`"<"`, `"<="`,
`"=="`, `">="`, `">"`).


#### `_mulle__rangeset_search_location`

``` c
uintptr_t   _mulle__rangeset_search_location( struct mulle__rangeset *p,
                                              uintptr_t location,
                                              enum mulle_rangeset_searchoperation searchOp)
```

Find the closest **location** in the rangeset satisfying the search operation
relative to `location`. Returns `mulle_not_found_e` if no match.

| Operation | Meaning |
|-----------|---------|
| `mulle_rangeset_equal` | `location` must be contained in a range |
| `mulle_rangeset_less_than` | Closest location strictly less than `location` |
| `mulle_rangeset_less_than_or_equal` | Closest location ≤ `location` |
| `mulle_rangeset_greater_than` | Closest location strictly greater than `location` |
| `mulle_rangeset_greater_than_or_equal` | Closest location ≥ `location` |


#### `_mulle__rangeset_search_range`

``` c
struct mulle_range   _mulle__rangeset_search_range( struct mulle__rangeset *p,
                                                    uintptr_t location,
                                                    enum mulle_rangeset_searchoperation searchOp)
```

Find the closest **range** in the rangeset satisfying the search operation
relative to `location`. A range `{2,3}` (covering locations 2,3,4) satisfies:

* `<= location` for location 2, 3, 4, 5, …
* `>= location` for location 0, 1, 2, 3, 4
* `< location` for location 5, 6, …
* `> location` for location 0, 1
* `== location` for location 2, 3, 4

Returns an invalid range if no match.


#### `_mulle__rangeset_search_nearest`

``` c
struct mulle_range   _mulle__rangeset_search_nearest( struct mulle__rangeset *p,
                                                      uintptr_t location)
```

Find the range nearest to `location` (by distance). If `location` is inside a
range, that range is returned. Returns an invalid range if the set is empty.


#### `_mulle__rangeset_search_exact`

``` c
struct mulle_range   _mulle__rangeset_search_exact( struct mulle__rangeset *p,
                                                    uintptr_t location)
```

Find the range containing `location` exactly. Returns an invalid range if
`location` is not contained in any range.


### Shift

#### `_mulle__rangeset_shift`

``` c
void   _mulle__rangeset_shift( struct mulle__rangeset *p,
                               uintptr_t location,
                               intptr_t delta,
                               struct mulle_allocator *allocator)
```

Shift all locations at or after `location` by `delta` positions. Useful for
tracking insertions and deletions in an indexed collection (e.g. when
characters are inserted into a text buffer, shift the index set).

* Positive `delta`: locations are shifted to higher values. Locations that
  would exceed `mulle_range_location_max` are removed first.
* Negative `delta`: locations that would become negative are removed first.

Ranges spanning `location` are split: the part before `location` stays, the
part at and after `location` is shifted.


### Memory Management

#### `_mulle__rangeset_grow`

``` c
void   _mulle__rangeset_grow( struct mulle__rangeset *p,
                              struct mulle_allocator *allocator)
```

Double the internal storage capacity (power-of-two growth). Handles the
transition from initial static storage to heap allocation.


#### `_mulle__rangeset_shrink_to_fit`

``` c
static inline void   _mulle__rangeset_shrink_to_fit( struct mulle__rangeset *p,
                                                     struct mulle_allocator *allocator)
```

Reallocate the internal storage to exactly fit the current number of ranges.
Does nothing if the rangeset uses initial static storage.


### Printing

#### `_mulle__rangeset_fprint`

``` c
void   _mulle__rangeset_fprint( struct mulle__rangeset *set, FILE *fp)
```

Print all ranges to a file stream `fp` in the format `#<index>: <location>,<length>\n`.


#### `_mulle__rangeset_print`

``` c
static inline void   _mulle__rangeset_print( struct mulle__rangeset *set)
```

Print all ranges to stdout. Equivalent to `_mulle__rangeset_fprint( set, stdout)`.
