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
`unsigned int `    | counts or indices into the data structure
`size_t`           | byte quantity values
`uintptr_t`        | hashes, search results (and other functions returning
                   | `mulle_not_found`)