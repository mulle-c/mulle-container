# `mulle_pointerarray`

`mulle_pointerarray` is a limited growing array of pointers. You can overwrite pointers too and remove the last item. It is useful as a stack.

## Types

```
struct mulle_pointerarray
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
* `get_notapointer`

### Operations Verbs

* `add`
* `get`
* `index`
* `remove_last`
* `find_last`
* `set`
* `member`

### Enumeration Verbs

* `enumerate`
                       

### `mulle_pointerarray_init`

```
void   mulle_pointerarray_init( struct mulle_pointerarray *array,
                                unsigned int  capacity,
                                void *notapointer,
                                struct mulle_allocator *allocator)
```

Initialize an unused `set` with `notapointer` and `allocator`. With capacity you can specify an initial size of the array.

`notapointer` is usually NULL, but if you want to store NULLs in the pointerarray, then you could use `mulle_container_not_a_pointer_key`.

`allocator` is a mulle-allocator. It can be NULL for the default allocator. 
