# Verbs

## Initialization and Destruction

Verb   | Meaning
-------|---------------
alloc  | allocate memory from heap
done   | tear down struct on stack or heap
free   | deallocate memory to heap
init   | setup struct on stack or heap
copy   | create + copy_items

Verb    | Decomposition
--------|---------------
create  | alloc + init     
destroy | done + free  
reset   | done + init   

Verb   | Opposite
-------|----------
alloc  | free     
create | destroy  
init   | done     
copy   | destroy


## Field Accessors

Verb          | Meaning
--------------|---------------
get_count     | get number of elements
get_length    | get size of character or byte data (always in char units)
get_allocator | return the mulle-allocator associated with the struct
get_callback  | return the callback associated with the struct
get_notakey   | return the value that is used to indicate that there is no element (usually NULL)


## Element Access Read 

Verb      | Meaning
----------|---------------
count     | get number of elements (non char or byte)
get       | random access read
next      | sequential access read
member    | returns 1 (yes) or 0 (no) depending on presence in container

## Element Access Write 

Verb      | Meaning
----------|---------------
add       | sequential access write, into empty space
extract   | destructive get, with ownership transfer (caller must free)
insert    | random access write, not overwriting 
member    | returns 1 (yes) or 0 (no) depending on presence in container


## Operations

Verb       | Meaning
-----------|---------------
enumerate  | create enumerator
find       | a search that is linear, returns an index or key
search     | a search that's not linear, returns the value
copy_items | copy each element from source to destination struct [ dst, src]
