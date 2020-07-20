# `mulle_uniquepointerarray`

A really simple kind of set that only does pointer equality checking.
It only grows and uses bsearch for lookup. This can be useful for very small sets ( < 100 entries).

## Types

```
struct mulle_uniquepointerarray
```

## Functions

Function names are `mulle_uniquepointerarray_<verb>`. So for example: `done`  would be `mulle_uniquepointerarray_done`.

### Creation / Destruction Verbs

* `init` - takes an allocator as a parameter
* `done`

### Operation Verbs

* `insert`
* `get`



