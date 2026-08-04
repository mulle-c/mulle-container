# mulle-container — Codebase Information

## Project Identity

- **Name**: mulle-container
- **Version**: 10.1.0 (`MULLE__CONTAINER_VERSION ((10UL << 20) | (1 << 8) | 0)`)
- **Language**: C (C11)
- **Type**: Library (no executable entry point)
- **License**: BSD 3-Clause
- **Author**: Nat! / Mulle kybernetiK
- **Build System**: CMake via mulle-sde

## Repository Statistics

- ~55,000 lines of code
- ~2,500 functions
- ~6,600 struct/type references
- 274 prioritized source files
- 38 public header files in `src/`

## Source Directory Layout

```
src/
├── mulle-container.h              # Umbrella header
├── mulle-container-callback.h     # Callback system
├── mulle-container-callback.c
├── mulle-container-math.h         # Math utilities
├── mulle-container-operation.h    # Operation types
├── mulle-container-compiler-attributes.h
├── array/                         # Array data structures
│   ├── mulle-array.h / mulle--array.h / mulle--array.c
│   ├── pointer/                   # mulle_pointerarray
│   └── struct/                    # mulle_structarray
├── assoc/                         # Associative arrays
│   ├── mulle-assoc.h / mulle--assoc.h / mulle--assoc.c
│   └── pointerpair/               # mulle_pointerpairarray
├── map/                           # Hashtable maps
│   ├── mulle-map.h / mulle--map.h / mulle--map.c
│   └── pointer/                   # mulle__pointermap
├── set/                           # Hashtable sets
│   ├── mulle-set.h / mulle--set.h / mulle--set.c
│   └── pointer/                   # mulle__pointerset
├── queue/                         # FIFO queues
│   ├── mulle--pointerqueue.h / mulle--pointerqueue.c
│   └── struct/                    # mulle__structqueue
├── rangeset/                      # Range set
│   └── mulle--rangeset.h / mulle--rangeset.c
├── rangemap/                      # Range map
│   └── mulle--rangemap.h / mulle--rangemap.c
├── generic/                       # Generated include headers
└── reflect/                       # Auto-generated (do not edit)
```

## Test Directory Layout

```
test/
├── array/           # Array tests (insert_in_range, struct variants)
├── assoc/           # Associative array tests
├── map/             # Map tests (including pointer/ subdirectory)
├── queue/           # Queue tests (including struct/ subdirectory)
├── range/           # Range utility tests
├── rangemap/        # Range map tests (noleak, add, remove, search)
├── rangeset/        # Range set tests (noleak, add, remove, shift, stress, search, operations)
├── set/             # Set tests (including pointer/ subdirectory)
├── sort/            # Sort utility tests
└── stash/           # Stash tests
```

## Dependencies

| Dependency | Purpose |
|------------|---------|
| mulle-allocator | Pluggable memory allocation |
| mulle-data | Hash functions and data utilities |
