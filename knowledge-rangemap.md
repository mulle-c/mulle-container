# mulle-rangemap Knowledge

## Terminology

### Concepts

1. `range` - A mulle_range structure defining a location and length
2. `value` - Associated pointer value for each range
3. `merged value` - When ranges are combined, the leftmost value is kept

## Structure Layout

- Single contiguous memory area split into two sections:
  1. First half: Array of ranges
  2. Second half: Array of corresponding values

## Key Operations

### Insert
- Merges adjacent ranges
- Keeps leftmost value during merges
- Can return merged values through optional parameter

### Remove
- Can split ranges
- Returns removed values through optional array
- Maintains range integrity

## Testing

### Build and Test Commands

1. Build the library:
```bash
mulle-sde craft
```

2. Rebuild test environment (required after library changes):
```bash
mulle-sde test craft
```

3. Run specific test:
```bash
mulle-sde test run test/rangemap/30_shift/shift.c
```

### Test Categories

- `00_noleak` - Memory management tests
- `10_add` - Basic and edge case insertion tests
  - `simple_add.c` - Basic functionality
  - `invalid_add.c` - Invalid inputs
  - `inconvenient_add.c` - Complex merging cases
- `20_remove` - Removal operation tests
- `30_shift` - Range shifting tests
- `40_stress` - Random operation stress tests
- `50_search` - Search operation tests
- `60_operations` - Other operations (intersect, etc.)

### Test Style Guidelines

1. Follow project's testing pattern:
   - Clear printf output
   - Test both empty and non-empty cases
   - Include .stdout files for verification
   - Test edge cases and error conditions

2. Value-specific testing:
   - Verify value preservation during merges
   - Track merged values
   - Validate value associations after operations
