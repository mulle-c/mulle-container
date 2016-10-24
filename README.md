# mulle-container

Release on [github](//github.com/mulle-nat/mulle-container): [![Build Status](https://travis-ci.org/mulle-nat/mulle-container.svg?branch=release)](https://travis-ci.org/mulle-nat/mulle-container)

A collection of C (C99) containers, with callbacks that are compatible to
[`NSHashtable`](//nshipster.com/nshashtable-and-nsmaptable/) and friends.


## API

File                                                         | Description
------------------------------------------------------------ | ----------------------------------------
[`mulle_container_callback`](dox/API_CONTAINER_CALLBACK.md)  | Callback structures for sets, arrays and maps.
                                                             |
[`mulle_array`](dox/API_ARRAY.md)                            | Wrapper for `_mulle_array` containing the allocator and the callback.
[`_mulle_array`](dox/API__ARRAY.md)                          | A growing mutable array of void pointers (-> objects) with enumeration. (NSMutableArray)
[`mulle_pointerarray`](dox/API_POINTERARRAY.md)              | A growing mutable array of void pointers w/o callbacks.
[`mulle_pointerpairarray`](dox/API_POINTERPAIRARRAY.md)      | A growing mutable array of pairs of void pointers w/o callbacks.
                                                             |
[`mulle_buffer`](dox/API_BUFFER.md)                          | Wrapper for `_mulle_buffer` containing the allocator and the callback.
[`_mulle_buffer`](dox/API__BUFFER.md)                        | A multi purpose byte container/stream. Usually used as a growing buffer of bytes. (NSMutableData)
                                                             |
[`mulle_hash`](dox/API_HASH.md)                              | The default hash. Currently it's a wrapper for the [CityHash](https://en.wikipedia.org/wiki/CityHash). The choice of CityHash is pretty close to arbitrary, it might change in the future.
[`mulle_prime`](dox/API_PRIME.md)                            | A simple scheme to get prime values for bit depths (up to 32 bit)
                                                             |
[`mulle_bigmap`](dox/API_BIGMAP.md)                          | Wrapper for `_mulle_bigmap` containing the allocator and the callback.
[`_mulle_bigmap`](dox/API__BIGMAP.md)                        | A two level growing hashmap (key indexing value map). Deemed useful for really big hash tables.
[`mulle_map`](dox/API_MAP.md)                                | Wrapper for `_mulle_map` containing the allocator and the callback.
[`_mulle_map `](dox/API__MAP.md)                             | A single level growing hashmap (key indexing value map). Your standard key/value associating hashtable (NSMutableDictionary)
                                                             |
[`_mulle_queue`](dox/API__QUEUE.md)                          | A LIFO queue for void pointers w/o callbacks. (TODO: rename to pointerqueue)
                                                             |
[`mulle_bigset`](dox/API_BIGSET.md)                          | Wrapper for `_mulle_bigset` containing the allocator and the callback.
[`_mulle_bigset`](dox/API__BIGSET.md)                        | A two level growing hashmap (key indexing value map). Deemed useful for really big hash tables.
[`mulle_set`](dox/API_SET.md)                                | Wrapper for `_mulle_set` containing the allocator and the callback.
[`_mulle_set`](dox/API__SET.md)                              | A single level growing hashed set. (NSMutableSet)
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
[mulle-c11](//www.mulle-kybernetik.com/software/git/mulle-c11/)


## Author

[Nat!](//www.mulle-kybernetik.com/weblog) for
[Mulle kybernetiK](//www.mulle-kybernetik.com) and
[Codeon GmbH](//www.codeon.de)
