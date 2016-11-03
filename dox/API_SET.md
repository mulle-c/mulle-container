# `mulle_set`

`mulle_set` is a hashtable for storing keys only. To store key/values use `mulle_map`. `mulle_set` uses [`_mulle_set`](API__SET.md) to do the actual work.

## Types

```
struct mulle_set
```


## Functions

### Creation / Destruction Verbs

* `alloc`
* `init` 
* `done`
* `reset` 
* `create` 
* `destroy`
* `copy`

### Accessors Verbs

* `get_allocator`
* `get_count`
* `get_keycallback`

### Element Read Verbs

* `get`

### Element Write Verbs

* `insert`
* `remove`
* `set`

### Enumeration Verbs

* `enumerate`
                       


### `mulle_set_init`

```
void   mulle_set_init( struct mulle_set *set,
                       unsigned int capacity,
                       struct mulle_container_keycallback *callback,
                       struct mulle_allocator *allocator);
```

Initialize an unused `set` with `callback` and `allocator`. With capacity you can specify an intital size of the set.

`allocator` is a mulle-allocator. It can be NULL for the default allocator. 


