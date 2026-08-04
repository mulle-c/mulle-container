# mulle-container Documentation Index

## How to Use This Documentation (for AI Assistants)

This index is the primary entry point for understanding the mulle-container
library. Read this file first to determine which documentation files contain
the information you need.

**Quick decision guide:**
- Need to understand the overall design? → `architecture.md`
- Need to know what data structures exist? → `components.md`
- Need function signatures or API details? → `interfaces.md` or the `dox/API_*.md` files
- Need to understand struct layouts? → `data_models.md`
- Need to know how to build/test/iterate? → `workflows.md`
- Need dependency info? → `dependencies.md`
- Need the full AI-oriented reference? → `TOC.md` (pre-existing, comprehensive)

## File Descriptions

| File | Purpose | When to Consult |
|------|---------|-----------------|
| `index.md` | This file — documentation map and AI guidance | Always read first |
| `architecture.md` | System architecture, layering pattern, design principles | Understanding how components relate, why double-underscore vs single-underscore |
| `components.md` | Catalog of all data structures with responsibilities | Choosing the right container, understanding what exists |
| `interfaces.md` | Public API surface, function naming conventions, verb system | Writing code that uses the library, finding the right function |
| `data_models.md` | Struct definitions, callback types, memory layouts | Understanding internal representation, writing callbacks |
| `workflows.md` | Build, test, development workflows using mulle-sde | Building, testing, adding files, CI |
| `dependencies.md` | External dependencies and their roles | Understanding what mulle-allocator and mulle-data provide |
| `review_notes.md` | Documentation gaps and inconsistencies | Improving documentation coverage |
| `TOC.md` | Pre-existing comprehensive AI reference with examples | Detailed API usage, code examples, performance characteristics |

## Cross-References

- `architecture.md` references `components.md` for specific data structure details
- `interfaces.md` references `data_models.md` for callback struct definitions
- `workflows.md` references `dependencies.md` for build dependency context
- All files reference the naming convention system documented in `interfaces.md`

## Existing API Documentation (in `dox/`)

These pre-existing files provide detailed API docs for specific components:

| File | Covers |
|------|--------|
| `dox/API_ARRAY.md` | `mulle_array` API (350 lines) |
| `dox/API_CONTAINER_CALLBACK.md` | Callback system (335 lines) |
| `dox/API_HASH.md` | Hash functions (64 lines) |
| `dox/API_MAP.md` | `mulle_map` API (129 lines) |
| `dox/API_POINTERARRAY.md` | `mulle_pointerarray` API (89 lines) |
| `dox/API_POINTERPAIRARRAY.md` | `mulle_pointerpairarray` API (99 lines) |
| `dox/API_POINTERSET.md` | `mulle_pointerset` API (27 lines) |
| `dox/API_SET.md` | `mulle_set` API (59 lines) |
| `dox/VERBS.md` | Function verb naming system (77 lines) |
| `dox/DATATYPES.md` | Integral type conventions (31 lines) |
