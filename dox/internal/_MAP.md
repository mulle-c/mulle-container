# `_mulle_map`

`_mulle_map` is the basis for the more convenient [`mulle_map`](../API_map.md). The difference between both data structures is, that `mulle_map` has the allocator and the callbacks embedded into the struct, whereas `_mulle_map` needs both provided as a parameter in most function calls.

Example:

```
void   _mulle_map_reset( struct _mulle_map *map,
                         struct mulle_container_keyvaluecallback *callback,
                         struct mulle_allocator *allocator);
```

vs.

```
void   mulle_map_reset( struct mulle_map *map)
```

## Types

```
struct _mulle_map
```


## Functions

Same as [`mulle_map`](API_map.md) functions, but using the `_mulle_map` prefix instead.

