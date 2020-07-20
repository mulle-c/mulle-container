# mulle-container

#### 🛄 Sets, hashtables, arrays and more - written in C

A collection of C (C99) data structures dealing exclusively with void pointers
and their integer counterpart `intptr_t`.

All data structures can live (temporarily) on the stack, or permanently in the
heap. None of them are thread-safe.
[mulle-allocator](//github.com/mulle-c/mulle-allocator) is used pervasively
to simplify memory management.
Some utilize callbacks that are compatible to OS X's
[`NSHashtable`](//nshipster.com/nshashtable-and-nsmaptable/) and friends.

Build Status | Release Version
-------------|-----------------------------------
[![Build Status](https://travis-ci.org/mulle-c/mulle-container.svg?branch=release)](https://travis-ci.org/mulle-c/mulle-container) | ![Mulle kybernetiK tag](https://img.shields.io/github/tag/mulle-c/mulle-container.svg) [![Build Status](https://travis-ci.org/mulle-c/mulle-container.svg?branch=release)](https://travis-ci.org/mulle-c/mulle-container)

## Example

Here is an example using `mulle_map` to associate c-strings with each other.
All the necessary memory management (copying of keys and values) is performed by `mulle_map` using callbacks. The on-stack version is commented out:


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

Check out [VERBS.md](dox/VERBS.md) for a list of common verbs used.

The various functions `assert` their parameters, but when compiled for release
there are no runtime checks. For development it is wise to use a debug version
of the library.

### Arrays

All arrays store their elements in a single block of memory.

### Standard Arrays

##### `mulle_array`

This is an array of `void *` that `realloc`s on power of two sizes. The
elements equality is determined with a callback function and the elements
can be  copied/freed or reference counted using [`mulle_container_callback`s](dox/API_CONTAINER_CALLBACK.md). Deletions use `memmove`. It could be the basis
for a `NSMutableArray` implementation (but isn't).
`mulle_array` is somewhat more efficient on element deletion, as compaction is delayed. For this it
has a notion of `notakey` (usually NULL) that is used for the holes.

![](pix/mulle-array.svg)

> Filled cells indicate a cell containing a pointer value. White cells have been removed (overwritten with notakey) or have not been used yet.
> The array was allocated with an initial capacity of 4. The additions of a fifth value forced an expansion to 8 (yellow: old cells, green: new cells)
> Therefore this array has a capacity (`size`) of 8. It contains 4 values (`count`), but there is only one cell left in the arrays capacity (`size` - `used`).


##### `mulle__array`

This is `mulle_array` minus the embedded `mulle_allocator`.


##### `mulle_pointerarray`

This is a stripped down version of `mulle_array` to manage
untyped `void *`. The pointers are simply compared with `==`.

![](pix/mulle-pointerarray.svg)




##### `mulle__pointerarray`

This is a stripped down version of `mulle_pointerarray` minus the embedded
`mulle_allocator` and `notakey`. As it has no notion of
`notakey` it is not possible to erase values with random access.  It can be convenient for quickly building up C arrays of `void *`.

![](pix/mulle--pointerarray.svg)

> Filled cells indicate a cell containing a pointer value. All values are possible, so there are no holes.
> The array was allocated with an initial capacity of 4. The additions of a fifth value forced an expansion to 8 (yellow: old cells, green: new cells)
> Therefore this array has a capacity (`size`) of 8. It contains 5 values (`used`), and there are 3 cells left in the arrays capacity (`size` - `used`).

##### `mulle__uniquepointerarray`

`mulle__uniquepointerarray` is based on `mulle__pointerarray`. It keeps its elements of `void *` in an array in sorted order for searching by pointer equality. The sorting is done lazily.

##### `mulle__rangeset`

This is a array of `mulle_range` elements, that are always kept in sorted order. A `mulle_range` is defined as

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

This is a variant of `mulle_pointerarray`, but instead of using `void *` you can specify
any C type as the element size. This can be useful for building arrays of `float` for example.


#### Associative Array

##### `mulle_pointerpairarray`

This is modified `mulle_pointerarray` that holds key/value pairs of `void *`, `void *` instead of
just a single `void *`. In comparison to using a map, order of addition is preserved and the
space requirement is lower. But it is slow in searches.


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
can be copied/freed or reference counted using callbacks [`mulle_container_callback`](dox/API_CONTAINER_CALLBACK.md).


##### `mulle__map`

This is `mulle_map` minus the embedded `mulle_allocator`. It is the basis for
the `NSMutableDictionary` implementation.


##### `mulle__pointermap`

This is a stripped down version of `mulle__map` minus the callback and the
allocator. So equality is determined by comparing pointers with `==`.

### Sets

#### `mulle_set`
#### `mulle__set`



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
[`mulle_uniquepointerarray`](dox/API_uniquepointerarray.md)                  | A binary searching set of void pointers, based on pointer equality. Useful for very small sets.


Data structure names prefixed with an underscore, e.g. "mulle__set", indicate
that they are incomplete. You need to provide additional parameters like an
"allocator" for operations. Otherwise the data structure is self-contained.

### Platforms and Compilers

All platforms and compilers supported by
[mulle-c11](//github.com/mulle-c/mulle-c11).


## Add

Use [mulle-sde](//github.com/mulle-sde) to add mulle-container to your project:

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


## Author

[Nat!](//www.mulle-kybernetik.com/weblog) for
[Mulle kybernetiK](//www.mulle-kybernetik.com) and
[Codeon GmbH](//www.codeon.de)
