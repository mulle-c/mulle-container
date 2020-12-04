# Verbs

Generally verbs act on the first paramater of the function call.
(e.g. mulle_map_get( map)). If that is not the case the verb is
prefixed with the scope of the get. That could be "threadget" for
example for accessing threadlocal storage.


## Initialization and Destruction

Verb    | Meaning
--------|---------------
alloc   | allocate memory from heap
done    | tear down struct on stack or heap
free    | deallocate memory to heap
init    | setup struct on stack or heap
make    | construct on the fly (e.g. return a struct by value)
copy    | create + copy_items

Verb    | Decomposition
--------|---------------
create  | alloc + init
destroy | done + free
reset   | remove all, keep allocation though

Verb    | Opposite
--------|----------
alloc   | free
create  | destroy
init    | done
copy    | destroy

## Verb modifier

Verb        | Meaning
------------|----------
length      | a quantity in bytes
count       | a quantity of something other than bytes (usually pointers)
size        | the current maximum quantity
size_as_length | as above but in bytes
used        | the amount of the maximum quantity in use
used_as_length | as above but in bytes

## Field Accessors

Verb          | Meaning
--------------|---------------
get_count     | get number of elements
get_length    | get size of character or byte data (always in char units)
get_allocator | return the mulle-allocator associated with the struct
get_callback  | return the callback associated with the struct
get_notakey   | return the value that is used to indicate that there is no element (usually NULL)


## Memory management

guarantee   | Ensure that there is space for 'n' elements without realloc, if that space will be requested
advance     | Reserve space for 'n' elements, returning a pointer to the first space
reserve     | Reserve space for 1 element, returning a pointer to it

## Element Access Read

Verb        | Meaning
------------|---------------
count       | get number of elements (non char or byte)
get         | random access read
next        | sequential access read
member      | returns 1 (yes) or 0 (no) depending on presence in container


## Element Access Write

Verb        | Meaning
------------|---------------
add         | sequential access write, into empty space
extract     | destructive get, with ownership transfer (caller must free)
insert      | random access write, not overwriting
member      | returns 1 (yes) or 0 (no) depending on presence in container
register    | a get or set operation, returns the previous value , with ownership transfer (caller must free)
set         | a destructive insert (may or may not return previous value)


## Operations

Verb        | Meaning
------------|---------------
enumerate   | create enumerator
find        | a search that is linear, returns an index (mulle_not_found_e) or key
search      | a search that's not linear, probably a binary search, returns the value
copy_items  | copy each element from source to destination struct [ dst, src]



