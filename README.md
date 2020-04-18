# mulle-container

🛄 Sets, hashtables, arrays and more - written in C

A collection of C (C99) data structures dealing exclusively with void pointers
and their integer counterpart `intptr_t`.

They utilize callbacks that are compatible to OS X's [`NSHashtable`](//nshipster.com/nshashtable-and-nsmaptable/) and friends. Data structures can live (temporarily)
on the stack, or permanently in the heap. None of them are thread-safe.
[mulle-allocator](//github.com/mulle-c/mulle-allocator) is used pervasively
to simplify memory management.


Build Status | Release Version
-------------|-----------------------------------
[![Build Status](https://travis-ci.org/mulle-c/mulle-container.svg?branch=release)](https://travis-ci.org/mulle-c/mulle-container) | ![Mulle kybernetiK tag](https://img.shields.io/github/tag/mulle-c/mulle-container.svg) [![Build Status](https://travis-ci.org/mulle-c/mulle-container.svg?branch=release)](https://travis-ci.org/mulle-c/mulle-container)

## Example

The API of the containers is fairly uniform, here is an example using
`mulle_map` to associate c-strings with each other. All the necessary memory
management (copying of keys and values) is performed by `mulle_map` using
callbacks:


```
#include <mulle-container/mulle-container.h>

static struct mulle_container_keyvaluecallback   callback;

static void  test( void)
{
   struct mulle_map              *map;
   struct mulle_mapenumerator    rover;
   void                          *key;
   void                          *value;

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
[`mulle_pointerset`](dox/API_POINTERSET.md)                  | A binary searching set of void pointers, based on pointer equality. Useful for very small sets.


Datastructure names prefixed with an underscore, e.g. "mulle__set", indicate
that they are incomplete. You need to provide additional parameters like an
"allocator" for operations. Otherwise the datastructure is self-contained.

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
