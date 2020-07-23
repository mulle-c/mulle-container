# mulle-container

#### 🛄 Arrays, hashtables and a queue

A collection of C (C11) data structures dealing mostly with void pointers and Objective-C objects.

All data structures can live (temporarily) on the stack, or permanently in the
heap. None of them are thread-safe.
[mulle-allocator](//github.com/mulle-c/mulle-allocator) is used pervasively to simplify memory management.
Some data structures utilize callbacks that are compatible to OS X's
[`NSHashtable`](//nshipster.com/nshashtable-and-nsmaptable/) and friends.

Build Status | Release Version
-------------|-----------------------------------
[![Build Status](https://travis-ci.org/mulle-c/mulle-container.svg?branch=release)](https://travis-ci.org/mulle-c/mulle-container) | ![Mulle kybernetiK tag](https://img.shields.io/github/tag/mulle-c/mulle-container.svg) [![Build Status](https://travis-ci.org/mulle-c/mulle-container.svg?branch=release)](https://travis-ci.org/mulle-c/mulle-container)


## Aspects

### Algorithms

There are three basic algorithms employed by the data structures in
mulle-container. The hashtable, the array and the queue.

Both the array and the hashtable use a contiguous block of memory.
An array reallocates on growth. A hashtable copies on growth. The queue is a linked list of smaller memory blocks.


### Contents and "notakey"

The hashtables use "holes" to discriminate between valid entries and available
space. Consequently a hashtable can not use all possible `void *` values as
keys and needs a special **notakey** marker for such holes.

The "notakey" value is usually `mulle_not_a_pointer`, which is defined as
`(void *) INTPTR_MIN`.


### Embedded allocator

For many data structures there exist two variants. One
that keeps a reference to the memory allocator, and one that doesn't. The
data structure without, is obviously smaller than the one with.
If you are managing millions of hashtables, as may be the case when doing
database fetches, this can be significant.
On the other hand, embedding the allocator makes the API be simpler and
less susceptible to allocator mix ups.

### NULL leniency

Each data structure supplies rigid functions and lenient functions, with
respect to NULL parameters. The rigid functions are prefixed with '`_`' and
do not check for NULL pointers. The lenient functions do nothing, if NULL is
passed. This is like Objective-C where you can call methods with 'nil'.

### Pointers or Objects

A data structure, conventionally containing the word "pointer" in its name, may merely handle pointer equality as a way of comparing
members. This is efficient and sometimes all you need. Other
data structures use a number of **callback**, to test for equality, memory management and creating hash codes.

## Example

Here is an example using `mulle_map` to associate c-strings with each other.
All the necessary memory management (copying of keys and values) is performed
by `mulle_map` using the callbacks contained in a `mulle_container_keyvaluecallback` struct. The on-stack version is commented out:


```
#include <mulle-container/mulle-container.h>

static struct mulle_container_keyvaluecallback   callback;

static void  test( void)
{
//   struct mulle_map              map;
   struct mulle_map              *map;
   struct mulle_mapenumerator    rover;
   void                          *key;
   void                          *value;

//   map = mulle_map_init( 0, &callback, NULL);
   map = mulle_map_create( 0, &callback, NULL);

   mulle_map( map, "VfL", "VFL");
   mulle_map( map, "Bochum", "BOCHUM");
   mulle_map( map, "1848", "1848");

   if( ! mulle_map_get( map, "1849"))
   {
	   rover = mulle_map_enumerate( map);
	   while( mulle_mapenumerator_next( &rover, &key, &value))
		   printf( "%s : %s\n", key, value);
	   mulle_mapenumerator_done( &rover);
	}
   mulle_map_remove( map, "1848");

//   mulle_map_done( &map);
   mulle_map_destroy( map);
}


int   main( void)
{
   // use predefined structs to compose our custom callback
   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   test();
   return( 0);
}
```


## API

The names of the functions are consistent. Each function is a **verb**
that is prefixed with the name of the data structure it handles. So for example
the `get` function for `mulle_array` is `mulle_array_get`. The first parameter,
except for creation functions, is always the container itself.

Check out [SYNTAX.md](dox/SYNTAX.md) for a somewhat formal
explanation and [VERBS.md](dox/VERBS.md) for a list of common verbs being employed.

The various functions `assert` their parameters, but when compiled for release
there are no runtime checks. For development it is wise to use a debug version
of the library.

### Arrays

All arrays store their elements in a single block of memory.

#### Standard Arrays

##### `mulle_array`

This is an array of `void *` that `realloc`s on power of two sizes. The
elements equality is determined with a callback function and the elements
can be copied/freed or reference counted using
[`mulle_container_callback`s](dox/API_CONTAINER_CALLBACK.md). Deletions use
`memmove`. It could be the basis for a `NSMutableArray` implementation
(but isn't currently).

![](pix/mulle-array.svg)

> The array was allocated with an initial capacity of 4. The addition of a fifth value forced an expansion to 8 (yellow: old cells, green: new cells). The "notakey" is not used for arrays.


##### `mulle__array`

This is `mulle_array` minus the `allocator` and `callback`.

![](pix/mulle--array.svg)


##### `mulle_pointerarray`

This is a stripped down version of `mulle_array` to manage
untyped `void *`. The pointers are simply compared with `==`.

![](pix/mulle-pointerarray.svg)


##### `mulle__pointerarray`

This is a stripped down version of `mulle_pointerarray` minus the `allocator`. It can be
convenient for quickly building up C arrays of `void *`.

![](pix/mulle--pointerarray.svg)


#### Special Arrays

##### `mulle__uniquepointerarray`

`mulle__uniquepointerarray` is based on `mulle__pointerarray`. It keeps its
elements of `void *` in an array in sorted order for searching by pointer
equality. The sorting is done lazily. It's use is as a set. It's
advantage over a hashtable set are low memory requirement and superior
performance for smaller quantities.


##### `mulle__rangeset`

This is an array of `mulle_range` elements, that are always kept in sorted
order. A `mulle_range` is defined as

``` objective-c
struct mulle_range
{
   uintptr_t   location;
   uintptr_t   length;
};
```

which is identical to `NSRange`. Ranges are combined and split as necessary, so that
`mulle_rangeset` can ensure that

* there are no overlapping ranges (they are coalesced)
* there are no adjacent ranges (they are coalesced)
* there are no ranges of size 0 (they are removed)

It could be the basis for an `NSIndexSet`.


#### `mulle_structarray`

This is a variant of `mulle_pointerarray`, but instead of using `void *` you
can specify any C type as the element size. This can be useful for building
arrays of `float` or any kind of `struct` for example.


#### Associative Arrays

##### `mulle_pointerpairarray`

This is modified `mulle_pointerarray` that holds key/value pairs of
`void *`, `void *` instead of just a single `void *`. In comparison to using a
map, order of addition is preserved and the space requirement is lower. But
it is slow in searches.

![](pix/mulle-pointerpairarray.svg)

##### `mulle__pointerpairarray`

This is the `mulle_pointerpairarray` variant without the `allocator`.

### Queues

#### `mulle__pointerqueue`

This is a FIFO queue for `void *`. It does not `realloc` on addition. Instead
it chains buckets of `void *` together. This makes large quantities of additions
pretty much as fast as possible.

![](pix/mulle--pointerqueue.svg)

> Filled cells indicate a cell containing a pointer value. All values are possible, so there are no holes.
> The cells are clustered in buckets and the buckets form a linked list. The `_read_index` maintains
> the number of values already read from the current `_read` bucket. Correspondingly the `_write_index`
> keeps a record of the next available index in  the `_write` bucket. If a bucket is exhausted a new one
> is allocated, chained into the linked list and the `_write` pointer is set to it, resetting the
> `_write_index`.


It is the basis for the `NSAutoreleasePool` implementation.


### Hashtables

A hashtable is
about twice the size of the number of elements contained within, the rest being
holes. A hash of the element is used to quickly
locate the entry in the hashtable.

Hashtable `realloc` and redistribute their
elements on power of two size boundaries.

#### Maps

Hashtable data structures that provide associative storage (key/value) are called maps.

##### `mulle_map`

The elements equality is determined with a callback function and the elements
can be copied/freed or reference counted using callbacks organized in a
[`mulle_container_callback`](dox/API_CONTAINER_CALLBACK.md).

![](pix/mulle-map.svg)

> The contigous memory is split into two equal sized parts "KEYS" and "VALUES".
> Gray cells indicate a "hole", they contain **notakey**. The corresponding value is undefined (white).
> The map was allocated with an initial capacity of 4. The addition of a third value forced an expansion to 8, as the hashtable needs enough holes to operate. (yellow: old cells, green: new cells).
> On growth all the keys and values are redistributed.
> The cells are placed according to the hash of the key. On collision the next available hole is used.


##### `mulle__map`

This is `mulle_map` minus the `allocator` and the `callback`. It is the basis for
the `NSMutableDictionary` implementation.

![](pix/mulle--map.svg)


##### `mulle__pointermap`

This is a simplified version of `mulle__map`. Equality is determined by comparing pointers with `==` and holes are always filled with `mulle_not_a_pointer` (which is `INTPTR_MIN`). Therefore no callbacks are used by `mulle__pointermap`.

![](pix/mulle--pointermap.svg)

### Sets

A set is a map minus the value part. The storage of pointers and objects in a set is not very efficient, compared to an array, but searching is fast for larger item quantities.

#### `mulle_set`

![](pix/mulle-set.svg)

#### `mulle__set`

The `mulle__set` is a `mulle_set` without the `allocator` and the `callback`.

![](pix/mulle--set.svg)


##### `mulle__pointerset`

This is a simplified version of `mulle__set`. Equality is determined by comparing pointers with `==` and holes are always filled with `mulle_not_a_pointer` (which is `INTPTR_MIN`). Therefore no callbacks are used by `mulle__pointerset`.

![](pix/mulle--pointerset.svg)


File                                                         | Description
------------------------------------------------------------ | ----------------------------------------
[`mulle_container_callback`](dox/API_CONTAINER_CALLBACK.md)  | Callback structures for sets, arrays and maps.
&nbsp;                                                       | &nbsp;
[`mulle_array`](dox/API_ARRAY.md)                            | A growing mutable array of void pointers (-> objects) with enumeration. (NSMutableArray)
[`mulle_pointerarray`](dox/API_POINTERARRAY.md)              | A growing mutable array of void pointers w/o callbacks.
[`mulle_pointerpairarray`](dox/API_POINTERPAIRARRAY.md)      | A growing mutable array of pairs of void pointers w/o callbacks.
&nbsp;                                                       | &nbsp;
[`mulle_hash`](dox/API_HASH.md)                              | The default hash. Currently it's a wrapper for the [CityHash](https://en.wikipedia.org/wiki/CityHash). The choice of CityHash is pretty close to arbitrary, it might change in the future.
[`mulle_prime`](dox/API_PRIME.md)                            | A simple scheme to get prime values for bit depths (up to 32 bit)
&nbsp;                                                       | &nbsp;
[`mulle_map`](dox/API_MAP.md)                                | A single level growing hashmap (key indexing value map). Your standard key/value associating hashtable (NSMutableDictionary)
&nbsp;                                                       | &nbsp;
[`mulle_set`](dox/API_SET.md)                                | A single level growing hashed set. (NSMutableSet)
[`mulle_uniquepointerarray`](dox/API_uniquepointerarray.md)  | A binary searching set of void pointers, based on pointer equality. Useful for very small sets.



## Efficiency

As time critical sections are all inlined, the performance of the
library code is as good as your compiler (within the runtime limitations of the algorithm.

Here is an example of mulle-container code, compiled with -Os:

```

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

```
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


## Add

Use [mulle-sde](//github.com/mulle-sde) to add mulle-container to your
mulle-sde project:

```
mulle-sde dependency add --c --github mulle-c mulle-container
```

## Install

### mulle-sde

Use [mulle-sde](//github.com/mulle-sde) to build and install mulle-container and all dependencies:

```
mulle-sde install --prefix /usr/local \
   https://github.com/mulle-c/mulle-container/archive/latest.tar.gz
```

### Manual Installation


Install the requirements:

Requirements                                             | Description
---------------------------------------------------------|-----------------------
[mulle-allocator](//github.com/mulle-c/mulle-allocator)  | Memory allocation wrapper
[farmhash](//github.com/mulle-c/farmhash)                | A hash function

Install into `/usr/local`:

```
mkdir build 2> /dev/null
(
   cd build ;
   cmake -DCMAKE_INSTALL_PREFIX=/usr/local \
         -DCMAKE_PREFIX_PATH=/usr/local \
         -DCMAKE_BUILD_TYPE=Release .. ;
   make install
)
```



## Platforms and Compilers

All platforms and compilers supported by
[mulle-c11](//github.com/mulle-c/mulle-c11).



## Author

[Nat!](//www.mulle-kybernetik.com/weblog) for
[Mulle kybernetiK](//www.mulle-kybernetik.com) and
[Codeon GmbH](//www.codeon.de)
