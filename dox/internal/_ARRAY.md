# `_mulle_array`

`_mulle_array` is the basis for the more convenient [`mulle_array`](API_array.md). The difference between both data structures is, that `mulle_array` has the allocator and the callbacks embedded into the struct, whereas `_mulle_array` needs both provided as a parameter in most function calls.

Example:

```
void   _mulle__array_reset( struct mulle__array *array,
                           struct mulle_container_keycallback *callback,
                           struct mulle_allocator *allocator);
```

vs.

```
void   mulle_array_reset( struct mulle_array *array)
```

## Types

```
struct mulle__array
```


## Functions

Same as [`mulle_array`](API_array.md) functions, but using the `_mulle_array` prefix instead.



