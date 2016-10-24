# Verbs


## Initialization and Destruction

Action | Opposite
-------|----------
alloc  | free     
create | destroy  
init   | done     


Action | Meaning
-------|---------------
alloc  | allocate memory from heap
done   | tear down struct on stack or heap
free   | deallocate memory to heap
init   | setup struct on stack or heap


Action  | Decomposition
--------|---------------
create  | alloc + init     
destroy | done + free  
reset   | done + init   


## Access


Action    | Meaning
----------|---------------
add       | sequential access write, into empty space
count     | get number of elements (non char or byte)
enumerate | create enumerator
extract   | destructive random access, with ownership transfer (caller must free)
find      | a search that is linear, returns an index or key
get       | random access read
give      | random access write, with ownership transfer (caller must free)
insert    | random access write, not overwriting at specific space
length    | get size of character or byte data (always in char units)
next      | sequential access read
remove    | random access remove
search    | a search that's not linear, returns the value
set       | random access write 
write     | either set or insert, depending on parameter