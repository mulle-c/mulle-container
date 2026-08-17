# Design Decisions

This document explains *why* mulle-container is designed the way it is.
Many choices appear unusual from a generic-C-library perspective but are
deliberate trade-offs optimized for the project's actual use cases: the
mulle-objc runtime, embedded contexts, and performance-critical C code.


## Why struct layouts are public

Container structs expose their fields (prefixed with `_`) in public headers.
This is intentional:

1. **Stack allocation is a first-class use case.** Containers must be
   declarable as local variables without heap allocation. This requires the
   compiler to know the struct size.

2. **Inline access eliminates function-call overhead.** The `get` operation on
   an array is literally `array->_storage[i]` after inlining. With opaque
   types, every access would require a function call through the ABI boundary.

3. **The naming convention IS the encapsulation.** Fields prefixed with `_` are
   private by convention. The single-underscore public functions are the stable
   API. This is the same model as CPython's object headers.

The trade-off is that struct layout becomes part of the practical ABI. This is
acceptable because mulle-container is consumed as source (via mulle-sde or
cmake `add_subdirectory`), not as a pre-compiled shared library with versioned
symbol tables.


## Why notakey instead of occupancy bitmaps

Footprint is a concern in this library. It is favoring code with many little
containers over the one huge container algorithm that benchmarks impressively.

Hashtables (maps, sets) use a sentinel value (`notakey`) to mark empty slots:

- **Zero extra memory per slot.** An occupancy bitmap would add 1 bit per slot
  (12.5% overhead for a pointer table). For a set with 100k entries, that's
  ~12 KB saved.

- **Simpler and faster probing.** The probe loop is a single pointer comparison
  per slot. A bitmap requires a bit-extraction operation per slot.

- **NSHashTable/NSMapTable compatibility.** The sentinel model matches Apple's
  Cocoa container design, which mulle-objc must be compatible with.

The cost is that one value cannot be stored as a key. `INTPTR_MIN` was chosen
because:
- No conforming C allocator returns this value (it would be a negative address).
- It is unlikely to appear as a meaningful integer key.
- The integer callbacks document that `INT_MIN`/`INTPTR_MIN` cannot be stored.

This is a platform contract, not a portable guarantee. The supported platforms
are LP64 and LLP64 systems where `INTPTR_MIN` is not a valid heap address.


## Why debug/release struct layouts differ

`MULLE__CONTAINER_HAVE_MUTATION_COUNT` adds a mutation counter to container
structs. By default it is set to `1` in debug builds (`NDEBUG` not defined)
and `0` in release builds (`NDEBUG` defined). You can override this by
defining `MULLE__CONTAINER_HAVE_MUTATION_COUNT` yourself before including
mulle-container headers.

The field is omitted in release builds because:

- It costs 8 bytes per container instance in production, with zero benefit.
- The standard C contract is that mixing `NDEBUG`/non-`NDEBUG` object files
  is undefined. This is the same situation as `assert()` — debug builds may
  have different behavior and layout.
- mulle-sde ensures consistent compilation flags across all translation units.

`MULLE__CONTAINER_MISER_MODE` is an alternative override that removes the
mutation counter while keeping `NDEBUG` undefined. It is incompatible with
`MULLE__CONTAINER_HAVE_MUTATION_COUNT=1` (the build will error).

**Rule:** All translation units that share container struct instances
(including consumers of public inline functions) must use the same setting of
`MULLE__CONTAINER_HAVE_MUTATION_COUNT`. Do not link debug and release object
files of mulle-container together.


## Why callbacks exist

The callback system is not gratuitous complexity. It exists because:

1. **Objective-C runtime compatibility.** `mulle_map` and `mulle_set` are the
   backing stores for `NSMutableDictionary` and `NSMutableSet` (via
   `mulle__map` / `mulle__set`). The callback model matches `NSHashTable` and
   `NSMapTable` semantics exactly.

2. **Ownership injection.** C has no destructors. Callbacks let the container
   manage object lifetimes (copy-on-insert, free-on-remove) without requiring
   the caller to manually track every pointer.

3. **Custom equality.** String keys need `strcmp`, pointer keys need `==`,
   object keys need a method dispatch. Callbacks make this pluggable without
   templates or code generation.

The complexity is concentrated in *setup* (choosing callbacks at init time),
not in *use* (calling add/get/remove). For the common cases, predefined
callback globals cover all needs:

- `_intptr` for integer keys
- `_nonowned_pointer` for pointer keys you don't want freed
- `_copied_cstring` for string keys the container should own


## Why the two-tier API (single vs double underscore)

Every container exists in two variants:

| Variant | Stores allocator/callback | Typical use |
|---------|--------------------------|-------------|
| Single underscore (e.g. `mulle_array`) | Yes, in the struct | Application code |
| Double underscore (e.g. `mulle__array`) | No, passed per call | Low-level / runtime code |

This is not a leaky abstraction — it's a deliberate choice:

- **Single underscore** is convenient. You pass the allocator once at init time and
  never think about it again.
- **Double underscore** is minimal. When you have 10,000 small containers sharing
  the same allocator/callback, storing those pointers per-instance wastes
  memory. The double-underscore variant saves 16 bytes per container.

The single-underscore functions are thin inlines that extract the stored
allocator/callback and delegate to the double-underscore layer.


## Why NULL is not a valid queue entry

`mulle_pointerqueue_pop` returns NULL to signal "queue empty." If NULL were a
valid stored value, there would be no way to distinguish "popped a NULL" from
"queue is empty" without a separate boolean return or out-parameter.

Keeping the simple `void *pop()` → NULL-means-empty contract avoids:
- An extra branch on every pop in the hot path
- A more complex API (`bool pop(queue, void **out)`) for the common case

If you need to store NULL, wrap it in a struct or use a sentinel.


## Why containers are not thread-safe

No container provides internal locking because:

1. **Locking granularity is a caller decision.** A mutex per container is
   almost never the right choice — callers typically need to lock across
   multiple operations (check-then-act patterns).

2. **Zero overhead for single-threaded use.** The mulle-objc runtime manages
   thread safety at a higher level (e.g., autorelease pool ownership). Paying
   mutex cost inside every container operation would be wasteful.

3. **Composability.** Lock-free or read-copy-update patterns are better built
   around containers than inside them.


## Documentation hierarchy

The project has a layered documentation system designed for both humans and AI:

| Layer | Location | Purpose |
|-------|----------|---------|
| Curated intro & patterns | `asset/dox/index.md` | Primary entry point — container selection, usage patterns, quirks |
| API reference | `dox/API_*.md` | Per-container function signatures and semantics |
| Design rationale | `dox/DESIGN.md` | Why things are this way (this file) |
| Howto system | `.mulle/share/howto/` | Accessed via `mulle-sde howto`; task-oriented guides |
| README | `README.md` | Overview, data structure descriptions, performance notes |

**`asset/dox/index.md` is the primary getting-started resource.** It provides
the curated "which container do I use?" decision guide, common usage patterns
with complete examples, and documents the quirks and gotchas that trip up new
users. It feeds into the `mulle-sde howto` system so that `mulle-sde howto
show` surfaces this content interactively.

The `dox/API_*.md` files are the detailed reference — they document every
public function signature but assume you already know which container you want.

The README provides a high-level overview with performance characteristics
and structural diagrams. It links to both `asset/dox/index.md` (for the
curated intro) and the individual `dox/API_*.md` files (for reference).

This layering addresses the reviewer concern about "too large a surface":
- New users start at `asset/dox/index.md` for guided container selection
- Experienced users go directly to `dox/API_*.md` for function reference
- The double-underscore layer is documented but clearly marked as advanced


## Build system: CMake files are managed

Files under `cmake/share/` and `cmake/reflect/` are generated by mulle-sde.
They should not be evaluated as hand-written code. The supported build paths
are:

- `mulle-sde craft` (the canonical build method)
- `add_subdirectory()` in a CMake project consuming mulle-container as a
  git submodule

The generated CMake artifacts use mulle-sde conventions that may look unusual
but are correct within that tooling context.
