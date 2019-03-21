# `_mulle_pointerqueue`

This is a FIFO array. It can' be used as a stack.
It only contains void pointers, and doesn't manage memory except its own.


## Types



```
struct _mulle_pointerqueue;
struct _mulle_pointerqueueenumerator
```

## Functions

### Verbs

* create
* init
* destroy
* done
* shrink
* reset

* next
* push
* pop

* enumerate


### `_mulle_pointerqueue_init`

```
void  _mulle_pointerqueue_init( struct _mulle_pointerqueue *queue,
                         unsigned short bucket_size,
                         unsigned short spare_allowance)
```

### `_mulle_pointerqueue_create`

```
struct _mulle_pointerqueue   *_mulle_pointerqueue_create( unsigned short bucket_size,
                                            unsigned short spare_allowance,
                                            struct mulle_allocator *allocator);
```

