# mulle-container

A collection of C (C99) containers, with callbacks that are compatible to 
`NSHashtable` and friends. Used and wrapped in MulleObjCFoundation.



## What's in here



File                         | Description
---------------------------- | ----------------------------------------
`mulle_container_callback `  | Callback structures for sets, arrays and maps.
                             |
`mulle_array`                | Wrapper for `_mulle_array` containing the allocator and the callback.
`_mulle_array`               | A growing mutable array of void pointers (-> objects) with enumeration. (NSMutableArray)
`mulle_pointerarray`         | A growing mutable array of void pointers w/o callbacks. 
`mulle_pointerpairarray`     | A growing mutable array of pairs of void pointers w/o callbacks. 
                             |
`mulle_buffer`               | Wrapper for `_mulle_buffer` containing the allocator and the callback.
`_mulle_buffer`              | A multi purpose byte container/stream. Usually used as a growing buffer of bytes. (NSMutableData)
                             |
`mulle_hash`                 | The default hash. Currently it's a wrapper for the [CityHash](https://en.wikipedia.org/wiki/CityHash). The choice of CityHash is pretty close to arbitrary, it might change in the future.
`mulle_prime`                | A simple scheme to get prime values for bit depths (up to 32 bit)
                             |
`mulle_bigmap`               | Wrapper for `_mulle_bigmap` containing the allocator and the callback.
`_mulle_bigmap`              | A two level growing hashmap (key indexing value map). Deemed useful for really big hash tables.
`mulle_map`                  | Wrapper for `_mulle_map` containing the allocator and the callback.
`_mulle_map `                | A single level growing hashmap (key indexing value map). Your standard key/value associating hashtable (NSMutableDictionary)
                             |
`_mulle_queue`               | A LIFO queue for void pointers w/o callbacks. (TODO: rename to pointerqueue)
                             |
`mulle_bigset`               | Wrapper for `_mulle_bigset` containing the allocator and the callback.
`_mulle_bigset`              | A two level growing hashmap (key indexing value map). Deemed useful for really big hash tables.
`mulle_set`                  | Wrapper for `_mulle_set` containing the allocator and the callback.
`_mulle_set`                 | A single level growing hashed set. (NSMutableSet)
`mulle_pointerset`           | A binary searching set of void pointers, based on pointer equality. Useful for very small sets.
                           
 
