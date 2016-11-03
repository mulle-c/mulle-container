# `mulle_pointerset`

A really simple kind of set that only does pointer equality checking. 
It only grows and uses bsearch for lookup. This can be useful for very small sets ( < 100 entries).

## Types

```
struct mulle_pointerset
```

## Functions

Function names are `mulle_pointerset_<verb>`. So for example: `done`  would be `mulle_pointerset_done`.

### Creation / Destruction Verbs

* `init` - takes an allocator as a parameter
* `done`

### Operation Verbs

* `insert`
* `get`
       


