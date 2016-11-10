# mulle-container

A collection of C (C99) data structures dealing exclusively with void pointers.
They utilize callbacks that are compatible to OS X's [`NSHashtable`](//nshipster.com/nshashtable-and-nsmaptable/) and friends. Data structures can live (temporarily)
on the stack, or permanently in the heap.
[mulle-allocator](//github.com/mulle-nat/mulle-allocator) is used pervasively
to simplify memory management. None of them are thread-safe.

> This library could benefit from more tests. Do not assume, that it
> is completely bug free.

Fork      |  Build Status | Release Version
----------|---------------|-----------------------------------
[Mulle kybernetiK](//github.com/mulle-nat/mulle-container) | [![Build Status](https://travis-ci.org/mulle-nat/mulle-container.svg?branch=release)](https://travis-ci.org/mulle-nat/mulle-container) | ![Mulle kybernetiK tag](https://img.shields.io/github/tag/mulle-nat/mulle-container.svg) [![Build Status](https://travis-ci.org/mulle-nat/mulle-container.svg?branch=release)](https://travis-ci.org/mulle-nat/mulle-container)
[Community](https://github.com/mulle-objc/mulle-container/tree/release) | [![Build Status](https://travis-ci.org/mulle-objc/mulle-container.svg)](https://travis-ci.org/mulle-objc/mulle-container) | ![Community tag](https://img.shields.io/github/tag/mulle-objc/mulle-container.svg) [![Build Status](https://travis-ci.org/mulle-objc/mulle-container.svg?branch=release)](https://travis-ci.org/mulle-objc/mulle-container)


## Example

The API of the containers is fairly uniform, here is an example using
`mulle_bigmap` to associate c-strings with each other. All the necessary memory
management (copying of keys and values) is performed by `mulle_bigmap` using
callbacks:


```
#include <mulle_container/mulle_container.h>

static struct mulle_container_keyvaluecallback   callback;

static void  test( void)
{
   struct mulle_bigmap              *map;
   struct mulle_bigmapenumerator    rover;
   void                             *key;
   void                             *value;


   map = mulle_bigmap_create( 0, &callback, NULL);

   mulle_bigmap_set( map, "VfL", "VFL");
   mulle_bigmap_set( map, "Bochum", "BOCHUM");
   mulle_bigmap_set( map, "1848", "1848");

   if( ! mulle_bigmap_get( map, "1849"))
   {
	   rover = mulle_bigmap_enumerate( map);
	   while( mulle_bigmapenumerator_next( &rover, &key, &value))
   		   printf( "%s : %s\n", key, value);
	   mulle_bigmapenumerator_done( &rover);
	}
   mulle_bigmap_remove( map, "1848");

   mulle_bigmap_destroy( map);
}

int main( void)
{
   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   test();
}
```


## API

The name of the functions is consistent. Each function is a **verb**
that is prefixed with the name of the data structure it handles. So for example
the `get` function for `mulle_array` is `mulle_array_get`. The first parameter,
except for creation functions, is always the container itself.

Check out [VERBS.md](dox/VERBS.md) for a list of common verbs used.

The various functions `assert` their parameters, but when compiled for release
there are no runtime checks. For development it is wise to use a debug version
of the library.


File                                                         | Description
------------------------------------------------------------ | ----------------------------------------
[`mulle_container_callback`](dox/API_CONTAINER_CALLBACK.md)  | Callback structures for sets, arrays and maps.
                                                             |
[`mulle_array`](dox/API_ARRAY.md)                            | A growing mutable array of void pointers (-> objects) with enumeration. (NSMutableArray)
[`mulle_pointerarray`](dox/API_POINTERARRAY.md)              | A growing mutable array of void pointers w/o callbacks.
[`mulle_pointerpairarray`](dox/API_POINTERPAIRARRAY.md)      | A growing mutable array of pairs of void pointers w/o callbacks.
                                                             |
[`mulle_hash`](dox/API_HASH.md)                              | The default hash. Currently it's a wrapper for the [CityHash](https://en.wikipedia.org/wiki/CityHash). The choice of CityHash is pretty close to arbitrary, it might change in the future.
[`mulle_prime`](dox/API_PRIME.md)                            | A simple scheme to get prime values for bit depths (up to 32 bit)
                                                             |
[`mulle_bigmap`](dox/API_BIGMAP.md)                          | A two level growing hashmap (key indexing value map). Deemed useful for really big hash tables.
[`mulle_map`](dox/API_MAP.md)                                | A single level growing hashmap (key indexing value map). Your standard key/value associating hashtable (NSMutableDictionary)
                                                             |
[`mulle_bigset`](dox/API_BIGSET.md)                          | A two level growing hashmap (key indexing value map). Deemed useful for really big hash tables.
[`mulle_set`](dox/API_SET.md)                                | A single level growing hashed set. (NSMutableSet)
[`mulle_pointerset`](dox/API_POINTERSET.md)                  | A binary searching set of void pointers, based on pointer equality. Useful for very small sets.



## Install

On OS X and Linux you can use [homebrew](//brew.sh), respectively
[linuxbrew](//linuxbrew.sh) to install the library:

```
brew tap mulle-kybernetik/software
brew install mulle-container
```

On other platforms you can use **mulle-install** from
[mulle-build](//www.mulle-kybernetik.com/software/git/mulle-build) to install the library:

```
mulle-install --prefix /usr/local --branch release https://www.mulle-kybernetik.com/repositories/mulle-container
```

Otherwise read:

* [How to Build](dox/BUILD.md)


### Platforms and Compilers

All platforms and compilers supported by
[mulle-c11](//www.mulle-kybernetik.com/software/git/mulle-c11)


## Author

[Nat!](//www.mulle-kybernetik.com/weblog) for
[Mulle kybernetiK](//www.mulle-kybernetik.com) and
[Codeon GmbH](//www.codeon.de)
