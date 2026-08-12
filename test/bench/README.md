# Benchmarks

This directory contains two kinds of benchmarks for every container type
in mulle-container.

## Deterministic benchmarks (golden `.stdout` files)

Files ending in `-bench.c` measure *deterministic* properties with a
counting allocator (`bench.h`):

- how many `calloc` / `realloc` / `free` calls a workload performs
- how the storage grows (capacity, bucket count, range count)
- how many comparisons a sorted structure makes (assoc, pointerpairarray)

Because the counting allocator hooks into `mulle_allocator_default` via
the `aba` slot, the numbers reflect exactly what the containers ask the
allocator to do. The workloads use only fixed keys (never addresses), so
the output is reproducible across runs, architectures and platforms. That
makes these tests suitable for golden-file comparison in CI: a change to
the growth policy, the bucket sizing or the sort algorithm will show up
as a diff.

### Included

| File                    | Measures                                   |
|-------------------------|--------------------------------------------|
| `pointerarray-bench.c`  | growth, preallocation, range remove/insert |
| `array-bench.c`         | growth, preallocation, linear member scan  |
| `structarray-bench.c`   | growth vs. reserve                         |
| `map-bench.c`           | bucket growth, lookup hits, remove         |
| `pointermap-bench.c`    | bucket growth, lookup hits                 |
| `set-bench.c`           | bucket growth, membership, remove          |
| `pointerset-bench.c`    | bucket growth, membership                  |
| `assoc-bench.c`         | comparisons for sorted/reverse/shuffled    |
| `pointerpairarray-bench.c` | qsort comparisons, sortedness            |
| `pointerqueue-bench.c`  | bucket chain allocations, add/pop          |
| `structqueue-bench.c`   | bucket chain allocations, add/pop          |
| `rangeset-bench.c`      | coalescing, range counts, remove           |
| `rangemap-bench.c`      | range counts, search, remove               |
| `head-to-head-bench.c`  | vs. raw C array / khash-style hash (ratios) |
| `crossover-bench.c`     | lookup cost vs. size (bsearch/array/assoc/set/map) |
| `timing-bench.c`        | wall-clock ns/op for core operations        |

## Head-to-head benchmark (no golden file)

`head-to-head-bench.c` answers the question *"how much slower is
mulle-container than just writing it in C myself?"* It measures each
container against a plain-C baseline on the same machine, same data and
same hash function (`mulle_pointer_hash`):

| mulle-container        | baseline                          |
|------------------------|-----------------------------------|
| `mulle_pointerarray`   | raw `void *[]` (no checks, `realloc` growth) |
| `mulle_map`            | khash-style open addressing (linear probing, 75 % load) |
| `mulle_set`            | same khash-style table            |

It prints ns/op for both sides plus the **ratio**, so "fast" and "slow"
mean something: a ratio of 1.0 is exactly hand-rolled C, above 1.0 is the
price of bounds checks / NULL-safety / callbacks, below 1.0 meansthe container's tuning beats the naive baseline. Representative release-mode
ratios (n=100000, ballpark): pointerarray add ~0.7-1.0x raw array, get
~1.0x unchecked index (the NULL-safe getter is static inline - the
compiler folds the NULL guard away when the container is used in the same
function, leaving a single load); map set 0.85-1.2x khash, map get
~1.2-2.0x; set set 0.6-0.9x, member 1.0-1.3x. Same release-build workflow
as the timing bench; N is argv[1].

> Note: the measured loops are deliberately made observable (a checksum is
> printed at the end), otherwise the compiler deletes dead-result loops or
> - in an unoptimized build - fails to inline the header getters, which
> inflates the ratio of NULL-safe accessors like `mulle_pointerarray_get`
> to an unchecked index (a known artifact of the earlier ~5-6x figure).

## Crossover benchmark (no golden file)

`crossover-bench.c` answers the question the README's performance claims
raise: *"when does a hash set beat a sorted array / a linear scan?"* It
measures membership/lookup cost in ns/op for four containers (raw C
`bsearch` as baseline, `mulle_array_member` linear scan, `mulle_assoc_get`
binary search, `mulle_set_member` and `mulle_map_get` hash) at n = 10,
100, 1000, 10000, 100000. Same release-build workflow as the timing
bench; optionally pass a starting size as argv[1].

Representative release-mode numbers (wine-emulated, ballpark):

```
n=      10: bsearch     21.3 | array     14.7 | assoc     11.7 | set     11.8 | map      9.2 ns/op
n=     100: bsearch     19.6 | array     86.8 | assoc     18.7 | set     11.8 | map     10.4 ns/op
n=    1000: bsearch     37.3 | array    759.9 | assoc     40.4 | set     13.6 | map     11.9 ns/op
n=   10000: bsearch     55.7 | array   7469.2 | assoc     54.7 | set     17.1 | map     15.8 ns/op
n=  100000: bsearch     68.3 | array  74167.2 | assoc     68.6 | set     20.7 | map     19.6 ns/op
```

Reading: hash lookups stay flat (~9-19 ns) while the linear scan scales
linearly (13 ns → 73 µs) and the binary searches grow logarithmically
(12 → 72 ns). `mulle_set`/`mulle_map` are competitive with the array scan
already at n=10 and clearly win from n=100 up — the README's "~100 items"
crossover rule is verified and is, if anything, conservative: in the
measured range (n >= 10) the linear scan never wins.

## Timing benchmark (no golden file)

`timing-bench.c` prints wall-clock ns/op numbers. It has **no** `.stdout`
file, so the test harness runs it as a smoke test but does not compare
the output (timing is inherently noisy and would break CI). Run it
directly to see the numbers:

``` sh
mulle-sde test run test/bench/timing-bench.c
```

or, after `mulle-sde test craft`:

``` sh
./test/bench/timing-bench.exe
```

**Use a release build for meaningful numbers.** The default debug test
build asserts inside `mulle_assoc_add` (linear membership scan) and
`_mulle__rangeset_insert` (O(n) internal assert per insert), which makes
those two benchmarks quadratic and utterly misleading. Craft the release
test build and run the executable directly (N is the first argument):

``` sh
mulle-sde test --release craft
./test/bench/timing-bench.exe 100000
```

Note that `mulle-sde test --release run` can still show debug-library
numbers in mixed kitchen environments (the test runner may resolve the
Debug DLLs on its library path) — the direct run of the freshly crafted
executable is the trustworthy one. The default N is small (10000) so the
debug smoke test stays fast. Treat all figures as ballpark numbers, not
as a microbenchmark-grade measurement.

## Adding a benchmark

Copy the shape of an existing `-bench.c` file:

1. `bench_allocator_init()` gives you a counting allocator.
2. Hand `&bench.allocator` to the container.
3. Print the counters and the observable state (count, size, rangecount).
4. Create the matching `.stdout` golden file:

``` sh
./your-bench.exe > your-bench.stdout
```

Verify it is stable by running it a few times.
