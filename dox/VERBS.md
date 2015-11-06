create   ->  alloc + init
init     ->  setup struct
done     ->  tear down struct
reset    ->  done + init
free     ->  done + dealloc


set      -> either insert or put (depends on mode)

insert   -> random access write into empty space
put      -> random access write into possibly occupied space
add      -> sequential write into empty space

remove   -> random access remove

get      -> random access read
next     -> sequential read

extract  -> destructive random access with ownership transfer

search   -> a search that's not linear, returns the value
find     -> a search that is linear, returns an index or key

count    -> get number of elements (non char or byte)
length   -> get size of character or byte data (always in char units)

enumerate -> create enumerator
