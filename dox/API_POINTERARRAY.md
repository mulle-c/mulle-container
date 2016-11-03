# `mulle_pointerarray`

`mulle_pointerarray` is a limited growing array of pointers. You can overwrite pointers too and remove the last item. It is useful as a stack.

** `mulle_pointerarray` does no range checking when compiled with asserts disabled.**


## Types

```
struct mulle_pointerarray
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
                       

### `mulle_pointerarray_init`

```
void   mulle_pointerarray_init( struct mulle_pointerarray *array,
                                unsigned int  capacity,
                                void *notakey,
                                struct mulle_allocator *allocator)
```

Initialize an unused `set` with `notapointer` and `allocator`. With capacity you can specify an initial size of the array.

`notakey` is usually NULL, but if you want to store NULLs in the pointerarray, then you could use `mulle_container_not_a_pointer_key`.

`allocator` is a mulle-allocator. It can be NULL for the default allocator. 



### `mulle_pointerarray_find_last`

```
void  *mulle_pointerarray_find_last( struct mulle_pointerarray *array)
```

Return the last element from `array`, if **notakey** is returned,
then thre is no last element.


### `mulle_pointerarray_remove_last`

```
void  *mulle_pointerarray_remove_last( struct mulle_pointerarray *array)
```

Removes the last element from `array` and returns it. If **notakey** is returned, then there was no last element.


### `mulle_pointerarray_set`

```
void   mulle_pointerarray_set( struct mulle_pointerarray *array,
                               unsigned int i,
                               void *p)
```

Overwrite element at index `i`  with `p`. You may overwrite with **notakey**. That is in marked difference to **add** that does not allow adding **notakey**.



