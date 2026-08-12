# mulle-container Components

## Component Catalog

### Arrays — Contiguous `void *` Storage

| Component | Header | Description |
|-----------|--------|-------------|
| `mulle_array` | `src/array/mulle-array.h` | Managed pointer array with callback-driven equality, copy, and free. Stores allocator + callback. |
| `mulle__array` | `src/array/mulle--array.h` | Minimal managed pointer array. No stored allocator/callback. |
| `mulle_pointerarray` | `src/array/pointer/mulle-pointerarray.h` | Unmanaged `void *` array. Pointer equality (`==`). Stores allocator. |
| `mulle__pointerarray` | `src/array/pointer/mulle--pointerarray.h` | Minimal unmanaged pointer array. No stored allocator. Foundational building block for other arrays. |
| `mulle_structarray` | `src/array/struct/mulle-structarray.h` | Array of arbitrary C types (not just pointers). Stores element size + allocator. Proper alignment. |
| `mulle__structarray` | `src/array/struct/mulle--structarray.h` | Minimal struct array. No stored allocator. |

**When to use which array:**
- Need callback lifecycle (copy/free on insert/remove)? → `mulle_array`
- Just storing raw pointers or casted integers? → `mulle_pointerarray`
- Storing `float`, `struct`, or other non-pointer C types? → `mulle_structarray`
- Building infrastructure code that manages its own allocator? → `mulle__*` variants

### Associative Arrays — Ordered Key/Value Pairs

| Component | Header | Description |
|-----------|--------|-------------|
| `mulle_pointerpairarray` | `src/assoc/pointerpair/mulle-pointerpairarray.h` | Ordered array of `{key, value}` pointer pairs. Stores allocator. |
| `mulle__pointerpairarray` | `src/assoc/pointerpair/mulle--pointerpairarray.h` | Minimal pointer pair array. No stored allocator. |
| `mulle_assoc` | `src/assoc/mulle-assoc.h` | Sorted associative array with binary search. Built on `mulle__pointerpairarray`. Stores allocator + callback. |
| `mulle__assoc` | `src/assoc/mulle--assoc.h` | Minimal sorted associative array. No stored allocator/callback. |

**When to use:**
- Need ordered key/value storage with fast search? → `mulle_assoc`
- Need simple ordered pairs, don't care about search speed? → `mulle_pointerpairarray`
- Small number of key/value pairs (< ~100)? → `mulle_pointerpairarray` (lower overhead than map)

### Maps — Hashtable Key/Value Storage

| Component | Header | Description |
|-----------|--------|-------------|
| `mulle_map` | `src/map/mulle-map.h` | Callback-driven hashtable map. Stores allocator + key/value callbacks. |
| `mulle__map` | `src/map/mulle--map.h` | Minimal callback-driven map. No stored allocator/callback. Basis for `NSMutableDictionary`. |
| `mulle__pointermap` | `src/map/pointer/mulle--pointermap.h` | Pointer-equality map. No callbacks. Uses `==` for keys. |

**When to use:**
- Need fast key/value lookup with custom equality? → `mulle_map`
- Keys are pointers compared by identity? → `mulle__pointermap`
- Fewer than ~100 entries and order matters? → `mulle_assoc` instead

### Sets — Hashtable Membership

| Component | Header | Description |
|-----------|--------|-------------|
| `mulle_set` | `src/set/mulle-set.h` | Callback-driven hashtable set. Stores allocator + callback. |
| `mulle__set` | `src/set/mulle--set.h` | Minimal callback-driven set. No stored allocator/callback. |
| `mulle__pointerset` | `src/set/pointer/mulle--pointerset.h` | Pointer-equality set. No callbacks. Uses `==`. |
| `mulle_pointerset` | `src/set/pointer/mulle-pointerset.h` | Pointer-equality set with stored allocator. |

**When to use:**
- Need membership testing with custom equality? → `mulle_set`
- Just tracking pointer identity? → `mulle_pointerset`

### Queues — FIFO Bucket-Chained Storage

| Component | Header | Description |
|-----------|--------|-------------|
| `mulle_pointerqueue` | `src/queue/mulle-pointerqueue.h` | FIFO queue for `void *`. Bucket-chained (no realloc). Stores allocator. |
| `mulle__pointerqueue` | `src/queue/mulle--pointerqueue.h` | Minimal pointer queue. No stored allocator. |
| `mulle_structqueue` | `src/queue/struct/mulle-structqueue.h` | FIFO queue for arbitrary structs. Stable element pointers. Stores allocator. |
| `mulle__structqueue` | `src/queue/struct/mulle--structqueue.h` | Minimal struct queue. No stored allocator. |

**When to use:**
- Need FIFO with many additions and no realloc? → `mulle_pointerqueue`
- Need stable pointers to queued elements? → `mulle_structqueue` (unlike structarray, elements don't move)

### Range Structures

| Component | Header | Description |
|-----------|--------|-------------|
| `mulle__rangeset` | `src/rangeset/mulle--rangeset.h` | Sorted, auto-coalescing set of `mulle_range`. Like `NSIndexSet`. |
| `mulle__rangemap` | `src/rangemap/mulle--rangemap.h` | Range-to-value mapping. |

### Callback Infrastructure

| Component | Header | Description |
|-----------|--------|-------------|
| `mulle_container_keycallback` | `src/mulle-container-callback.h` | Key callback: hash, equality, copy, free, notakey |
| `mulle_container_valuecallback` | `src/mulle-container-callback.h` | Value callback: copy, free (no hash/equality needed) |
| `mulle_container_keyvaluecallback` | `src/mulle-container-callback.h` | Combined key + value callback pair for maps |


