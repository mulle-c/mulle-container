# `mulle_pointerqueue`

`mulle_pointerqueue` is a FIFO queue for `void *` pointers. You can not use it
as a stack. It uses chained buckets internally, so it does not `realloc`/`memcpy`
on addition. This makes it faster than `mulle_pointerarray` for ever-growing
collections.

You can not store NULL into it.

It is the basis for the `NSAutoreleasePool` implementation.

### Pointer stability

The queue stores pointers (not pointed-to objects), so there is no "element
address" stability concern as with `mulle_structqueue`. The enumerator is
invalidated by `pop`, `reset`, or `removeall`.


## Types

``` c
struct mulle_pointerqueue
struct mulle_pointerqueueenumerator
```


## Functions

> For most functions there is an '`_`' version available, that is not
> NULL lenient.


### Setup

#### `mulle_pointerqueue_alloc`

``` c
struct mulle_pointerqueue *
   mulle_pointerqueue_alloc( struct mulle_allocator *allocator)
```

Allocate a `mulle_pointerqueue` on the heap. Does not set the allocator field;
use `mulle_pointerqueue_init` afterwards.


#### `mulle_pointerqueue_free`

``` c
void   mulle_pointerqueue_free( struct mulle_pointerqueue *queue)
```

Free a `mulle_pointerqueue` that was allocated with `mulle_pointerqueue_alloc`.
Does not release internal resources — call `mulle_pointerqueue_done` first if
the queue was initialized.


#### `mulle_pointerqueue_init`

``` c
void   mulle_pointerqueue_init( struct mulle_pointerqueue *queue,
                                unsigned short bucket_size,
                                unsigned short spare_allowance,
                                struct mulle_allocator *allocator)
```

Initialize a `mulle_pointerqueue`. `bucket_size` is the number of pointers per
bucket (larger = fewer allocations for mostly-add workloads).
`spare_allowance` is the number of freed buckets to cache (useful for many
edit operations). `allocator` may be NULL.


#### `mulle_pointerqueue_init_default`

``` c
void   mulle_pointerqueue_init_default( struct mulle_pointerqueue *queue,
                                        struct mulle_allocator *allocator)
```

Initialize with default bucket_size (256) and spare_allowance (16).


#### `mulle_pointerqueue_create`

``` c
struct mulle_pointerqueue *
   mulle_pointerqueue_create( unsigned short bucket_size,
                              unsigned short spare_allowance,
                              struct mulle_allocator *allocator)
```

Allocate and init a `mulle_pointerqueue` on the heap.


#### `mulle_pointerqueue_destroy`

``` c
void   mulle_pointerqueue_destroy( struct mulle_pointerqueue *queue)
```

Destroy the queue (done + free).


#### `mulle_pointerqueue_done`

``` c
void   mulle_pointerqueue_done( struct mulle_pointerqueue *queue)
```

Free internal resources.


### Accessors

#### `mulle_pointerqueue_get_count`

``` c
size_t   mulle_pointerqueue_get_count( struct mulle_pointerqueue *queue)
```

Return the number of queued pointers.


#### `mulle_pointerqueue_get_bucketsize`

``` c
size_t   mulle_pointerqueue_get_bucketsize( struct mulle_pointerqueue *queue)
```

Return the bucket size.


#### `mulle_pointerqueue_get_allocator`

``` c
struct mulle_allocator *
   mulle_pointerqueue_get_allocator( struct mulle_pointerqueue *queue)
```

Return the allocator.


### Basic Element Operations

#### `mulle_pointerqueue_push`

``` c
void   mulle_pointerqueue_push( struct mulle_pointerqueue *queue, void *p)
```

Enqueue a pointer. `p` must not be NULL. A new bucket is allocated if the
current write bucket is full.


#### `mulle_pointerqueue_add`

``` c
void   mulle_pointerqueue_add( struct mulle_pointerqueue *queue, void *p)
```

Alias for `mulle_pointerqueue_push` for orthogonality with `mulle_pointerarray`.


#### `mulle_pointerqueue_pop`

``` c
void   *mulle_pointerqueue_pop( struct mulle_pointerqueue *queue)
```

Dequeue and return the oldest pointer (FIFO). Returns NULL if the queue is
empty.


### Queue Operations

#### `mulle_pointerqueue_reset`

``` c
void   mulle_pointerqueue_reset( struct mulle_pointerqueue *queue)
```

Remove all elements and free all buckets.


#### `mulle_pointerqueue_removeall`

``` c
void   mulle_pointerqueue_removeall( struct mulle_pointerqueue *queue)
```

Remove all elements. Buckets may be cached as spares.


#### `mulle_pointerqueue_grow`

``` c
void   mulle_pointerqueue_grow( struct mulle_pointerqueue *queue)
```

Allocate a new write bucket.


#### `mulle_pointerqueue_shrink`

``` c
void   mulle_pointerqueue_shrink( struct mulle_pointerqueue *queue)
```

Free spare buckets.


### Enumeration

The enumerator interface is rarely useful since you can NOT use it to
manipulate the queue.


#### `mulle_pointerqueue_enumerate`

``` c
struct mulle_pointerqueueenumerator
   mulle_pointerqueue_enumerate( struct mulle_pointerqueue *queue)
```

Start enumeration of the queue from oldest to newest.


#### `_mulle_pointerqueueenumerator_next`

``` c
int   _mulle_pointerqueueenumerator_next( struct mulle_pointerqueueenumerator *rover,
                                          void **item)
```

Returns 1 and the next pointer in `item`. Returns 0 when done.


#### `mulle_pointerqueueenumerator_done`

``` c
void   mulle_pointerqueueenumerator_done( struct mulle_pointerqueueenumerator *rover)
```

Marks the end of the enumerator lifetime. Conventional.


### Macros

#### `MULLE_POINTERQUEUE_DATA`

``` c
MULLE_POINTERQUEUE_DATA( bucket_size, spare_allowance, allocator)
```

Compound-literal initializer for a `struct mulle_pointerqueue`. Can be used
for static or stack initialization without calling `mulle_pointerqueue_init`.


#### `mulle_pointerqueue_do`

``` c
mulle_pointerqueue_do( name)
{
   // use name as struct mulle_pointerqueue *
}
```

Stack-allocate a queue with automatic cleanup (bucket_size 64).


#### `mulle_pointerqueue_for`

``` c
mulle_pointerqueue_for( queue, item)
{
   // item is void *
}
```

Enumerate all queued pointers.
