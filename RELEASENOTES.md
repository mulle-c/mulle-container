### 10.1.3







* all allocations now use overflow-checked ``mulle_allocator_size_multiply`/`size_add`;` growth overflow calls ``mulle_allocation_fail`` instead of `abort`
* structarray elements are now properly aligned (correct round-up of ``sizeof_struct`)` and static-storage sentinels are computed with the right stride
* pointermap `insert`/`register` no longer misreport duplicate inserts (or drop the new value) when the existing value is NULL
* rangeset greater-than searches no longer overflow at ``mulle_range_location_max``
* rangemap handles empty maps safely: no NULL-pointer arithmetic in ``get_ranges_values`,` ``get_exact`` (sets errno), or enumeration
* pointerpair pointer/intptr key compares return normalized -1/0/1 instead of a raw (and potentially overflowing) difference
* ``mulle_array_member`` is now NULL-safe; array range removals clamp out-of-bounds ranges instead of overrunning
* pointerset enumerator mutation detection now also covers remove and shrink




* added BSD 3-clause license headers to all source files missing them
* documented MSVC C1001 optimizer crashes on memcpy() calls with /O2

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
