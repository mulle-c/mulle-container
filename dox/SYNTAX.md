# Syntax


## Data structure

A [data structure](https://en.wikipedia.org/wiki/Data_structure)
is a collection of a struct and functions operating on it.


### Struct

If a data structure interfaces with mulle-allocator, it
(see: mulle-allocator) is significant and distinguished in the syntax, if the
data structure keeps track of the allocator in its struct:

``` c
struct my__data
{
   void                    *bytes;
   size_t                  length;
   // no allocator, therefore my__data and not my_data
};
```

``` c
struct my_data
{
   void                    *bytes;
   size_t                  length;
   struct mulle_allocator  *allocator;
};
```


The `struct` is namedspaced with a string, which is always "mulle"
in case of the "mulle_container" library,

```
struct                   : struct_with_allocator
                         | struct_without_allocator
                         ;

struct_with_allocator    : namespace '_' struct_identifier
                         ;

struct_without_allocator : namespace '_'  '_' struct_identifier
                         ;

namespace                : "mulle"

struct_identifier        : [a-z][a-z0-9]+
                         ;
```

Notice that there are no uppercase characters and no '`_`' underscore character
in a struct_identifier.

A tightly coupled struct inherits the allocator identification, even if itself
has no allocator or adds an allocator. A typical case is an enumerator:

``` c
struct my_dataenumerator
{
   void                    *curr;
   size_t                  length;  // has no allocator, but belongs to `my_data`
                                     // therefore not my__dataenumerator
};

```

#### Fields

Structs are composed of fields. It's distinguished, if a field is supposed to
be accessed via accessor functions only (private).

``` c
struct mulle__string
{
   char     *s;      // public
   size_t   _len;    // private
};

size_t   mulle__string_get_length( struct mulle__string *p);
```


```
field            : public_field
                 | private_field
                 ;

public_field     : field_identifier
                 ;

private_field    : '_'  field_identifier
                 ;

field_identifier : [a-z][a-z0-9_]*
                 ;
```

Notice that no uppercase characters are part of a field_identifier.
Underscores following an initial letter are fine, but should not be the last
character.


### Functions

Functions of a data structure are prefixed with the name of the struct.
What the function does, is expressed in its name which is most often a
[verb](VERBS.html) with a predefined meaning:

```
verb            : [a-z_][a-z0-9_]+
                ;

```

``` c
size_t   mulle__string_get_length( struct mulle__string *s);
```

An unsafe_function function does not actively check the given arguments for
NULL pointers. A safe_function guarantees to not crash on NULL pointer input.


```
function            : unsafe_function
                    | safe_function
                    ;

unsafe_function     :  '_' struct '_' function_identifier
                    ;

safe_function       :  struct '_' function_identifier
                    ;

function_identifier : verb
                    | [a-z_][a-z0-9_]+
                    ;

verb                : "add"
                    | "advance"
                    | "alloc"
                    | "copy"
                    | "copy_items"
                    | "count"
                    | "create"
                    | "destroy"
                    | "done"
                    | "enumerate"
                    | "extract"
                    | "find"
                    | "free"
                    | "get"
                    | "get_allocator"
                    | "get_callback"
                    | "get_count"
                    | "get_length"
                    | "get_notakey"
                    | "guarantee"
                    | "init"
                    | "insert"
                    | "make"
                    | "member"
                    | "next"
                    | "register"
                    | "reset"
                    | "reserve"
                    | "search"
                    | "set"
                    ;
```