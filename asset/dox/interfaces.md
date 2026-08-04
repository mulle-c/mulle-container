# mulle-container Interfaces

## Function Naming Convention

All public functions follow a strict naming scheme:

```
[_][mulle[_]_]<container>_<verb>[_<object>][_<modifier>]
```

### Prefix Rules

| Prefix | Meaning |
|--------|---------|
| `mulle_foo_verb` | NULL-safe convenience function (stores allocator/callback) |
| `_mulle_foo_verb` | Asserts non-NULL, convenience layer |
| `mulle__foo_verb` | NULL-safe minimal function (no stored allocator) |
| `_mulle__foo_verb` | Asserts non-NULL, minimal layer |

### Standard Verbs

See `dox/VERBS.md` for the complete reference. Key verbs:

#### Lifecycle

| Verb | Meaning | Opposite |
|------|---------|----------|
| `init` | Setup struct (stack or heap) | `done` |
| `create` | `alloc` + `init` (heap only) | `destroy` |
| `done` | Tear down, release resources | — |
| `destroy` | `done` + `free` | — |
| `reset` | Remove all elements, keep allocation | — |

#### Memory Management

| Verb | Meaning |
|------|---------|
| `guarantee` | Ensure space for N elements without realloc; returns pointer |
| `advance` | Reserve space for N elements, return pointer to first |
| `reserve` | Reserve space for 1 element, return pointer |

#### Read Access

| Verb | Meaning |
|------|---------|
| `get` | Random access read by index |
| `get_count` | Number of elements |
| `get_size` | Current capacity |
| `get_last` | Last element |
| `next` | Sequential read (enumerator) |
| `member` | Membership test (returns 0/1) |
| `find` | Linear search, returns index or `mulle_not_found_e` |

#### Write Access

| Verb | Meaning |
|------|---------|
| `add` | Append to end |
| `insert` | Random access write (does not overwrite) |
| `set` | Destructive insert (overwrites, does not return old value) |
| `update` | Destructive insert (returns old value) |
| `register` | Get-or-set (returns previous value with ownership transfer) |
| `remove` | Remove element (returns `void`) |
| `pop` | Get and remove from end |
| `push` | Add to end (queue terminology) |

#### Enumeration

| Verb | Meaning |
|------|---------|
| `enumerate` | Create forward enumerator |
| `reverseenumerate` | Create reverse enumerator |

### Standard Objects (Suffixes)

| Object | Meaning |
|--------|---------|
| `count` | Quantity of elements (not bytes) |
| `size` | Current maximum capacity |
| `length` | Quantity in bytes |
| `size_as_length` | Capacity in bytes |

## API Surface by Component

### mulle_pointerarray

```c
// Lifecycle
void   _mulle_pointerarray_init(struct mulle_pointerarray *array, unsigned int capacity, struct mulle_allocator *allocator);
void   _mulle__pointerarray_done(struct mulle__pointerarray *array, struct mulle_allocator *allocator);

// Write
void   _mulle__pointerarray_add(struct mulle__pointerarray *array, void *pointer, struct mulle_allocator *allocator);
void   _mulle__pointerarray_set(struct mulle__pointerarray *array, unsigned int index, void *pointer);
void  *_mulle__pointerarray_pop(struct mulle__pointerarray *array);

// Read
void  *_mulle__pointerarray_get(struct mulle__pointerarray *array, unsigned int index);
void  *_mulle__pointerarray_get_last(struct mulle__pointerarray *array);
unsigned int _mulle__pointerarray_get_count(struct mulle__pointerarray *array);

// Search
unsigned int _mulle__pointerarray_find(struct mulle__pointerarray *array, void *pointer);
unsigned int _mulle__pointerarray_find_in_range(struct mulle__pointerarray *array, void *pointer, struct mulle_range range);

// Memory
void   _mulle__pointerarray_grow(struct mulle__pointerarray *array, struct mulle_allocator *allocator);
void   _mulle__pointerarray_guarantee(struct mulle__pointerarray *array, unsigned int count, struct mulle_allocator *allocator);
```

### mulle_map

```c
// Lifecycle
void   _mulle_map_init(struct mulle_map *map, unsigned int capacity, struct mulle_container_keyvaluecallback *callback, struct mulle_allocator *allocator);
void   _mulle_map_done(struct mulle_map *map);
void   _mulle__map_destroy(struct mulle__map *map, struct mulle_container_keyvaluecallback *callback, struct mulle_allocator *allocator);

// Write
void   _mulle__map_set_pair(struct mulle__map *map, struct mulle_pointerpair pair, struct mulle_container_keyvaluecallback *callback, struct mulle_allocator *allocator);
void   _mulle__map_remove(struct mulle__map *map, void *key, struct mulle_container_keyvaluecallback *callback, struct mulle_allocator *allocator);

// Read
void  *_mulle__map_get(struct mulle__map *map, void *key, struct mulle_container_keyvaluecallback *callback);
struct mulle_pointerpair _mulle__map_get_pair(struct mulle__map *map, void *key, struct mulle_container_keyvaluecallback *callback);
unsigned int _mulle__map_get_count(struct mulle__map *map);
```

### mulle_set

```c
// Lifecycle
void   _mulle__set_done(struct mulle__set *set, struct mulle_container_keycallback *callback, struct mulle_allocator *allocator);

// Write
void  *_mulle__set_set(struct mulle__set *set, void *item, struct mulle_container_keycallback *callback, struct mulle_allocator *allocator);
void  *_mulle__set_insert(struct mulle__set *set, void *item, struct mulle_container_keycallback *callback, struct mulle_allocator *allocator);
void   _mulle__set_remove(struct mulle__set *set, void *item, struct mulle_container_keycallback *callback, struct mulle_allocator *allocator);

// Read
void  *_mulle__set_member(struct mulle__set *set, void *item, struct mulle_container_keycallback *callback);
unsigned int _mulle__set_get_count(struct mulle__set *set);
```

### Enumerator Pattern

All containers follow the same enumeration pattern:

```c
// Forward enumeration
struct mulle__pointerarrayenumerator rover;
void *item;

rover = _mulle__pointerarray_enumerate(array);
while (_mulle__pointerarrayenumerator_next(&rover, &item))
{
   // use item
}
mulle__pointerarrayenumerator_done(&rover);
```

For maps, the enumerator yields key/value pairs:

```c
struct mulle__mapenumerator rover;
void *key;
void *value;

rover = _mulle__map_enumerate(map, callback);
while (_mulle__mapenumerator_next(&rover, &key, &value))
{
   // use key, value
}
mulle__mapenumerator_done(&rover);
```

## Predefined Callbacks

### Key Callbacks (`mulle_container_keycallback_*`)

| Name | Type | Notakey | Equality | Copies | Frees |
|------|------|---------|----------|--------|-------|
| `_int` | `int` | `INTPTR_MIN` | `==` | No | No |
| `_intptr` | `intptr_t` | `INTPTR_MIN` | `==` | No | No |
| `_nonowned_pointer_or_null` | `void *` | `INTPTR_MIN` | `==` | No | No |
| `_nonowned_pointer` | `void *` | `NULL` | `==` | No | No |
| `_owned_pointer` | `void *` | `NULL` | `==` | No | Yes |
| `_copied_cstring` | `char *` | `NULL` | `strcmp` | Yes | Yes |
| `_nonowned_cstring` | `char *` | `NULL` | `strcmp` | No | No |
| `_owned_cstring` | `char *` | `NULL` | `strcmp` | No | Yes |

### Value Callbacks (`mulle_container_valuecallback_*`)

| Name | Type | Copies | Frees |
|------|------|--------|-------|
| `_int` | `int` | No | No |
| `_intptr` | `intptr_t` | No | No |
| `_nonowned_pointer` | `void *` | No | No |
| `_owned_pointer` | `void *` | No | Yes |
| `_copied_cstring` | `char *` | Yes | Yes |
| `_nonowned_cstring` | `char *` | No | No |
| `_owned_cstring` | `char *` | No | Yes |
