# mulle-container

#### 🛄 Arrays, hashtables and a queue

A collection of C (C11) data structures to store integers, strings, pointers,
structs.

All data structures can live (temporarily) on the stack, or permanently in the
heap. None of them are thread-safe.
[mulle-allocator](//github.com/mulle-c/mulle-allocator) is used pervasively to
simplify memory management.
Some data structures utilize callbacks that are compatible to OS X's
[`NSHashtable`](//nshipster.com/nshashtable-and-nsmaptable/) and friends.


| Release Version                                       | Release Notes
|-------------------------------------------------------|--------------
| ![Mulle kybernetiK tag](https://img.shields.io/github/tag/mulle-c/mulle-container.svg?branch=release) [![Build Status](https://github.com/mulle-c/mulle-container/workflows/CI/badge.svg?branch=release)](//github.com/mulle-c/mulle-container/actions)| [RELEASENOTES](RELEASENOTES.md) |


/home/src/srcO/mulle-c/mulle-container()
## Data Structures

The names of the functions are consistent. Each function is a **verb**
that is prefixed with the name of the data structure it handles. So for example
the `get` function for `mulle_array` is `mulle_array_get`. The first parameter,
- except for creation functions - is always the container itself.

Check out [SYNTAX.md](//github.come/mulle-c/mulle-c11/dox/SYNTAX.md) for a
somewhat formal explanation and [VERBS.md](dox/VERBS.md) for a list of common
verbs being employed.

The various underscore prefixed functions may `assert` their parameters, but
when compiled for "Release" there will be no runtime checks. The non
underscore functions protect themselves against NULL pointers.
For development it is wise, to use a debug version of the library.


### Arrays

All arrays store their elements in a single block of memory.

#### Standard Arrays

##### `mulle_array` conveniently stores managed pointers

This is an array of `void *` that `realloc`s on power of two sizes. The
elements equality is determined with a callback function. The elements
can be copied/freed or reference counted using
[`mulle_container_callback`s](dox/API_CONTAINER_CALLBACK.md). It could be the
basis for a `NSMutableArray` implementation (but isn't currently).

![](pix/mulle-array.svg)

> The array was allocated with an initial capacity of 4. The addition of a
> fifth value forced an expansion to 8 (yellow: initial cells in use,
> green: additional cells in use).
> The "notakey" is not used for arrays.

There is also an [API Documentation](dox/API_ARRAY.md).


##### `mulle__array` conveniently stores managed pointers with minimal overhead

This is `mulle_array` minus the `allocator` and `callback`.

![](pix/mulle--array.svg)


##### `mulle_pointerarray` stores unmanaged pointers

This is a stripped down version of `mulle_array` to manage
untyped `void *`. The pointers are simply compared with `==`. You can also
store integers casted as `void *`. The memory referenced by a pointer isn't
managed (e.g. freed when removed from storage), like mulle-array does.

![](pix/mulle-pointerarray.svg)


##### `mulle__pointerarray` stores pointers with minimal overhead

This is a stripped down version of `mulle_pointerarray` minus the `allocator`.
It can be convenient for quickly building up lots of C arrays of `void *`.

![](pix/mulle--pointerarray.svg)



##### flexarray, a replacement for alloca

The `mulle_flexarray` can be used as an replacement for `alloca`. The problem
with `alloca` is always two-fold. 1.) It's non-standard and not available on
all platforms. 2.) The amount of memory to `alloca` may exceed the available
stack space. The `mulle_flexarray` solves this problem by using a small amount
of stack space for low memory scenarios and moving to `malloc`, when it's
needed.

Example:

``` c
 void  foo( int n, int *data)
{
   mulle_flexarray_do( copy, int, 32, n)
   {
      memcpy( copy, data, n * sizeof( int));    // using copy here for something
   }
}
```


#### Special Arrays

##### `mulle__uniquepointerarray` keeps unmanaged sorted pointers for reference

`mulle__uniquepointerarray` is based on `mulle__pointerarray`. It keeps its
elements of `void *` in an array in sorted order for searching by pointer
equality. The sorting is done lazily. It's use is as a set. It's
advantage over a hashtable set are low memory requirement and superior
performance for smaller quantities.


##### `mulle__rangeset` stores selected elements of an array

This is an array of `mulle_range` elements, that are always kept in sorted
order. A `mulle_range` is defined as

``` c
struct mulle_range
{
   uintptr_t   location;
   uintptr_t   length;
};
```

which is identical to `NSRange`. Ranges are combined and split as necessary, so
that `mulle_rangeset` can ensure that

* there are no overlapping ranges (they are coalesced)
* there are no adjacent ranges (they are coalesced)
* there are no ranges of size 0 (they are removed)

It could be the basis for an `NSIndexSet`.


#### `mulle_structarray` stores any kind of C type with proper alignment

This is a variant of `mulle_pointerarray`, but instead of using `void *` you
can specify any C type as the element size. This can be useful for building
arrays of `float` or any kind of `struct` for example.


#### Associative Arrays

##### `mulle_pointerpairarray` stores unmanaged key/value pairs

This is a modified `mulle_pointerarray` that holds key/value pairs of
`void *`, `void *` instead of just a single `void *`. In comparison to using a
map, order of addition is preserved and the space requirement is lower. But
it is slow in searches.

![](pix/mulle-pointerpairarray.svg)

##### `mulle__pointerpairarray` stores key/value pairs with minimal overhead

This is the `mulle_pointerpairarray` variant without the `allocator`.

### Queues

#### `mulle__pointerqueue` stores unmanaged pointers without copying

This is a FIFO queue for `void *`. It does not `realloc` on addition. Instead
it chains buckets of `void *` together. This makes large quantities of
additions pretty much as fast as possible.

![](pix/mulle--pointerqueue.svg)

> Filled cells indicate a cell containing a pointer value. All values are
> possible, so there are no holes.
> The cells are clustered in buckets and the buckets form a linked list. The
> `_read_index` maintains the number of values already read from the current
> `_read` bucket.
> Correspondingly the `_write_index` keeps a record of the next available index
> in  the `_write` bucket. If a bucket is exhausted a new one is allocated,
> chained into the linked list and the `_write` pointer is set to it, resetting
> the `_write_index`.

It is the basis for the `NSAutoreleasePool` implementation.


#### `mulle__structqueue` stores structs instead of pointers

A key difference to `mulle_structarray` is, that maintaining pointers to
`mulle__structqueue` elements are possible, whereas `mulle_structarray`
elements may get reorganised.


### Hashtables

A hashtable is about twice the size of the number of elements contained within,
the rest being holes. A hash of the element is used to quickly locate the entry
in the hashtable.

Hashtable `realloc` and redistribute their elements on power of two size
boundaries.

A hashtables use "holes" to discriminate between valid entries and available
space. Consequently a hashtable can not use all possible `void *` values as
keys and needs a special **notakey** marker for such holes.

The "notakey" value is usually `mulle_not_a_pointer`, which is defined as
`(void *) INTPTR_MIN`.


#### Maps

Hashtable data structures that provide associative storage (key/value) are
called maps.

##### `mulle_map` conveniently stores managed keys and values

The elements equality is determined with a callback function and the elements
can be copied/freed or reference counted using callbacks organized in a
[`mulle_container_callback`](dox/API_CONTAINER_CALLBACK.md).

![](pix/mulle-map.svg)

> The contigous memory is split into two equal sized parts "KEYS" and "VALUES".
> Gray cells indicate a "hole", they contain **notakey**. The corresponding
> value is undefined (white).
> The map was allocated with an initial capacity of 4. The addition of a third
> value forced an expansion to 8, as the hashtable needs enough holes to
> operate. (yellow: initial cells in use, green: additional cells in use).
> On growth all the keys and values are redistributed.
> The cells are placed according to the hash of the key. On collision the next
> available hole is used.


##### `mulle__map` stores managed keys and values with minimal overhead

This is `mulle_map` minus the `allocator` and the `callback`. It is the basis
for the `NSMutableDictionary` implementation.

![](pix/mulle--map.svg)


##### `mulle__pointermap` stores unmanaged keys and values

This is a simplified version of `mulle__map`. Equality is determined by
comparing pointers with `==` and holes are always filled with
`mulle_not_a_pointer` (which is `INTPTR_MIN`). Therefore no callbacks are used
by `mulle__pointermap`.

![](pix/mulle--pointermap.svg)

### Sets

A set is the most simple hashtable. It's like a map minus the value part.
The storage of pointers and objects in a set is not very memory efficient,
compared to an array, but searching is fast for sets with larger item
quantities. (ca. > 100)

#### `mulle_set` conveniently stores managed pointers for quick lookup

![](pix/mulle-set.svg)

#### `mulle__set` stores managed pointers with minimal overhead

The `mulle__set` is a `mulle_set` without the `allocator` and the `callback`.

![](pix/mulle--set.svg)


##### `mulle__pointerset` stores unmanaged pointers with minimal overhead

This is a simplified version of `mulle__set`. Equality is determined by
comparing pointers with `==` and holes are always filled with
`mulle_not_a_pointer` (which is `INTPTR_MIN`). Therefore no callbacks are used
by `mulle__pointerset`.

![](pix/mulle--pointerset.svg)



## Predefined Callbacks

### Key Callbacks

As used by `mulle_array`, `mulle_map`, `mulle_set`. Each callback name is to
be prefixed with `mulle_container_keycallback_`, which has been elided for
brevity:

| Name                        | Type       | Notakey      | Equality | Copies | Frees
|-----------------------------|------------|--------------|----------|--------|-------
| `_int`                      | `int`      | `INTPTR_MIN` | `==`     | NO     | NO
| `_intptr`                   | `intptr_t` | `INTPTR_MIN` | `==`     | NO     | NO
| `_nonowned_pointer_or_null` | `void *`   | `INTPTR_MIN` | `==`     | NO     | NO
| `_nonowned_pointer`         | `void *`   | `NULL`       | `==`     | NO     | NO
| `_owned_pointer`            | `void *`   | `NULL`       | `==`     | NO     | YES
| `_copied_cstring`           | `char *`   | `NULL`       | `strcmp` | YES    | YES
| `_nonowned_cstring`         | `char *`   | `NULL`       | `strcmp` | NO     | NO
| `_owned_cstring`            | `char *`   | `NULL`       | `strcmp` | NO     | YES

### Value Callbacks

As used by `mulle_map`. Value callbacks don't need to hash and check for
equality. Each callback name is to be prefixed with
`mulle_container_valuecallback`, which has been elided for brevity.


| Name                | Type       | Copies | Frees
|---------------------|------------|--------|-------
| `_int`              | `int`      | NO     | NO
| `_intptr`           | `intptr_t` | NO     | NO
| `_nonowned_pointer` | `void *`   | NO     | NO
| `_owned_pointer`    | `void *`   | NO     | YES
| `_copied_cstring`   | `char *`   | YES    | YES
| `_nonowned_cstring` | `char *`   | NO     | NO
| `_owned_cstring`    | `char *`   | NO     | YES


## Efficiency

As time critical sections are all inlined, the performance of the
library code is as good as your compiler (within the runtime limitations of the
algorithm.

Here is an example of mulle-container code, compiled with -Os:

``` c
int   _mulle__pointerset_count_zeroes_generic( struct mulle__pointerset *src,
                                               struct mulle_container_keycallback *callback,
                                               struct mulle_allocator *allocator)
{
   struct mulle__genericpointersetenumerator  rover;
   void                                       *item;
   int                                        rval;

   rval  = 0;
   rover = _mulle__pointerset_enumerate_generic( src, callback);
   while( _mulle__genericpointersetenumerator_next( &rover, &item))
   {
       if( item == 0)
           ++rval;
   }
   mulle__genericpointersetenumerator_done( &rover);
   return( rval);
}
```

produces:

``` nasm
_mulle__pointerset_count_zeroes_generic(mulle__pointerset*, mulle_container_keycallback*, mulle_allocator*): # @_mulle__pointerset_count_zeroes_generic(mulle__pointerset*, mulle_container_keycallback*, mulle_allocator*)
        mov     rcx, qword ptr [rdi + 8]
        test    rcx, rcx
        je      .LBB0_1
        mov     rdx, qword ptr [rsi + 40]
        mov     rsi, qword ptr [rdi]
        xor     eax, eax
.LBB0_3:                                # =>This Inner Loop Header: Depth=1
        mov     rdi, qword ptr [rsi]
        add     rsi, 8
        cmp     rdi, rdx
        je      .LBB0_3
        cmp     rdi, 1
        adc     eax, 0
        dec     rcx
        jne     .LBB0_3
        ret
.LBB0_1:
        xor     eax, eax
        ret
```



### You are here

![Overview](overview.dot.svg)





## Add

**This project is a component of the [mulle-core](//github.com/mulle-core/mulle-core) library. As such you usually will *not* add or install it
individually, unless you specifically do not want to link against
`mulle-core`.**


### Add as an individual component

Use [mulle-sde](//github.com/mulle-sde) to add mulle-container to your project:

``` sh
mulle-sde add github:mulle-c/mulle-container
```

To only add the sources of mulle-container with dependency
sources use [clib](https://github.com/clibs/clib):


``` sh
clib install --out src/mulle-c mulle-c/mulle-container
```

Add `-isystem src/mulle-c` to your `CFLAGS` and compile all the sources that were downloaded with your project.


## Install

### Install with mulle-sde

Use [mulle-sde](//github.com/mulle-sde) to build and install mulle-container and all dependencies:

``` sh
mulle-sde install --prefix /usr/local \
   https://github.com/mulle-c/mulle-container/archive/latest.tar.gz
```

### Manual Installation

Install the requirements:

| Requirements                                 | Description
|----------------------------------------------|-----------------------
| [mulle-allocator](https://github.com/mulle-c/mulle-allocator)             | 🔄 Flexible C memory allocation scheme
| [mulle-data](https://github.com/mulle-c/mulle-data)             | #️⃣ A collection of hash functions

Download the latest [tar](https://github.com/mulle-c/mulle-container/archive/refs/tags/latest.tar.gz) or [zip](https://github.com/mulle-c/mulle-container/archive/refs/tags/latest.zip) archive and unpack it.

Install **mulle-container** into `/usr/local` with [cmake](https://cmake.org):

``` sh
cmake -B build \
      -DCMAKE_INSTALL_PREFIX=/usr/local \
      -DCMAKE_PREFIX_PATH=/usr/local \
      -DCMAKE_BUILD_TYPE=Release &&
cmake --build build --config Release &&
cmake --install build --config Release
```


## Author

[Nat!](https://mulle-kybernetik.com/weblog) for Mulle kybernetiK



