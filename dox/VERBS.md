# Verbs

## Initialization and Destruction

Action | Opposite
-------|----------
alloc  | free     
create | destroy  
init   | done     
copy   | destroy


Action | Meaning
-------|---------------
alloc  | allocate memory from heap
done   | tear down struct on stack or heap
free   | deallocate memory to heap
init   | setup struct on stack or heap
copy   | create + copy_items

Action  | Decomposition
--------|---------------
create  | alloc + init     
destroy | done + free  
reset   | done + init   


## Field Accessors

get_count     | get number of elements
get_length    | get size of character or byte data (always in char units)
get_allocator | return the mulle-allocator associated with the struct
get_callback  | return the callback associated with the struct


## Element Access Read or Write

Action    | Meaning
----------|---------------
add       | sequential access write, into empty space
count     | get number of elements (non char or byte)
extract   | destructive get, with ownership transfer (caller must free)
get       | random access read
insert    | random access write, not overwriting 
next      | sequential access read
remove    | random access remove
set       | random access write, will overwrite 
member    | returns 1 (yes) or 0 (no) depending on presence in container


## Operations

enumerate  | create enumerator
find       | a search that is linear, returns an index or key
search     | a search that's not linear, returns the value
copy_items | copy each element from source to destination struct [ dst, src]
