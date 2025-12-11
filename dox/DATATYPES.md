## Integral Types

Use of integral types is modelled after how one uses them in a malloced
C array:

```
unsigned int   i;
long           *array;

array   = malloc( sizeof( long) * n);  // size_t
for( i = 0; i < n; i++)                // unsigned int
   array[ i] = i;
```

Type               | Usage
-------------------|------------------------------------------------------
`unsigned int`     | counts or indices into the data structure
`size_t`           | byte quantity values
`uintptr_t`        | hashes, search results (and other functions returning
                   | `mulle_not_found`)

MEMO: so usually get_count should return `unsigned int` unless the
data structure holds bytes size info, then use `size_`. For ... reasons
`unsigned int` the count is more often than not also `size_t`...

## Struct datatypes

Usually struct fields are accessed via inline functions. The actual fields
that are prefixed with a `_` are not public and therefore subject to change
within a major version.

