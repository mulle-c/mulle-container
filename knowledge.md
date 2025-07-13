# mulle-container Knowledge

## Terminology

### Size-related Terms

The codebase carefully distinguishes between different size-related concepts:

1. `capacity` 
   - Used in public APIs (create/init functions)
   - Represents the user's requested/expected number of elements
   - Example: `mulle_map_create(size_t capacity, ...)`

2. `_size` 
   - Internal implementation detail
   - Actual allocated space after rounding up to power of 2
   - Always larger than or equal to capacity
   - Not exposed through public API
   - Used for internal storage allocation

3. `_count`
   - Current number of elements actually stored
   - Accessible through `get_count()` functions
   - Example: `mulle_map_get_count()`

### Mutation Tracking

The codebase uses mutation tracking to detect concurrent modifications during enumeration:

1. Container Base Structure
   - Add `_n_mutations` counter when not in MISER_MODE
   - Example:
   ```c
   #ifndef MULLE__CONTAINER_MISER_MODE
   #define MULLE__CONTAINER_BASE          \
       _MULLE__CONTAINER_BASE;            \
       uintptr_t  _n_mutations
   #endif
   ```

2. Mutation Points
   - Increment counter on any operation that modifies container:
     - Adding/removing elements
     - Resizing storage
     - Direct array modifications
     - Memory reallocation
   - Example: `array->_n_mutations++`

3. Enumerator Structure
   - Must match container's mutation tracking
   - Stores container reference and mutation count
   - Example:
   ```c
   #if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   struct mulle__containerenumerator
   {
      _MULLE__CONTAINERENUMERATOR_BASE;
      struct mulle__container *_array;
      uintptr_t  _n_mutations;
   };
   #endif
   ```

4. Enumeration Safety
   - Check mutation count during enumeration
   - Assert if count changed
   - Example:
   ```c
   assert( rover->_array->_n_mutations == rover->_n_mutations && 
          "array was modified during enumeration");
   ```

5. Public/Private Interface Pattern
   - Private enumerator (mulle__) defines base fields
   - Public enumerator (mulle_) inherits base and adds mutation tracking
   - Define empty enumerator for both
   - Example:
   ```c
   // Private
   #define MULLE__SETENUMERATOR_BASE   \
      void     **_curr;                \
      size_t   _left;                  \
      void     *_notakey

   // Public inherits private
   #define MULLE_SETENUMERATOR_BASE   MULLE__SETENUMERATOR_BASE

   // Empty enumerator definition
   #define mulle_setenumerator_empty  \
      ((struct mulle_setenumerator) { 0 })
   ```

### Usage Guidelines

- Use `capacity` when asking user for expected size in constructors
- Use `get_count()` to query current number of elements
- Never expose internal `_size` through public API
- Size management (growing/shrinking) should remain internal
- Track mutations on all container modifications
- Match mutation tracking in public and private interfaces

## Build and Test

### Basic Commands

1. Regular build:
```bash
unset MULLE_SDE_SANDBOX && mulle-sde craft -- --no-hook
```

2. Clean build (when dependencies need rebuild):
```bash
unset MULLE_SDE_SANDBOX && mulle-sde craft -g -- --no-hook
```
Note: `-g` is shortcut for `mulle-sde clean gravetidy`, only use when dependencies need rebuild

3. After adding new files:
```bash
unset MULLE_SDE_SANDBOX && mulle-sde reflect
```

4. Build test environment (after library changes):
```bash
unset MULLE_SDE_SANDBOX && mulle-sde test craft
```

5. Run specific test:
```bash
unset MULLE_SDE_SANDBOX && mulle-sde test run test/rangemap/30_shift/shift.c
```

IMPORTANT: Always unset MULLE_SDE_SANDBOX before mulle-sde commands

### Adding New Files

1. Create new source files in appropriate directory (e.g., src/rangemap/)
2. Include headers through "include.h" for public dependencies
3. Run `mulle-sde reflect` to update build system
4. Build with `mulle-sde craft`

## Coding Patterns

- Underscore prefix versions for non-NULL checking variants
- Consistent parameter ordering (container, key/value, callback, allocator)
- Public APIs should not expose implementation details
- Track mutations on all container modifications
- Match mutation tracking in public and private interfaces

## Design Philosophy

- Shield users from low-level implementation details
- Provide high-level container abstractions
- Avoid exposing raw C arrays or requiring their manual management
- Prefer clean enumeration patterns over creating temporary collections
- Keep interfaces consistent and self-contained within the library
- Don't add convenience methods that would require understanding additional container types
- Ensure enumeration safety through mutation tracking

## Testing Guidelines

### Test Organization

- Create separate .c files for each function or feature being tested
- Name test files descriptively: `<container>-<function>.c`
- Include matching .stdout files with expected output
- Keep tests focused and minimal

### Test Style

- Use printf to produce output for verification
- Compare output with .stdout files rather than using assertions
- Test both normal operation and edge cases
- Always include NULL handling tests
- Maintain proper memory management (create/destroy)
- Test output should be clear and descriptive
- Test enumeration with concurrent modifications

Example test structure:
```c
#include <mulle-container/mulle-container.h>

int main( void)
{
   // setup container and callbacks
   printf( "Test description: %d\n", result);
   // cleanup
   return( 0);
}
```
