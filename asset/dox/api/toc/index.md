# mulle-container Library Documentation for AI
<!-- Keywords: containers, collections -->

## 1. Introduction & Purpose

**mulle-container** provides a collection of high-performance C11 data structures for storing integers, strings, pointers, and structs. It offers:

- **Dynamic arrays** (mulle_array, mulle_pointerarray) with customizable growth and callbacks
- **Hashtables** (mulle_map, mulle_set) for fast key-value and key lookups
- **Queues** (mulle_pointerqueue, mulle_structqueue) for FIFO operations
- **Range sets** for efficient range management
- **Flexible containers**: Can live temporarily on stack or permanently on heap
- **Allocator abstraction**: Uses mulle_allocator for all memory management
- **Callback system**: Supports custom equality, hash, copy, and free functions for elements

This library is a foundational component of mulle-core and provides the basis for collection types in the Objective-C runtime and various data structure libraries.

## 2. Key Concepts & Design Philosophy

- **Callback-driven behavior**: Elements can be managed (copied/freed) or unmanaged (simple pointer storage) via mulle_container_callback
- **Naming conventions**: Functions are verbs prefixed with container name (e.g., `mulle_array_get`, `mulle_map_set`)
- **Underscore variants**: `_` prefixed functions may assert but lack NULL checks; non-prefixed versions are safe but slower
- **Power-of-two growth**: Arrays resize by doubling for amortized O(1) append performance
- **Hashtable holes**: Use "notakey" (mulle_not_a_pointer = (void*)INTPTR_MIN) to mark empty slots
- **Memory density**: Hashtables maintain ~50% fill factor with automatic rehashing
- **Lock-free design**: No thread-safety; external synchronization required
- **Stack allocation support**: Most containers can be initialized on stack with mulle_*_do macros

## 3. Core API & Data Structures

### 3.1. Container Callbacks

#### `struct mulle_container_callback`
- **Purpose**: Defines how container manages element lifecycle and equality
- **Key Fields**:
  - `unsigned (*hash)(void *)`: Hash function for element (used by hashtables)
  - `int (*equal)(void *, void *)`: Equality function for element comparison
  - `void *(*copy)(void *, struct mulle_allocator *)`: Optional copy on insert
  - `void (*free)(void *, struct mulle_allocator *)`: Optional free on remove
  - `void (*retain)(void *)`: Optional reference counting retain
  - `void (*release)(void *)`: Optional reference counting release

**Common patterns**:
- `mulle_container_callback_void_pointer`: Simple pointer comparison
- `mulle_container_callback_copying`: Copy on insert, free on remove
- `mulle_container_callback_returning`: Free on remove, no copy

### 3.2. Arrays

#### `mulle_array`
- **Purpose**: Growable array of managed `void *` pointers
- **Lifecycle**:
  - `mulle_array_create(callback, allocator)`: Allocate on heap
  - `mulle_array_init(array, callback, allocator)`: Initialize on stack
  - `mulle_array_done(array)`: Cleanup (for stack-allocated)
  - `mulle_array_destroy(array)`: Deallocate (for heap-allocated)

- **Core Operations**:
  - `mulle_array_add(array, element)`: Append element (with callback copy if specified)
  - `mulle_array_insert_at(array, index, element)`: Insert at specific index
  - `mulle_array_remove_at(array, index)`: Remove element at index (calls callback free)
  - `mulle_array_get(array, index)`: Get element at index
  - `mulle_array_find(array, element)`: Find element by equality

- **Inspection**:
  - `mulle_array_get_count(array)`: Number of elements
  - `mulle_array_get_capacity(array)`: Current capacity (allocated size)
  - `mulle_array_get_bytes(array)`: Pointer to underlying array
  - `mulle_array_is_empty(array)`: Check if empty

- **Bulk Operations**:
  - `mulle_array_remove_all(array)`: Clear all elements (calls free callbacks)
  - `mulle_array_guarantee_capacity(array, capacity)`: Pre-allocate space
  - `mulle_array_add_array(array, other)`: Merge another array

- **Macros**:
  - `mulle_array_do(array, element) { ... }`: Iterate over elements
  - `mulle_array_enumerate(array, i, element) { ... }`: Enumerate with index

#### `mulle__array` (Lightweight Array)
- Like mulle_array but without allocator and callback fields; used when management is external

#### `mulle_pointerarray`
- **Purpose**: Growable array of unmanaged `void *` pointers (simple pointer equality, no callbacks)
- Simpler than mulle_array; pointers are compared with `==` only
- No copy/free callbacks; suitable for storing raw pointers or casted integers
- **Key Functions**:
  - `mulle_pointerarray_add(array, pointer)`
  - `mulle_pointerarray_remove_at(array, index)`
  - `mulle_pointerarray_get(array, index)`
  - `mulle_pointerarray_find(array, pointer)`: O(n) linear search

#### `mulle__pointerarray` (Lightweight Pointer Array)
- Like mulle_pointerarray but without allocator field

#### `mulle_flexarray`
- **Purpose**: Dynamic array that starts on stack and moves to heap if needed (alloca replacement)
- Typical use: `mulle_flexarray_do(buf, type, size, count) { ... use buf ... }`
- Example: `mulle_flexarray_do(copy, int, 32, n) { memcpy(copy, data, n * sizeof(int)); }`
- Safe alternative to `alloca` that handles large allocations gracefully

#### `mulle__structarray`
- **Purpose**: Array of fixed-size structs instead of pointers
- Stores struct copies inline in the array
- Useful for arrays of lightweight structs without pointer indirection

### 3.3. Maps and Sets

#### `mulle_map`
- **Purpose**: Hashtable for key-value storage with callback support
- **Lifecycle**:
  - `mulle_map_create(key_callback, value_callback, allocator)`
  - `mulle_map_init(map, key_callback, value_callback, allocator)`
  - `mulle_map_done(map)`: Cleanup
  - `mulle_map_destroy(map)`: Deallocate

- **Core Operations**:
  - `mulle_map_set(map, key, value)`: Insert or update key-value pair
  - `mulle_map_get(map, key)`: Retrieve value for key
  - `mulle_map_remove(map, key)`: Remove key-value pair
  - `mulle_map_find(map, key)`: Check if key exists

- **Inspection**:
  - `mulle_map_get_count(map)`: Number of entries
  - `mulle_map_is_empty(map)`: Check if empty
  - `mulle_map_get_capacity(map)`: Current hash table size

- **Bulk Operations**:
  - `mulle_map_remove_all(map)`: Clear all entries
  - `mulle_map_set_map(map, other)`: Merge another map

- **Enumerators**:
  - `mulle_map_tinyenumerator_t`: Enumerate map entries
  - `mulle_map_get_tinyenumerator(map)`: Get enumerator
  - `mulle_map_next_tinyenumerator(enum, key_p, value_p)`: Get next entry

- **Macros**:
  - `mulle_map_for(map, key, value) { ... }`: Iterate over entries

#### `mulle__map` (Lightweight Map)
- Like mulle_map but for internal use without allocator

#### `mulle_map` with Tiny Enumerator
- `mulle_maptinyenumerator_t`: Lightweight enumerator struct for iterating map entries
- Pattern: `enumerator = mulle_map_get_tinyenumerator(map); while (mulle_map_next_tinyenumerator(&enumerator, &key, &value))`

#### `mulle__pointermap`
- **Purpose**: Hashtable using pointer keys and values without callbacks
- Simpler than mulle_map; uses direct pointer equality and hashing
- **Key Functions**:
  - `mulle__pointermap_set(map, key, value)`
  - `mulle__pointermap_get(map, key)`
  - `mulle__pointermap_remove(map, key)`

#### Sets (mulle_set, mulle__pointermap used as set)
- **Purpose**: Hashtable for membership testing (key without value)
- mulle_set behaves like mulle_map but values are ignored
- mulle__pointermap can be used as set by storing keys only

#### `mulle__rangemap`
- **Purpose**: Map-like structure for range-to-value associations
- Useful for overlapping range management

### 3.4. Queues

#### `mulle__pointerqueue`
- **Purpose**: FIFO queue for `void *` pointers using chained buckets (not resizable array)
- **Lifecycle**:
  - `mulle__pointerqueue_init(queue, allocator)`
  - `mulle__pointerqueue_done(queue)`

- **Core Operations**:
  - `mulle__pointerqueue_add(queue, pointer)`: Enqueue pointer
  - `mulle__pointerqueue_remove(queue)`: Dequeue pointer (returns NULL if empty)
  - `mulle__pointerqueue_get(queue)`: Peek at next pointer without removing

- **Inspection**:
  - `mulle__pointerqueue_is_empty(queue)`: Check if queue is empty
  - `mulle__pointerqueue_get_count(queue)`: Number of queued items

- **Design**: Uses bucketed linked list (not array); efficient for large queues

#### `mulle_pointerqueue`
- Like mulle__pointerqueue but with allocator support

#### `mulle__structqueue`
- **Purpose**: FIFO queue for fixed-size structs
- Allows maintaining stable pointers to queued elements (unlike structarray)

### 3.5. Range Management

#### `mulle__rangeset`
- **Purpose**: Ordered collection of non-overlapping ranges (like NSIndexSet)
- Stores `mulle_range` elements (location, length pairs)
- Automatically coalesces overlapping and adjacent ranges
- **Operations**:
  - Add, remove, find ranges
  - Test membership (location in range?)
  - Iterate over ranges

- **Useful for**: Selection sets, sparse arrays, interval management

#### `mulle__rangemap`
- **Purpose**: Associates values with ranges
- Similar to rangeset but each range has an associated value

### 3.6. Container Callback Functions

#### Predefined Callbacks
- `mulle_container_callback_void_pointer`: Simple pointer equality (`==`) and pointer hash
- `mulle_container_callback_void_pointers`: Multiple pointer callbacks
- `mulle_container_callback_copying`: Calls copy on insert, free on remove
- `mulle_container_callback_returning`: Calls free on remove, no copy

#### Custom Callbacks
- Create callback struct with custom hash, equal, copy, free functions
- Hash function must be consistent with equality (equal elements must have same hash)

## 4. Performance Characteristics

- **mulle_array**: O(1) amortized append, O(1) indexed access, O(n) linear search, O(n) insert/remove at arbitrary index
- **mulle_pointerarray**: Same as mulle_array but faster for unmanaged pointers
- **mulle_map / mulle__pointermap**: O(1) average get/set/remove, O(n) worst case; ~50% fill factor maintained
- **mulle__pointerqueue**: O(1) enqueue/dequeue (bucketed), handles large queues efficiently
- **mulle__rangeset**: O(log n) operations with range coalescing overhead
- **Growth strategy**: Power-of-two doubling for arrays; power-of-two rehashing for maps
- **Thread-safety**: None; external locking required

## 5. AI Usage Recommendations & Patterns

### Best Practices

1. **Choose the right container**:
   - `mulle_array`: Need callbacks (copy/free), ordered access
   - `mulle_pointerarray`: Simple unmanaged pointers, ordered access
   - `mulle_map`: Key-value lookups, any type of callback
   - `mulle__pointermap`: Simple pointer key-value, no callbacks
   - `mulle__pointerqueue`: FIFO, large quantities, no resizing

2. **Use macros for iteration**: `mulle_array_do`, `mulle_map_for` are more readable than manual loops

3. **Callbacks for element lifecycle**: Define copy/free callbacks to manage element lifecycle automatically

4. **Allocator consistency**: Use same allocator throughout container lifetime; don't mix allocators

5. **Stack allocation with _do macros**: Most containers support `mulle_*_do` pattern for automatic cleanup

### Common Pitfalls

1. **Hashtable notakey**: Can't use `mulle_not_a_pointer` as actual key; it's reserved for empty slots

2. **Callback consistency**: Hash function must be consistent with equality; unequal elements should usually hash differently

3. **Iterator invalidation**: Don't modify container during iteration (may cause crashes or data loss)

4. **Element ownership**: Clear semantics: callbacks copy on insert? Copy on read? Or borrow? Design callback strategy clearly.

5. **Enumerator safety**: Some enumerators become invalid after modification; use separate enumeration pass if modifying

6. **Underscore function safety**: Functions with `_` prefix may assert on NULL; use non-underscore versions if NULL is possible

## 6. Integration Examples

**Note**: These examples are designed to be integrated into a mulle-sde C project. To use them:
1. Create a project with: `mulle-sde init -d my-project -m mulle-core/c-developer executable`
2. Replace the content of `src/main.c` with the example code below
3. Run `mulle-sde craft && mulle-sde run` to build and execute

### Example 1: Simple dynamic array with auto-cleanup

```c
#include <mulle-container/mulle-container.h>
#include <stdio.h>

int main() {
    mulle_pointerarray_do(array) {
        mulle_pointerarray_add(array, (void *)1);
        mulle_pointerarray_add(array, (void *)2);
        mulle_pointerarray_add(array, (void *)3);
        
        printf("Count: %zu\n", mulle_pointerarray_get_count(array));
        printf("First: %p\n", mulle_pointerarray_get(array, 0));
    }
    return 0;
}
```

### Example 2: Map with string keys and pointer values

```c
#include <mulle-container/mulle-container.h>
#include <stdio.h>
#include <string.h>

static unsigned mulle_string_hash(void *s) {
    return mulle_fnv1a_hash(s, strlen(s));
}

static int mulle_string_equal(void *a, void *b) {
    return strcmp((char *)a, (char *)b) == 0;
}

int main() {
    struct mulle_container_callback key_cb = {
        .hash = mulle_string_hash,
        .equal = mulle_string_equal,
    };
    
    mulle_map_do(map, key_cb, mulle_container_callback_void_pointer) {
        mulle_map_set(map, "apple", (void *)10);
        mulle_map_set(map, "banana", (void *)20);
        
        printf("apple: %ld\n", (intptr_t)mulle_map_get(map, "apple"));
    }
    return 0;
}
```

### Example 3: FIFO queue for pointer data

```c
#include <mulle-container/mulle-container.h>
#include <stdio.h>

int main() {
    mulle__pointerqueue_do(queue) {
        mulle__pointerqueue_add(queue, (void *)1);
        mulle__pointerqueue_add(queue, (void *)2);
        mulle__pointerqueue_add(queue, (void *)3);
        
        void *item;
        while ((item = mulle__pointerqueue_remove(queue)) != NULL) {
            printf("Dequeued: %p\n", item);
        }
    }
    return 0;
}
```

### Example 4: Array iteration with macro

```c
#include <mulle-container/mulle-container.h>
#include <stdio.h>

int main() {
    mulle_pointerarray_do(array) {
        for (int i = 0; i < 5; i++)
            mulle_pointerarray_add(array, (void *)(intptr_t)i);
        
        void *element;
        mulle_pointerarray_do(array, element) {
            printf("Item: %ld\n", (intptr_t)element);
        }
    }
    return 0;
}
```

### Example 5: Array with custom callbacks (copy/free)

```c
#include <mulle-container/mulle-container.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void *copy_string(void *s, struct mulle_allocator *a) {
    size_t len = strlen((char *)s) + 1;
    char *copy = mulle_allocator_malloc(a, len);
    return strcpy(copy, (char *)s);
}

static void free_string(void *s, struct mulle_allocator *a) {
    mulle_allocator_free(a, s);
}

int main() {
    struct mulle_container_callback cb = {
        .copy = copy_string,
        .free = free_string,
    };
    
    mulle_array_do(array, cb) {
        mulle_array_add(array, "hello");
        mulle_array_add(array, "world");
        
        printf("Count: %zu\n", mulle_array_get_count(array));
        printf("First: %s\n", (char *)mulle_array_get(array, 0));
    }
    return 0;
}
```

### Example 6: Iterate map entries with enumerator

```c
#include <mulle-container/mulle-container.h>
#include <stdio.h>

int main() {
    mulle__pointermap_do(map) {
        mulle__pointermap_set(map, (void *)1, (void *)10);
        mulle__pointermap_set(map, (void *)2, (void *)20);
        
        mulle_maptinyenumerator_t enumerator = mulle__pointermap_get_tinyenumerator(map);
        void *key, *value;
        
        while (mulle_maptinyenumerator_next(&enumerator, &key, &value)) {
            printf("%p -> %p\n", key, value);
        }
    }
    return 0;
}
```

## 7. Dependencies

- `mulle-allocator`: For memory allocation throughout containers
- `mulle-data`: For hash functions and range support
- `mulle-c11`: Cross-platform C compiler glue

