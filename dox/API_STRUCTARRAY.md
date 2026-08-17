# `mulle_structarray`

`mulle_structarray` is a growing array of fixed-size structs. Unlike
`mulle_pointerarray` which stores `void *` pointers, `mulle_structarray`
stores struct copies inline in the array with proper alignment.

It has been coded for a fast "reserve" operation.

**BEWARE**: As the `mulle_structarray` reallocs, returned pointers are
guaranteed to be useful only until you modify the `mulle_structarray`. If you
need pointer-stable storage use `mulle_structqueue` instead.

### Pointer stability rules

| Pointer from | Valid until |
|--------------|------------|
| `get` (element pointer) | next `add`/`grow`/`guarantee` that triggers realloc |
| `guarantee` (reserved storage) | next `add`/`grow`/`guarantee` beyond remaining capacity |
| `get_current` | next `add`/`grow`/`guarantee` that triggers realloc |
| enumerator iteration | any mutation (add, remove, grow, reset) |

If you need stable element addresses, use `mulle_structqueue` instead — its
bucket-chained design never moves existing elements.


## Types

```c
struct mulle_structarray
struct mulle_structarrayenumerator
struct mulle_structarrayreverseenumerator
```

### Comparison callback

```c
typedef int   mulle_structarray_cmp_t( void *, void *, void *);
```

Used by the sorting functions. The first two arguments are pointers to
elements, the third is the `userinfo` passed to the sort call.


## Functions

> For most functions there is an '`_`' version available, that is not
> NULL-safe but asserts in debug builds.


### Creation / Destruction

#### `mulle_structarray_alloc`

```c
struct mulle_structarray  *mulle_structarray_alloc( struct mulle_allocator *allocator)
```

Allocate an uninitialized `mulle_structarray` on the heap.


#### `mulle_structarray_init`

```c
void   mulle_structarray_init( struct mulle_structarray *array,
                               size_t sizeof_struct,
                               size_t alignof_struct,
                               size_t capacity,
                               struct mulle_allocator *allocator)
```

Initialize a `mulle_structarray` for elements of `sizeof_struct` bytes with
`alignof_struct` alignment. `capacity` is the initial number of elements.
`allocator` may be NULL for the default allocator.

Typical use:

```c
struct mulle_structarray   array;

mulle_structarray_init( &array, sizeof( struct my_struct), alignof( struct my_struct), 8, NULL);
```

There is also a convenience macro:

```c
mulle_structarray_init_default( array, type)
```

which expands to `mulle_structarray_init( array, sizeof( type), alignof( type), 8, NULL)`.


#### `mulle_structarray_init_with_static_storage`

```c
void   mulle_structarray_init_with_static_storage( struct mulle_structarray *array,
                                                   size_t sizeof_struct,
                                                   size_t alignof_struct,
                                                   size_t count,
                                                   void  *storage,
                                                   struct mulle_allocator *allocator)
```

Initialize the array using pre-existing `storage` that can hold `count`
elements. If the array outgrows this storage it will `realloc` via `allocator`.


#### `mulle_structarray_create`

```c
struct mulle_structarray  *mulle_structarray_create( size_t sizeof_struct,
                                                     size_t alignof_struct,
                                                     size_t capacity,
                                                     struct mulle_allocator *allocator)
```

Allocate and init a `mulle_structarray` on the heap.

There is also a convenience macro:

```c
mulle_structarray_create_default( type)
```

which expands to `mulle_structarray_create( sizeof( type), alignof( type), 8, NULL)`.


#### `mulle_structarray_done`

```c
void   mulle_structarray_done( struct mulle_structarray *array)
```

Free internal resources. Call on stack-allocated arrays when done.


#### `mulle_structarray_destroy`

```c
void   mulle_structarray_destroy( struct mulle_structarray *array)
```

Done + free. Use for heap-allocated arrays created with `mulle_structarray_create`.


#### `mulle_structarray_reset`

```c
void   mulle_structarray_reset( struct mulle_structarray *array)
```

Remove all elements, keep allocation.


### Accessors

#### `mulle_structarray_get_count`

```c
size_t   mulle_structarray_get_count( struct mulle_structarray *array)
```

Return the number of elements stored in the array.


#### `mulle_structarray_get_size`

```c
size_t   mulle_structarray_get_size( struct mulle_structarray *array)
```

Return the current capacity (number of elements that fit without realloc).


#### `mulle_structarray_get_size_as_length`

```c
size_t   mulle_structarray_get_size_as_length( struct mulle_structarray *array)
```

Return the current capacity in bytes.


#### `mulle_structarray_get_element_size`

```c
size_t   mulle_structarray_get_element_size( struct mulle_structarray *array)
```

Return the aligned size of each element in bytes.


#### `mulle_structarray_get_struct_size`

```c
size_t   mulle_structarray_get_struct_size( struct mulle_structarray *array)
```

Alternative name for `mulle_structarray_get_element_size`.


#### `mulle_structarray_get_allocator`

```c
struct mulle_allocator  *mulle_structarray_get_allocator( struct mulle_structarray *array)
```

Return the allocator used by the array.


#### `mulle_structarray_get_current`

```c
void  *mulle_structarray_get_current( struct mulle_structarray *array)
```

Return a pointer past the last element. Use in conjunction with
`_mulle_structarray_guarantee` only.


### Element Access

#### `mulle_structarray_get`

```c
void  *mulle_structarray_get( struct mulle_structarray *array, size_t i)
```

Return a pointer to the element at index `i`. The pointer is valid only until
the next modification of the array.


#### `mulle_structarray_get_lenient`

```c
void  *mulle_structarray_get_lenient( struct mulle_structarray *array, size_t i)
```

Return a pointer to the element at index `i`, or NULL if `i` is out of bounds.


#### `mulle_structarray_get_first`

```c
void  *mulle_structarray_get_first( struct mulle_structarray *array)
```

Return a pointer to the first element, or NULL if empty.


#### `mulle_structarray_get_last`

```c
void  *mulle_structarray_get_last( struct mulle_structarray *array)
```

Return a pointer to the last element, or NULL if empty.


### Element Operations

#### `mulle_structarray_add`

```c
void   mulle_structarray_add( struct mulle_structarray *array, void *item)
```

Copy `item` (which must point to a struct of the correct size) into the array.
The array grows automatically.


#### `mulle_structarray_set`

```c
void   mulle_structarray_set( struct mulle_structarray *array, size_t i, void *item)
```

Overwrite element at index `i` with a copy of `item`.


#### `mulle_structarray_pop`

```c
int   mulle_structarray_pop( struct mulle_structarray *array, void *item)
```

Remove the last element and copy it into `item`. Returns 1 on success, 0 if
the array was empty.


#### `mulle_structarray_reserve`

```c
void  *mulle_structarray_reserve( struct mulle_structarray *array)
```

Reserve space for one element and return a pointer to it. The element is
counted as used — copy your data in.


#### `mulle_structarray_remove_last`

```c
void   mulle_structarray_remove_last( struct mulle_structarray *array)
```

Remove the last element.


#### `mulle_structarray_remove_in_range`

```c
void   mulle_structarray_remove_in_range( struct mulle_structarray *array,
                                          struct mulle_range range)
```

Remove elements in the given range, shifting subsequent elements down.


#### `mulle_structarray_remove_at_index`

```c
void   mulle_structarray_remove_at_index( struct mulle_structarray *array,
                                          uintptr_t location)
```

Remove the element at `location`, shifting subsequent elements down.


#### `mulle_structarray_insert`

```c
void   mulle_structarray_insert( struct mulle_structarray *array,
                                 uintptr_t location,
                                 void *item)
```

Insert `item` at `location`, shifting subsequent elements up.


#### `mulle_structarray_insert_in_range`

```c
void   mulle_structarray_insert_in_range( struct mulle_structarray *array,
                                          struct mulle_range range,
                                          void *items)
```

Insert multiple `items` at the position specified by `range`.


### Array Operations

#### `mulle_structarray_add_structarray`

```c
void   mulle_structarray_add_structarray( struct mulle_structarray *array,
                                          struct mulle_structarray *other,
                                          struct mulle_range range)
```

Add elements from `other` in the given range to `array`.
Use `mulle_range_make_all()` to add all elements.


#### `mulle_structarray_copy`

```c
struct mulle_structarray  *mulle_structarray_copy( struct mulle_structarray *other,
                                                   struct mulle_allocator *allocator)
```

Create a heap-allocated copy of `other` using `allocator`.


### Sorting

#### `mulle_structarray_qsort_r_inline`

```c
void   mulle_structarray_qsort_r_inline( struct mulle_structarray *array,
                                         mulle_structarray_cmp_t *compare,
                                         void *userinfo)
```

Sort the array in-place using `compare`. This variant may be inlined by the
compiler.


#### `mulle_structarray_qsort_r`

```c
void   mulle_structarray_qsort_r( struct mulle_structarray *array,
                                  mulle_structarray_cmp_t *compare,
                                  void *userinfo)
```

Sort the array in-place using `compare`.


### Enumeration

#### `mulle_structarray_enumerate`

```c
struct mulle_structarrayenumerator
   mulle_structarray_enumerate( struct mulle_structarray *array)
```

Return an enumerator to iterate over the array front-to-back.


#### `mulle_structarrayenumerator_next`

```c
int   mulle_structarrayenumerator_next( struct mulle_structarrayenumerator *rover,
                                        void **item)
```

Returns 1 and a pointer to the next element in `*item`. Returns 0 when done.


#### `mulle_structarrayenumerator_done`

```c
void   mulle_structarrayenumerator_done( struct mulle_structarrayenumerator *rover)
```

Marks the end of the enumerator lifetime. Must be called.


#### `mulle_structarray_reverseenumerate`

```c
struct mulle_structarrayreverseenumerator
   mulle_structarray_reverseenumerate( struct mulle_structarray *array)
```

Return an enumerator to iterate over the array back-to-front.


#### `mulle_structarrayreverseenumerator_next`

```c
int   mulle_structarrayreverseenumerator_next( struct mulle_structarrayreverseenumerator *rover,
                                               void **item)
```

Returns 1 and a pointer to the next element (back to front) in `*item`.
Returns 0 when done.


#### `mulle_structarrayreverseenumerator_done`

```c
void   mulle_structarrayreverseenumerator_done( struct mulle_structarrayreverseenumerator *rover)
```

Marks the end of the reverse enumerator lifetime. Must be called.


### Macros

#### `MULLE_STRUCTARRAY_DATA`

```c
MULLE_STRUCTARRAY_DATA( storage, type, count, allocator)
```

Compound literal initializer for a `mulle_structarray` backed by `storage`
holding `count` elements of `type`.


#### `mulle_structarray_do`

```c
mulle_structarray_do( name, type)
{
   // use name as struct mulle_structarray *
}
```

Stack-allocate a structarray that is automatically cleaned up when the scope
exits.


#### `mulle_structarray_do_flexible`

```c
mulle_structarray_do_flexible( name, type, stackcount)
{
   // use name as struct mulle_structarray *
}
```

Like `mulle_structarray_do`, but backed by a stack buffer of `stackcount`
elements to avoid initial heap allocation.


#### `mulle_structarray_for`

```c
mulle_structarray_for( array, item)
{
   // item is void * pointing to each element
}
```

Enumerate all elements front-to-back.


#### `mulle_structarray_for_reverse`

```c
mulle_structarray_for_reverse( array, item)
{
   // item is void * pointing to each element
}
```

Enumerate all elements back-to-front.
