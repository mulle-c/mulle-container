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
* rename `_mulle_buffer_add_string_with_length` to
  `_mulle_buffer_add_string_with_maxlength` and fix the assert
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
