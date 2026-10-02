# The flat expansion of the cmake tree

`tools/cmake-graph.py` draws the tree. `tools/cmake-flatten.py` *executes* the
part of it that can be executed on paper: it starts at `CMakeLists.txt`, follows
every `include()` below `cmake/`, and splices each file's body in at the point
of its call. The result is one readable file in which the include order is
already resolved.

cmake runs `include()` in the *current* scope, so a depth-first splice
reproduces the order of effects exactly - that is what makes this a compiler
rather than a pretty dump.

## Artifact

| File | What it is |
|------|------------|
| `pix/cmake-flat.cmake` | 33 files, 2758 lines: the whole tree with `include()` resolved |

## Regenerating

```sh
tools/cmake-flatten.py                    # CMakeLists.txt -> pix/cmake-flat.cmake
tools/cmake-flatten.py --verify           # ... and prove nothing was lost
tools/cmake-flatten.py --stdout           # pipe it instead of writing a file
tools/cmake-flatten.py --entry Library    # expand one module rather than the root
tools/cmake-flatten.py --entry Library --out /tmp/library.cmake
```

`--entry` takes `CMakeLists.txt`, a module name (`Library`), or a path
(`cmake/share/Library.cmake`). `make -C pix flat` does the same as the first
line plus `--verify`.

The module search order is read out of the `list( INSERT CMAKE_MODULE_PATH 0 ...)`
lines in `CMakeLists.txt` and reversed, because each insert goes to the front -
this tree really searches `cmake`, then `cmake/reflect`, then `cmake/share`.

## How to read it

Four markers, all at column 0 so they grep:

| Marker | Means |
|--------|-------|
| `# >> INLINE  <file>  <- <site>` | the body of `<file>` starts here. `<site>` is the `include()` call that pulled it in, as `file:line`, plus the `if()` it sits under if it was conditionally included |
| `# << END     <file>` | that body ends |
| `# -- KEEP    include( X)  [<reason>]` | not ours: a project hook this repo does not define, or a module cmake supplies. **The call is kept verbatim**, so the flat file still fires the hook |
| `# -- SKIP    include( X)  [already expanded, guard <G>]` | dropped, because `X` was already spliced in and its `<G>` guard makes a second include a no-op |

Everything between the markers is the original source text, byte for byte -
indentation, comments and all. Nothing is re-indented, so a diff against a real
file is meaningful.

Reading the whole thing means reading the build in the order cmake runs it:
`Environment` -> `Files` -> (`Headers`, `Sources`, `Resources`) ->
`Dependencies` -> `Library` -> `InstallLibrary` -> `InstallCMakePackage` ->
`FinalOutput`.

## What it shows for this tree

- **Of the 54 `include()` calls in the 33 files `CMakeLists.txt` can reach, 33
  expand, 3 are no-ops and 18 stay calls.** (The tree has 80 calls in total; the
  other 26 are in files the root never reaches - `Framework`, `Executable`,
  toolchains - and `--entry` is how you look at those.)
  `StringCase` is included from four places, and only the first does anything -
  which is the only reason `snakeCaseString` is defined before its second,
  third and fourth supposed definition.
- **The one warning is a real bug.** `cmake/share/Files.cmake:6` tests
  `__FILES__CMAKE__` but line 7 sets `__FILES___CMAKE__` (three underscores).
  The guard never latches, so the file is not protected against a second
  include - the tool expands it again rather than pretending otherwise, and
  says so. This is the same defect as item 1 in the upstream bug list.
- **Conditional includes are labelled rather than silently taken.**
  `include( Dependencies OPTIONAL)` is marked `under if( LINK_PHASE)`,
  `include( InstallCMakePackage)` likewise, and `cmake/reflect/_Dependencies.cmake`
  is marked as sitting under
  `if( IS_DIRECTORY "${DEPENDENCY_DIR}" OR NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)` -
  the mode-detection condition that the usage-modes report is about.
- **17 of the 18 kept calls are extension points this project leaves empty**
  (`PreLibrary`, `PreLibraryAuxC`, `AllLoadAuxC`, `PostInstallLibrary`, ...).
  That list is the project's hook surface, enumerated.

## What it cannot model

| Thing | What happens |
|-------|--------------|
| `if()` / `foreach()` conditions | **Not evaluated.** Conditions the tool can see are printed on the banner, but the branch is expanded as if taken, so the flat file is the union of all branches |
| `CMAKE_CURRENT_LIST_DIR`, `CMAKE_CURRENT_LIST_FILE` | Now refer to the flat file, not to the file the line came from. In this tree they are only used in `MULLE_TRACE_INCLUDE` messages |
| `return()` | In an included file it returns to the includer; in the flat file it returns out of everything. Reported as a warning where it occurs |
| computed include names | `include(${X})` cannot be resolved. There are none in this tree, which is why the expansion is complete |

## Why the result can be trusted

`--verify` rescans the flat file with `cmake-graph.py`'s own command scanner and
compares it against the sources, as a **multiset** of `(command, arguments)` -
so a call that genuinely runs twice must appear twice:

```
verify: 76 effectful commands in the 33 expanded sources, 76 in the flat file
   ok: identical (command, arguments) multisets
   blocks: 244 open / 244 close ok
```

The block count catches a bad splice (`if(`/`endif()` pairs have to survive).
Note the column: a *set* difference would be the wrong test here, and a naive
block counter reports a false mismatch because `elseif(` replaces the innermost
`if` instead of opening a block.

This is structural verification, not execution: the flat file has not been run
through cmake itself, which would need the project's dependency tree.

## Related

- `cmake_build_graph.md` - the same tree drawn as four graphs
- `index.md` - the documentation map

The guard typo above is item 1 of the "silent bugs" list in the repo root's
`mulle-sde-cmake-findings.md`, which is gitignored scratch and not part of these
docs.
