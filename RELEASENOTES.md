### 10.1.3 (unreleased)

Fixes:

- `_mulle__rangeset_shift` no longer crashes when a shift empties the set
  (NULL-deref guard). Regression test: `test/rangeset/30_shift/shift-to-empty.c`
- `_mulle__rangeset_shrink_to_fit` now actually shrinks (inverted condition).
  Regression test: `test/rangeset/60_operations/shrink-to-fit.c`
- `mulle_pointerpair_qsort_r` dispatches to the correct comparator fast
  path (pointer vs. string key were swapped)
- `mulle_structarray_init_default` macro signature changed from
  `( type)` to `( array, type)` — the container argument is now passed
  explicitly

### 10.1.2

Various small improvements
