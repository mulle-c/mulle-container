# `mulle__rangeset`

`mulle__rangeset` is an ordered collection of non-overlapping `mulle_range`
elements. It automatically coalesces overlapping and adjacent ranges and
removes zero-length ranges. It could be the basis for an `NSIndexSet`.

This is a double-underscore container — it does not store an allocator. You
must pass the allocator to each function call.

The strategy of this container is:

* no overlapping ranges
* no adjacent ranges
* no ranges of size 0
* always sorted


## Types

``` c
struct mulle__rangeset
```


## Functions


### Setup

#### `_mulle__rangeset_init`

``` c
void   _mulle__rangeset_init( struct mulle__rangeset *p,
                              size_t capacity,
                              struct mulle_allocator *allocator)
```

Initialize a `mulle__rangeset`. `capacity` is the initial number of ranges.
Pass 0 to start empty. `allocator` is used for the initial allocation if
capacity > 0.


#### `_mulle__rangeset_init_with_static_ranges`

``` c
void   _mulle__rangeset_init_with_static_ranges( struct mulle__rangeset *p,
                                                  struct mulle_range *storage,
                                                  size_t capacity)
```

Initialize with pre-allocated static storage. The rangeset will use `storage`
until it needs to grow beyond `capacity`.


#### `_mulle__rangeset_done`

``` c
void   _mulle__rangeset_done( struct mulle__rangeset *p,
                              struct mulle_allocator *allocator)
```

Free internal resources. Does not free static storage.


#### `_mulle__rangeset_reset`

``` c
void   _mulle__rangeset_reset( struct mulle__rangeset *p,
                               struct mulle_allocator *allocator)
```

Remove all ranges.


### Accessors

#### `_mulle__rangeset_get_rangecount`

``` c
uintptr_t   _mulle__rangeset_get_rangecount( struct mulle__rangeset *p)
```

Return the number of stored ranges (not the number of individual locations).


#### `_mulle__rangeset_get_range`

``` c
struct mulle_range   _mulle__rangeset_get_range( struct mulle__rangeset *p,
                                                 uintptr_t i)
```

Return the range at index `i`. Returns an invalid range if `i` is out of
bounds.


#### `_mulle__rangeset_get`

``` c
struct mulle_range   _mulle__rangeset_get( struct mulle__rangeset *p,
                                           size_t i)
```

Return the range at index `i`. Returns an invalid range if out of bounds.


#### `_mulle__rangeset_get_first`

``` c
uintptr_t   _mulle__rangeset_get_first( struct mulle__rangeset *p)
```

Return the first location in the rangeset, or `mulle_not_found_e` if empty.


#### `_mulle__rangeset_get_last`

``` c
uintptr_t   _mulle__rangeset_get_last( struct mulle__rangeset *p)
```

Return the last location in the rangeset, or `mulle_not_found_e` if empty.


#### `_mulle__rangeset_sum_lengths`

``` c
uintptr_t   _mulle__rangeset_sum_lengths( struct mulle__rangeset *p)
```

Return the total number of individual locations covered by all ranges.


### Insert and Remove

#### `_mulle__rangeset_insert`

``` c
void   _mulle__rangeset_insert( struct mulle__rangeset *p,
                                struct mulle_range range,
                                struct mulle_allocator *allocator)
```

Insert a range. Overlapping and adjacent ranges are automatically coalesced.
Zero-length ranges are ignored.


#### `_mulle__rangeset_remove`

``` c
void   _mulle__rangeset_remove( struct mulle__rangeset *p,
                                struct mulle_range range,
                                struct mulle_allocator *allocator)
```

Remove a range. Existing ranges may be split or shortened as needed.


#### `_mulle__rangeset_insert_ranges`

``` c
void   _mulle__rangeset_insert_ranges( struct mulle__rangeset *p,
                                       struct mulle_range *ranges,
                                       uintptr_t n,
                                       struct mulle_allocator *allocator)
```

Insert multiple ranges at once.


#### `_mulle__rangeset_remove_ranges`

``` c
void   _mulle__rangeset_remove_ranges( struct mulle__rangeset *p,
                                       struct mulle_range *ranges,
                                       uintptr_t n,
                                       struct mulle_allocator *allocator)
```

Remove multiple ranges at once.


#### `_mulle__rangeset_insert_rangeset`

``` c
void   _mulle__rangeset_insert_rangeset( struct mulle__rangeset *p,
                                         struct mulle__rangeset *other,
                                         struct mulle_allocator *allocator)
```

Insert all ranges from `other`.


#### `_mulle__rangeset_remove_rangeset`

``` c
void   _mulle__rangeset_remove_rangeset( struct mulle__rangeset *p,
                                         struct mulle__rangeset *other,
                                         struct mulle_allocator *allocator)
```

Remove all ranges present in `other`.


### Query

#### `_mulle__rangeset_contains`

``` c
int   _mulle__rangeset_contains( struct mulle__rangeset *p,
                                 struct mulle_range range)
```

Returns 1 if the rangeset fully contains the given range.


#### `_mulle__rangeset_intersects`

``` c
int   _mulle__rangeset_intersects( struct mulle__rangeset *p,
                                   struct mulle_range range)
```

Returns 1 if the rangeset intersects with the given range.


### Search

#### `_mulle__rangeset_search_location`

``` c
uintptr_t   _mulle__rangeset_search_location( struct mulle__rangeset *p,
                                              uintptr_t location,
                                              enum mulle_rangeset_searchoperation searchOp)
```

Find the closest location satisfying the search operation relative to
`location`. Search operations are:

| Operation | Meaning |
|-----------|---------|
| `mulle_rangeset_equal` | Location must be in a range |
| `mulle_rangeset_less_than` | Closest location < given |
| `mulle_rangeset_less_than_or_equal` | Closest location <= given |
| `mulle_rangeset_greater_than` | Closest location > given |
| `mulle_rangeset_greater_than_or_equal` | Closest location >= given |

Returns `mulle_not_found_e` if no match.


#### `_mulle__rangeset_search_range`

``` c
struct mulle_range   _mulle__rangeset_search_range( struct mulle__rangeset *p,
                                                    uintptr_t location,
                                                    enum mulle_rangeset_searchoperation searchOp)
```

Find the closest range satisfying the search operation.


### Shift

#### `_mulle__rangeset_shift`

``` c
void   _mulle__rangeset_shift( struct mulle__rangeset *p,
                               uintptr_t location,
                               intptr_t delta,
                               struct mulle_allocator *allocator)
```

Shift all locations >= `location` by `delta`. Useful for tracking insertions
and deletions in an indexed collection. Ranges that would be shifted outside
valid bounds are destroyed.


### Other

#### `_mulle__rangeset_shrink_to_fit`

``` c
void   _mulle__rangeset_shrink_to_fit( struct mulle__rangeset *p,
                                       struct mulle_allocator *allocator)
```

Minimize memory usage.


#### `_mulle__rangeset_fprint`

``` c
void   _mulle__rangeset_fprint( struct mulle__rangeset *set, FILE *fp)
```

Print the rangeset to a file stream.


#### `_mulle__rangeset_print`

``` c
void   _mulle__rangeset_print( struct mulle__rangeset *set)
```

Print the rangeset to stdout.


### Macros

#### `mulle__rangeset_do`

``` c
mulle__rangeset_do( name)
{
   // use name as struct mulle__rangeset *
}
```

Stack-allocate a rangeset with automatic cleanup.


#### `mulle__rangeset_do_flexible`

``` c
mulle__rangeset_do_flexible( name, stackcount)
{
   // uses stack storage for up to stackcount ranges
}
```

Stack-allocate a rangeset with pre-allocated static storage on the stack.
