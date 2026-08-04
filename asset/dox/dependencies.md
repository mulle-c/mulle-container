# mulle-container Dependencies

## Direct Dependencies

### mulle-allocator

- **Repository**: https://github.com/mulle-c/mulle-allocator
- **Purpose**: Pluggable C memory allocation scheme
- **Role in mulle-container**: All memory allocation goes through `struct mulle_allocator *`. This enables:
  - Custom allocators (pool allocators, debug allocators, etc.)
  - Fail-fast behavior (default allocator aborts on OOM — no NULL checks needed)
  - Consistent memory management across all containers
- **Key types used**: `struct mulle_allocator`, `mulle_allocator_malloc()`, `mulle_allocator_realloc()`, `mulle_allocator_free()`, `mulle_allocator_strdup()`
- **Convention**: Containers with `mulle_` prefix store the allocator in their struct. Containers with `mulle__` prefix require the allocator to be passed to each function call.

### mulle-data

- **Repository**: https://github.com/mulle-c/mulle-data
- **Purpose**: Hash functions and data utilities
- **Role in mulle-container**: Provides hash functions used by hashtable-based containers (maps, sets)
- **Key types used**: `mulle_fnv1a_hash()`, `mulle_pointer_hash()`, `struct mulle_data`, `struct mulle_range`

## Transitive Dependencies

mulle-allocator and mulle-data themselves depend on:

- **mulle-c11**: Cross-platform C11 compiler compatibility macros (`MULLE_C_UNUSED`, etc.)

## Dependency Management

Dependencies are managed via mulle-sde sourcetree:

```bash
mulle-sde dependency list              # Show current dependencies
mulle-sde dependency add github:owner/repo  # Add new dependency
```

### Dependency Marks

| Mark | Meaning |
|------|---------|
| `no-all-load` | Don't force-load all symbols |
| `no-cmake-inherit` | Don't propagate cmake settings |
| `no-import` | Don't auto-import header |
| `no-singlephase` | Multi-phase build compatible |

## Integration

mulle-container is typically consumed as part of **mulle-core** (the umbrella library). Direct use is possible but less common:

```bash
# As part of mulle-core (recommended)
mulle-sde init -d my-project -m mulle-core/c-developer executable

# As standalone dependency
mulle-sde dependency add github:mulle-c/mulle-container
```
