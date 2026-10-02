# The CMake build system, as a graph

The `cmake/` tree is a small dataflow system: each file declares variables,
includes other files in a fixed order, and hands its variables on to whoever
comes next. `tools/cmake-graph.py` parses that structure and renders it with
graphviz, so the wiring can be read instead of guessed.

## Artifacts

| File | What it is |
|------|------------|
| `pix/cmake-order.svg` | include() order only, compact enough for one screen |
| `pix/cmake-commands.svg` | flat, pre-processed list of the cmake commands each file creates |
| `pix/cmake-state.svg` | only the writes that outlive the file making them, by blast radius |
| `pix/cmake.svg` | the value-flow map (large: pan/zoom or print) |
| `pix/cmake*.dot` | the graphviz sources |

`pix/Makefile` already renders every `*.dot` to `*.svg`, so once the `.dot`
files exist, `make -C pix cmake.svg` (or plain `make -C pix`) is enough.

## Regenerating

```sh
tools/cmake-graph.py --render          # all four graphs, into pix/
```

Only the standard library is needed; graphviz (`dot`) is needed for `--render`.

Useful slices:

```sh
tools/cmake-graph.py --focus Library            # Library.cmake and 1 include hop
tools/cmake-graph.py --focus Files --depth 2    # wider neighbourhood
tools/cmake-graph.py --all                      # also toolchains, executive files, ...
tools/cmake-graph.py --layout value             # rank by value flow, not by include()
tools/cmake-graph.py --mode commands            # only the flat commands view
tools/cmake-graph.py --mode state               # only the global-state view
tools/cmake-graph.py --mode state --show-private  # ... including PRIVATE target flags
tools/cmake-graph.py --no-hooks                 # hide the *Aux extension hooks
tools/cmake-graph.py --show-hidden              # also TMP_*/loop vars, guards
tools/cmake-graph.py --max-names 3              # shorter rows
```

To read the same tree as one flat file instead of a graph - the include order
already resolved - see `cmake_flat_expansion.md`.

## How to read the flow map

Every `.cmake` file is one node, drawn as a table that reads top to bottom:

```
+-------------------------------------------------------+
| Library.cmake                       <- include() lands |
| in  · phase:   LINK_PHASE                              |  values arriving
| in  · deps:    DEPENDENCY_LIBRARIES, ...               |
| includes:  1 StringCase  2 PreLibrary?  3 InstallRpath? |  include order
| out · targets: LIBRARY_COMPILE_TARGET, ...             |  values leaving
| out · identity: LIBRARY_NAME                           |
+-------------------------------------------------------+
```

* **Rows are colored by value group** - every path-ish value shares the `paths`
  color, every phase flag the `phase` color, and so on. The `legend` node in
  the top left is the key.
* **The node header is colored by folder**, so it is obvious at a glance which
  files you may edit. See the table below.
* **Vertical position is include depth.** `CMakeLists.txt` is at the top, the
  files it pulls in below it, their children below that.
* **Dashed grey edges are `include()`**, labeled with the position of the call
  in the including file. A `?` means `include( X OPTIONAL)`, i.e. the file may
  not exist (all the `*Aux` extension hooks).
* **Solid colored edges are hand-offs of a value** from the `out` row of the
  file that sets it to the `in` row of every file that reads it. The color is
  the group; the label names the variables.
* **`name*`** is read here but never set anywhere in the tree - it comes from
  the command line, the environment, or cmake itself. The `outside world` node
  at the top lists them all.
* **Dashed white boxes** are the `*Aux`/`*AuxC` extension hooks. They are not
  in this repository; a project drops `cmake/<Name>.cmake` next to
  `CMakeLists.txt` to take part in that step.

## The commands view (what the config step actually does)

`pix/cmake-commands.svg` answers a different question from the other two: not
"who feeds whom" but "what does running cmake actually create". The include tree
is **flattened** into the order cmake processes it, and every effectful command
is listed beside the file that issues it:

```
 3  cmake/share/Environment.cmake   --  find_program(MULLE_SDE)
                                   --  execute_process(probe)
 ...
12  cmake/share/Library.cmake      --  add_library(${LIBRARY_COMPILE_TARGET} OBJECT)
                                   --  add_library(${LIBRARY_NAME})          +1 if LINK_PHASE
                                   --  install(TARGETS)  (from InstallLibrary)
```

* **Left column: every file, numbered in the order cmake reaches it.** This is
  the flattening - a depth-first walk from `CMakeLists.txt` following the
  `include()` calls, which is exactly what cmake does (each file runs once, the
  `__X__CMAKE__` guards see to that).
* **Right column: one node per command**, collapsed by command *and subject*,
  so `install(FILES a)` and `install(FILES b)` are both `install(FILES)`, while
  `add_library(${LIBRARY_COMPILE_TARGET} OBJECT)` and `add_library(${LIBRARY_NAME})`
  stay separate. Arrow color = command family.
* **The edge label is the guard.** `+1 if LINK_PHASE` means this call sits one
  `if()` deep and that `if()` is `LINK_PHASE`. `∀ x` means it is inside
  `foreach(x ...)`, so it runs once per element. `×N` means the file makes the
  call N times. Hovering an edge gives the full guard chain and the line number.
* **Dashed command nodes are built by a `function()`/`macro()` body** - they only
  run when something calls that helper, so they are not immediate effects.

This is where a reading like "`add_library(${LIBRARY_NAME})` always runs" gets
corrected: it is inside `if( LIBRARY_SOURCES OR OTHER_...)`, and the real
`add_library(${LIBRARY_NAME} INTERFACE)` on the header-only path is a different
node.

## The state view (what the config changes outside itself)

`pix/cmake-state.svg` answers the third question: not "who feeds whom" and not
"what does it build", but **what does running this config change that outlives
the file making the change**. It is the view to open before wondering whether
this library, dropped into somebody else's project, will move their search
paths or their flags.

Every write is bucketed by **blast radius**, and the buckets are the four colour
groups in the picture:

| Bucket | Means | Typical writers here |
|--------|-------|----------------------|
| `PROCESS / DIRECTORY` | visible to every target configured *after* this point, and to subdirectories | `include_directories`, `add_definitions`, `set( CMAKE_*_FLAGS)`, `set( CMAKE_LIBRARY_PATH)`, `set( CMAKE_INSTALL_RPATH)`, `set_property( GLOBAL ...)`, `option()`-adjacent `set( CMAKE_MODULE_PATH)` |
| `CACHE` | survives a reconfigure, shows up in `ccmake`, and is inherited by a project that embeds this one | the 16 `option()` calls, `set( ... CACHE BOOL|STRING|FILEPATH)`, plus `CACHE INTERNAL` memoisation |
| `EXPORTED` | what a target that links the library compiles and links against | `target_link_libraries( ... PUBLIC)`, `target_include_directories( ... INTERFACE)`, `target_compile_definitions( ... PUBLIC)`, `set_target_properties( ... INTERFACE_*)` |
| `ON DISK` | the files and the cmake package written into the install tree | `install( TARGETS|FILES|DIRECTORY)`, `configure_file`, `write_basic_package_version_file`, `export()` |

Rules the picture follows:

* Only writes that **leave** the file are drawn. `PRIVATE` target flags are
  hidden by default (`--show-private` brings them back) because they change
  nothing outside this build.
* Commands that only **read** are not state: `find_library`, `find_program`,
  `execute_process`. They appear in the commands view, where shelling out to
  `mulle-sde` during configure is visible.
* A channel is one *piece* of state, not one variable - which is why there is a
  single `link / library search path` node even though several `CMAKE_*_PATH`
  variables feed it, and why the values listed in the node are the interesting
  part rather than its title.
* `PARENT_SCOPE` writes land in `PROCESS / DIRECTORY` with the scope spelled
  out, because they escape the file by definition - inside a `function()` they
  land in the caller's scope, at file scope in the enclosing directory, which
  for a top level `CMakeLists.txt` is whoever included *this* project. That is
  the channel that matters for the embedded case.
* Edges are labeled like the commands view: command name, `×N` when the file
  writes the same thing N times, and the innermost guard; the tooltip has the
  line number and the full guard chain. A dashed edge means the write happens
  inside a `function()`/`macro()` body, i.e. only when something calls it.
* **Values are wrapped, not truncated.** A long value continues on the next
  line and the `file:line` annotation trails the last one. Nothing in the
  commands or state graphs is cut off with `…`; `--max-names` (default 6) caps
  how many values a channel lists, marking the remainder `+N more (hover)`, and
  the tooltip always carries the complete text.

Two readings this view makes trivial, and which are worth checking whenever the
`cmake/` tree is touched:

* **Does it move global search paths?** Yes, in four places:
  `CMAKE_MODULE_PATH` (the `list( INSERT ...)` calls in `CMakeLists.txt`),
  `CMAKE_INCLUDE_PATH`/`CMAKE_FRAMEWORK_PATH` and `CMAKE_LIBRARY_PATH`
  (`Environment.cmake`), plus three `include_directories()` calls that add
  include dirs for *every* target configured afterwards.
* **How does a consumer link the library?** The `EXPORTED` column is the
  answer: `INTERFACE_INCLUDE_DIRECTORIES` (with `$<BUILD_INTERFACE:>` /
  `$<INSTALL_INTERFACE:>` split), `INTERFACE_LINK_LIBRARIES`,
  `INTERFACE_COMPILE_OPTIONS` and `INTERFACE_LINK_OPTIONS`.

## Where a file lives

All graphs color by origin, because the four locations have very different
rules attached to them.

| Color | Origin | Rule |
|-------|--------|------|
| gold | `CMakeLists.txt` | the entry point; the only file whose include order matters most |
| green | `cmake/` | yours - copy a file up from `cmake/share/` to override it |
| blue | `cmake/share/` | managed by mulle-sde, rewritten by `mulle-sde upgrade` |
| purple | `cmake/reflect/` | generated by `mulle-sde reflect`, never edit |

In the flow map it is the header (title) row of each node; in the include-order
graph it is the node fill. `*Aux` hooks are the dashed white boxes - they would
live in `cmake/` or the project root if the project defined them.

## The value groups

| Group | Covers |
|-------|--------|
| `phase` | `HEADER(S)_PHASE`, `COMPILE_PHASE`, `LINK_PHASE`, `MULLE_MAKE_PHASE` |
| `identity` | `PROJECT_*`, `LIBRARY_*IDENTIFIER`, `*_NAME`, `*_VERSION` |
| `build` | `BUILD_SHARED_LIBS`, `CMAKE_BUILD_TYPE`, `STANDALONE`, `MULLE_TEST`, ... |
| `install` | `INSTALL_*`, `ASSET_DIR`, the config/header templates |
| `files` | `SOURCES`, `*_HEADERS`, `INCLUDE_DIRS`, `PROJECT_FILES`, ... |
| `deps` | `DEPENDENCY_*`, `ALL_LOAD_*`, `OS_SPECIFIC_*`, `INHERITED_*`, ... |
| `compiler` | `*_FLAGS`, `*_WARNINGS`, `*_COMPILER_ID`, `MULLE_LANGUAGE` |
| `targets` | `LIBRARY_COMPILE_TARGET`, `LIBRARY_LINK_TARGET`, `INSTALL_LIBRARY_TARGETS` |
| `paths` | `*_DIR`, `*_PATH`, `MULLE_SDK_*`, `MULLE_VIRTUAL_ROOT` |
| `cmake` | cmake builtins (`CMAKE_*`) |
| `platform` | `APPLE`, `WIN32`, `MSVC`, `UNIX`, ... |
| `env` | `$ENV{...}` references |
| `function` | functions/macros and their call sites |
| `other` | everything that no rule claimed |

Hidden unless `--show-hidden`: `internal` (`TMP_*`, loop and helper variables),
`guard` (`__X__CMAKE__` include guards) and `debug` (`MULLE_TRACE_INCLUDE`).

## What the parser actually does

It is a regex pass over comment-stripped source, not a cmake interpreter, so
treat the picture as an accurate sketch rather than ground truth. It reads:

* `include( X [OPTIONAL])` and the order of the calls
* `set` / `unset` / `option`, `list(APPEND|...)`, `list(LENGTH|GET)`,
  `foreach` loop variables, `execute_process(... OUTPUT_VARIABLE X)`
* `function` / `macro` definitions and their call sites
* `project( name ...)` as producing `PROJECT_NAME` / `PROJECT_VERSION*`
* `snakeCaseString( in OUT)` and `ResolveFileSymlinksIfNeeded( in OUT)` -
  these fill the variable named by their **last argument**, a mulle convention
  that otherwise looks like a read
* `${X}`, `$CACHE{X}`, `$ENV{X}`, and bare identifiers inside `if`/`elseif`/
  `while` conditionsWhat it does not model: computed names such as `set( INSTALL_${ID}_RESOURCES)`
(ignored on purpose), `string()` outputs, generator expressions, and the exact
value of a branch condition. A variable that is only ever written inside a
branch is reported as written, not as conditionally written.

The commands view is restricted to a fixed allowlist of build-affecting
commands (`add_library`, `install`, `target_*`, `set_target_properties`,
`find_library`, ...). Project-local helper macros and plain bookkeeping like
`set()`/`list()`/`message()` are not effects and are left out deliberately.

Being a sketch is still useful: it is how the `__FILES__CMAKE__` /
`__FILES___CMAKE__` mismatch at the top of `cmake/share/Files.cmake` shows up
as a `name*` on the `Files` node - the guard it tests is never set there.
