alloc - free
init  - done
create - destroy

create   ->  alloc + init     (heap)
init     ->  setup struct     (stack)
done     ->  tear down struct (stack)
reset    ->  done + init
destroy  ->  done + free      (heap)


get      -> random access read
set      -> random access write 
remove   -> random access remove

give     -> random access write, with ownership transfer
extract  -> destructive random access, with ownership transfer

insert   -> random access write, not overwriting at specific space
write    -> either set or insert, depending on parameter

add      -> sequential access write, into empty space
next     -> sequential access read


find     -> a search that is linear, returns an index or key
search   -> a search that's not linear, returns the value

count    -> get number of elements (non char or byte)
length   -> get size of character or byte data (always in char units)

enumerate -> create enumerator
