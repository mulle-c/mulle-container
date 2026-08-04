# mulle-container Architecture

## Design Philosophy

mulle-container is a C11 library of data structures optimized for performance,
memory efficiency, and composability. Every design decision serves these goals:

1. **Inlined hot paths** — Time-critical operations are `static inline` in headers
2. **Pluggable allocators** — All memory goes through `mulle_allocator`, never raw `malloc`
3. **Callback-driven behavior** — Element lifecycle (hash, equality, copy, free) is configurable
4. **Stack-friendly** — All containers support stack allocation via `_init`/`_done`
5. **No thread safety** — Callers handle synchronization; no internal locks

## Two-Tier Layering Pattern

Every data structure follows a consistent two-tier design:

```mermaid
graph TB
    subgraph "Convenience Layer (single underscore)"
        A["mulle_array<br/>stores allocator + callback"]
        B["mulle_map<br/>stores allocator + callback"]
        C["mulle_set<br/>stores allocator + callback"]
    end
    subgraph "Minimal Layer (double underscore)"
        D["mulle__array<br/>no allocator, no callback"]
        E["mulle__map<br/>no allocator, no callback"]
        F["mulle__set<br/>no allocator, no callback"]
    end
    A -->|"delegates to"| D
    B -->|"delegates to"| E
    C -->|"delegates to"| F
```

- `mulle_foo` (single underscore) — Convenience wrapper. Bundles `allocator` and
  `callback` into the struct. Caller-friendly: pass fewer arguments per call.
- `mulle__foo` (double underscore) — Minimal/raw version. No stored allocator or
  callback. Lower memory overhead. Caller must pass allocator/callback to every
  function call.

The convenience layer functions are thin inlines that extract the stored
allocator/callback and forward to the minimal layer.

## Composition Hierarchy

```mermaid
graph TB
    PA["mulle__pointerarray<br/>(void* storage)"]
    A["mulle__array<br/>(+ callback equality/lifecycle)"]
    SA["mulle__structarray<br/>(arbitrary C types)"]
    PPA["mulle__pointerpairarray<br/>(key/value pairs)"]
    ASSOC["mulle__assoc<br/>(sorted key/value)"]

    PS["mulle__pointerset<br/>(pointer equality set)"]
    S["mulle__set<br/>(+ callback equality)"]

    PM["mulle__pointermap<br/>(pointer equality map)"]
    M["mulle__map<br/>(+ callback equality)"]

    PQ["mulle__pointerqueue<br/>(bucket-chained FIFO)"]
    SQ["mulle__structqueue<br/>(struct FIFO)"]

    RS["mulle__rangeset<br/>(coalescing ranges)"]
    RM["mulle__rangemap<br/>(range-to-value)"]

    PA -->|"base for"| A
    PA -->|"base for"| PPA
    PPA -->|"base for"| ASSOC
    PS -->|"base for"| S
    PM -->|"base for"| M
```

Key relationships:
- `mulle__array` is built on top of `mulle__pointerarray`, adding callback-driven lifecycle
- `mulle__assoc` is built on `mulle__pointerpairarray`, adding sorted binary search
- `mulle__set` wraps `mulle__pointerset` with callback support
- `mulle__map` wraps `mulle__pointermap` with callback support
- Queues and range structures are independent

## Generic Implementation Pattern

Hashtable-based structures (sets and maps) share generic implementation files:

```mermaid
graph LR
    GSH["mulle--pointerset-generic.{c,h}<br/>hash probing, resize, enumerate"]
    GMH["mulle--pointermap-generic.{c,h}<br/>hash probing, resize, enumerate"]

    PS["mulle__pointerset"] -->|"uses"| GSH
    S["mulle__set"] -->|"uses"| GSH
    PM["mulle__pointermap"] -->|"uses"| GMH
    M["mulle__map"] -->|"uses"| GMH
```

The `*-generic` files contain the core open-addressing hash table logic
(linear probing, power-of-two sizing, sentinel-based hole detection). The
concrete types parameterize this logic via callbacks or direct pointer
comparison.

## Memory Layout Principles

- **Arrays**: Single contiguous `void **` buffer, power-of-two realloc growth
- **Hashtables**: Single contiguous buffer split into keys region and (for maps)
  values region. ~50% fill factor. Holes marked with `mulle_not_a_pointer`
  (`(void *) INTPTR_MIN`)
- **Queues**: Linked list of fixed-size buckets. No realloc on add — O(1) enqueue
- **Range sets**: Sorted array of `mulle_range` structs, auto-coalescing

## Function Safety Tiers

| Prefix | NULL-safe | Asserts in Debug | Example |
|--------|-----------|------------------|---------|
| `mulle_foo_verb` | Yes | Yes | `mulle_array_add()` |
| `_mulle_foo_verb` | No (asserts) | Yes | `_mulle_array_add()` |
| `_mulle__foo_verb` | No (asserts) | Yes | `_mulle__array_add()` |

The non-prefixed functions protect against NULL container pointers. The
underscore-prefixed functions skip NULL checks but may assert in debug builds.
In release builds, underscore functions have zero overhead.
