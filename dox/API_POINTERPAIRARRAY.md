# `mulle_pointerpairarray`

`mulle_pointerpairarray` is a limited growing array of pointerpairs. A pointerpair is a pair of pointers :) You can overwrite pointerpairs too and remove the last item. It is useful as a stack.

#### Caveat: `mulle_pointerpairarray` does no range checking when compiled with asserts disabled.

A pointerpair with they key part is `notakey` this counts as "not found" pointerpair.

## Types

```
struct mulle_pointerpairarray
```

```
struct mulle_pointerpair
{
   void   *_key;  // suggestively named
   void   *_value;
};
```


## Functions

### Creation / Destruction Verbs

* `alloc`
* `init` - see below
* `done`
* `reset` 
* `create` 
* `destroy`
* `copy`

### Accessors Verbs

* `get_allocator`
* `get_count`
* `get_notakey`

### Operations Verbs

* `add`
* `get`
* `find`
* `set` - see below
* `member`

### Enumeration Verbs

* `enumerate`
                       

### `mulle_pointerpairarray_init`

```
void   mulle_pointerpairarray_init( struct mulle_pointerpairarray *array,
                                unsigned int  capacity,
                                void *notakey,
                                struct mulle_allocator *allocator)
```

Initialize an unused `set` with `notapointer` and `allocator`. With capacity you can specify an initial size of the array.

`notakey` is usually NULL, but if you want to store NULLs in the pointerpairarray, then you could use `mulle_container_not_a_pointer_key`.

`allocator` is a mulle-allocator. It can be NULL for the default allocator. 

### `mulle_pointerpairarray_find_last`

```
struct mulle_pointerpair  mulle_pointerpairarray_find_last( struct mulle_pointerpairarray *array)
```

Return the last element from `array`, If **notakey** is returned in `._key` then there is no last element.


### `mulle_pointerpairarray_remove_last`

```
struct mulle_pointerpair   mulle_pointerpairarray_remove_last( struct mulle_pointerpairarray *array)
```

Removes the last element from `array` and returns it. If **notakey** is returned in `._key`, then there was no last element.


### `mulle_pointerpairarray_set`

```
void   mulle_pointerpairarray_set( struct mulle_pointerpairarray *array,
                               unsigned int i,
                               struct mulle_pointerpair pair)
```

Overwrite element at index `i`  with `pair `. You may overwrite with `._key` being  **notakey**. That is in marked difference to **add** that does not allow adding a `._key` with **notakey**.



