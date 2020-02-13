# `_mulle_set`

`_mulle_set` is the basis for the more convenient [`mulle_set`](../API_SET.md). The
difference between both data structures is, that `mulle_set` has the allocator and the callbacks embedded into the struct, whereas `_mulle_set` needs both provided as a parameter in most function calls.

Example:

```
void   _mulle__set_reset( struct mulle__set *set,
                         struct mulle_container_keycallback *callback,
                         struct mulle_allocator *allocator);
```

vs.

```
void   mulle_set_reset( struct mulle_set *set)
```

## Types

```
struct mulle__set
```


## Functions

Same as [`mulle_set`](API_SET.md) functions, but using the `_mulle_set` prefix instead.




