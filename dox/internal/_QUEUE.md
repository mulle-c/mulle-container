# `_mulle_queue`

This is a LIFO array. It can' be used a s stack.


## Types



```
struct _mulle_queue;
struct _mulle_queueenumerator
```

## Functions

### Verns

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
* remove_all


### `_mulle_queue_init`

```
void  _mulle_queue_init( struct _mulle_queue *queue,
                         unsigned short bucket_size,
                         unsigned short spare_allowance)
```

### `_mulle_queue_create`

```
struct _mulle_queue   *_mulle_queue_create( unsigned short bucket_size,
                                            unsigned short spare_allowance,
                                            struct mulle_allocator *allocator);
```

