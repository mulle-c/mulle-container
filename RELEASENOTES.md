# 7.0.0

* remove package.json as it conflicts with clib.json
* the various "empty" structs are now just defines, which produces better code
* `mulle__rangeset` can now be intialized with static storage
* `_mulle__rangeset_get` added
* **BREAKING** `_mulle__rangeset_search` renamed to `_mulle__rangeset_search_location`
* make empty const structs #defined for { 0}, not really API breaking IMO, but will give problems if you do `&mulle__pointersetenumerator_empty` (but why would you ?
* more dumbing down of perfectly fine code for adversarial compiler writers
* add clib.json to project


## 6.1.0

* GLOBAL declaration changes for Windows
* added function `mulle_structarray_get_first`
* added function `mulle_structarray_get_current`
* added function `mulle_structarray_get_allocator`
* added function `mulle_structarray_reset`


# 6.0.0

* new function `mulle_array_copy` for completeness
* new function `mulle_set_add_set` for completeness
* new function `mulle_map_add_map` for completeness
* `mulle_array_create` gets a capacity parameter, to make it more similiar to `mulle_map_create` and `mulle_set_create`
* `_mulle__pointermapenumerator_next_pair` now returns the pair as other pair enumerator functions do, at the expense of a larger enumerator
* fixed `mulle_array` to also respect 'notakey'
* added `mulle_array_remove` function
* added `MULLE_CONTAINER_EXTERN_GLOBAL` to some functions
* fix `notakey` sometimes not being used but NULL instead
* add ``mulle_map_init`` and ``mulle_map_done``


# 5.0.0

* `struct mulle_range` and `mulle_not_found_e` moved to mulle-data
* continued to unify enumerators to return value by reference and state by int return value
* added more convenience functions to structarray and pointerarray
* removed allocator from function signatures of `mulle_pointerarray_guarantee` and `mulle_pointerarray_grow` which was superflous
* added `mulle_pointerarray_advance`
* renamed all `*_remove_last` functions to `*_pop`
* there is now a `mulle_pointerqueue` that complements the `mulle__pointerqueue` without the embedded allocator


# 4.0.0

* still flip/flopping on the index type. Started with ``uintpr_t`,` then moved to ``size_t`` and now `unsigned int`. Don't want to typedef this though. See dox/DATATYPES.md for this.
* moved prime and hash code to new project mulle-data
* removed some stub documentation, that now does more harm than good
* added verb modifiers to the function naming style doc
* added `describe` for debugging
* `guarantee` now returns a non-null pointer on success
* new range get methods like ``_mulle__array_get_in_range`` for example
* `compact` as a verb is no more, use ``size_to_fit``
* enumerators now nil the by reference item when they are exhausted
* redid all enumerators, so that they are uniformly returning the value by reference
* made the capabilities of the various data structures more orthogonal to each other, but there is still some functions lacking
* There is no more notakey functionality in the arrays
* Improved README with nice pictures
* redid mulle-set it is now based on a mulle--pointerset, similar to mulle-array
* mulle--pointers have no notakey and mulle-pointerarray has notakey
* add mulle--pointerset and mulle--pointerarray
* added ``_mulle__map_count_collisions``
* add mulle-structarray


### 3.0.1

* Various small improvements

# 3.0.0

* moved nonnull stuff to mulle-c11, fixed up container functions with `MULLE_C_NONNULL` declarations
* renamed incomplete datastructures from `_mulle_<name>` to `mulle__<name>`. Functions that are checking for null parameters and those that don't can now be discerned properly.
* renamed `mulle_pointermap` to `_mulle_pointermap` because it contains no allocator
* added shrinking to map and set for the benefit of enumeration
* fixed range so that ranges with 0 length are always contained f.e
* added mulle-pointermap
* add `mulle_range` and `_mulle_rangeset` to project
* move some common code to mulle-container-math.h
* don't necessarily make the given capacity a power of two to conserve space


# 2.0.0

* removed mulle-bigmap and mulle-bigset, which are currently unused but now buggy due to recent changes in mulle-map and mulle-set
* rewrote `_mulle_set` and `_mulle_map` for improved performance
* rewrote `_mulle_map` to internally use a different memory layout, which in theory should be better for the cpu cache
* small maps get will be preceeded by a quick scan for pointer equality, which can speed up things considerably
* Improved the container callback functions with a few typedefs.
* Fixed bugs in `_mulle-pointerqueue`
* Added `mulle_pointerarray_enumerate_nil,` for when you know that notakey is nil. It allows passing in a nil array for enumeration


### 1.2.1

* Improved the container callback functions with a few typedefs.
* Fixed bugs in `_mulle-pointerqueue`
* Added `mulle_pointerarray_enumerate_nil,` for when you know that notakey is nil. It allows passing in a nil array for enumeration

## 1.2.0

* added `mulle_pointerarray_create`
* renamed `mulle_pointerarray_enumerator`  to `mulle_pointerarrayenumerator`
* added empty enumerator structs
* renamed and redid `_mulle_queue` as `_mulle_pointerqueue`


### 1.1.8

* modernized to mulle-sde with .mulle folder

### 1.1.7

* use memmove in strategic places

### 1.1.6

* fix travis

### 1.1.5

* upgrade to newest mulle-sde

### 1.1.4

* hide farmhash headers

### 1.1.3

* use latest instead of mulle-c

### 1.1.2

* fix cityhash-info remnant

### 1.1.1

* less verbose travis

## 1.1.0

* migrated to farmhash and got rid of the painful configure step


### 1.0.8

* fix for mingw, update project

### 1.0.7

* modernized mulle-sde

### 1.0.6

* set -std=c99 for everybody

### 1.0.5

* fix corrupted environment

### 1.0.4

* Various small improvements

### 1.0.3

* Various small improvements

### 1.0.2

* fix travis.yml

### 1.0.1

* fix travis

# 1.0.0

* migrated to mulle-sde
* made headernames hyphenated
* no longer distributed as a homebrew package


### 0.10.9

* Various small improvements

### 0.10.7

* support new mulle-tests

### 0.10.5

* fixed scion wrapper command

### 0.10.3

* follow mulle-configuration 3.1 changes and move .travis.yml to trusty

## 0.10.1

* adapt to mulle-configuration 2.0


## 0.9.3

* make it a cmake "C" project

## 0.9.1

* drastic API change  _keys_and_values functions have been replaced by
values_for_keys functions that reflect the parameter order. sorry

# 0.8.10

* community version

# 0.8.7

* merge in community version

# 0.8.6

* fix CMakeList.txt

# 0.8.4

* improve README

# 0.8.3

* merge in community version changes

# 0.8.2

* fixes for community version

# 0.8

* documentation, documentation, documentation
* move mulle-buffer into own library, because it really is quite different
* adapt to mulle-alloctor 2.0

# 0.7

* fix crass bug of misplaced pointer hash function
* remove some warnings


# 0.6

* ensure it's C99 compatible
* weaned off `<unistd.h>`, lost `off_t` in the process though, might come to
  regret this.

# 0.5

* don't malloc a block on extract, if there is nothing in the buffer
* moved the hexdumper here, because I want to reuse it

# 0.4

* upgrade mulle_allocator
* rename `_mulle__buffer_add_string_with_length` to
  `_mulle__buffer_add_string_with_maxlength` and fix the assert
* added mulle_pointerpairarray
* renamed keyvaluepair to pointerpair

# 0.3

* Fix problems with notakey being different than 0
* make buffer growth work as intended


# 0.2

* Moved the allocator out of the callback, since that clashes with the use
in MulleObjCFoundation.

# 0.1

* Adapted to new mulle_allocator version.
