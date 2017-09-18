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
