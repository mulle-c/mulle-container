# `_mulle_bigmap`

`_mulle_bigmap` is the basis for the more convenient [`mulle_bigmap`](API_bigmap.md).
The difference between both data structures is, that `mulle_bigmap` has the
allocator and the callbacks embedded into the struct, whereas `_mulle_bigmap`
needs both provided as a parameter in most function calls.

Example:

```
void   _mulle_bigmap_reset( struct _mulle_bigmap *bigmap,
                           struct mulle_container_keyvaluecallback *callback,
                           struct mulle_allocator *allocator);
```

vs.

```
void   mulle_bigmap_reset( struct mulle_bigmap *bigmap)
```

## Types

```
struct _mulle_bigmap
```


## Functions

Same as [`mulle_bigmap`](API_bigmap.md) functions, but using the `_mulle_bigmap`
prefix instead.



