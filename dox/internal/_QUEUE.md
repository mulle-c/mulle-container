# `_mulle_pointerqueue`

This is a FIFO array. It can' be used as a stack.
It only contains void pointers, and doesn't manage memory except its own.


## Types



```
struct mulle__pointerqueue;
struct mulle__pointerqueueenumerator
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


### `_mulle__pointerqueue_init`

```
void  _mulle__pointerqueue_init( struct mulle__pointerqueue *queue,
                         unsigned short bucket_size,
                         unsigned short spare_allowance)
```

### `mulle__pointerqueue_create`

```
struct mulle__pointerqueue   *mulle__pointerqueue_create( unsigned short bucket_size,
                                            unsigned short spare_allowance,
                                            struct mulle_allocator *allocator);
```

