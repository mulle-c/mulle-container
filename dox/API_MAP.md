# `mulle_map`

`mulle_map` is a container that uses `mulle_container_keyvaluecallback`
the management of it's elements.

## Types

```
struct mulle_map
```

### Creation / Destruction Verbs

* create
* destroy
* init
* done
* reset
* copy

### Accessors Verbs

* get_callback
* get_allocator
* get_count

### Element Access Read 

* get

### Element Access Write 

* insert
* remove
* set

### Operations 

* copy_items



### `mulle_map_create`

```
struct mulle_map   *mulle_map_create( size_t capacity,
                                      struct mulle_container_keyvaluecallback *callback,
                                      struct mulle_allocator *allocator)
```

### `mulle_map_init`

```
void   mulle_map_init( struct mulle_map *map,
                       size_t capacity,
                       struct mulle_container_keyvaluecallback *callback,
                       struct mulle_allocator *allocator)
```

Initalize a `map` to hold initially `capacity` items. Use `callback` to
manage items and `allocator` to manage the map itself.

The callback is NOT copied. It's usually best to keep it in static storage.


Example:

```
static struct mulle_container_keyvaluecallback   callback;
struct mulle_map                                 map;

c_string_int_callback.keycallback    = mulle_container_keycallback_copied_cstring;
c_string_int_callback.valuecallback  = mulle_container_valuecallback_int;

mulle_map_init( &map, 0, &c_string_int_callback, NULL);
```

### `mulle_map_get_notakey`

```
void   mulle_map_get_notakey( struct mulle_map *map)
```

Convenience to get the `notakey` value from the key callback of `map`.


### `mulle_map_insert_keys_and_values_v`


```
void   mulle_map_insert_keys_and_values_v( struct mulle_map *map,
                                           void *firstkey,
                                           void *firstvalue,
                                           va_list args)
```

The `va_list` version of `mulle_map_insert_keys_and_values`, see below.


### `mulle_map_insert_keys_and_values`

```
void   mulle_map_insert_keys_and_values( struct mulle_map *map,
                                         void *firstkey,
                                         void *firstvalue,
                                         ...)
```

Insert multiple keys and values, terminated by a "notakey" value.

Example

```
mulle_map_insert_keys_and_values( map,
                                  "blue", (void *) 18,
                                  "red", (void *) 48,
                                  mulle_map_get_notakey( map));
```


