#include <mulle-container/mulle-container.h>
#include <stdio.h>

int main(void)
{
    struct mulle__rangemap map;
    struct mulle_range range;
    void *value;
    int result;
    uintptr_t index;
    
    printf("Testing rangemap edge cases and boundary conditions\n");
    
    // Test 1: NULL parameter handling
    printf("\nTest 1: NULL parameter handling\n");
    
    // Test NULL allocator in init
    _mulle__rangemap_init(&map, 4, NULL);
    printf("NULL allocator init handled gracefully\n");
    
    // Test 2: Invalid range handling
    printf("\nTest 2: Invalid range handling\n");
    
    _mulle__rangemap_init(&map, 4, NULL);
    
    // Test zero length range
    range = mulle_range_make(10, 0);
    result = _mulle__rangemap_insert(&map, range, (void *)0x1234, NULL);
    printf("Zero length range insert returned: %d (expected EINVAL=%d)\n", result, EINVAL);
    
    // Test invalid range
    range = mulle_range_make(20, 0);
    result = _mulle__rangemap_insert(&map, range, (void *)0x1234, NULL);
    printf("Invalid range insert returned: %d (expected EINVAL=%d)\n", result, EINVAL);
    
    // Test 3: Empty map operations
    printf("\nTest 3: Empty map operations\n");
    
    _mulle__rangemap_reset(&map, NULL);
    printf("Empty map count: %zu\n", _mulle__rangemap_get_count(&map));
    printf("Empty map is empty: %d\n", _mulle__rangemap_is_empty(&map));
    
    // Search in empty map
    index = _mulle__rangemap_search(&map, 100);
    printf("Search in empty map returned: %zu (expected %zu)\n", index, mulle_not_found_e);
    
    // Remove from empty map
    range = mulle_range_make(100, 10);
    result = _mulle__rangemap_remove(&map, range, NULL);
    printf("Remove from empty map returned: %d (expected ENOENT=%d)\n", result, ENOENT);
    
    // Test 4: Boundary conditions
    printf("\nTest 4: Boundary conditions\n");
    
    // Test adjacent ranges
    range = mulle_range_make(10, 5);
    result = _mulle__rangemap_insert(&map, range, (void *)0x1000, NULL);
    printf("Insert range 10-15: %d\n", result);
    
    range = mulle_range_make(15, 5);
    result = _mulle__rangemap_insert(&map, range, (void *)0x2000, NULL);
    printf("Insert adjacent range 15-20: %d\n", result);
    
    range = mulle_range_make(5, 5);
    result = _mulle__rangemap_insert(&map, range, (void *)0x3000, NULL);
    printf("Insert adjacent range 5-10: %d\n", result);
    
    // Test overlapping ranges
    range = mulle_range_make(12, 5);
    result = _mulle__rangemap_insert(&map, range, (void *)0x4000, NULL);
    printf("Insert overlapping range 12-17: %d (expected EADDRINUSE=%d)\n", result, EADDRINUSE);
    
    // Test exact match removal
    range = mulle_range_make(10, 5);
    result = _mulle__rangemap_remove(&map, range, NULL);
    printf("Remove exact range 10-15: %d\n", result);
    
    // Test non-exact match removal
    range = mulle_range_make(10, 3);
    result = _mulle__rangemap_remove(&map, range, NULL);
    printf("Remove non-exact range 10-13: %d (expected EACCES=%d)\n", result, EACCES);
    
    // Test 5: Index boundary conditions
    printf("\nTest 5: Index boundary conditions\n");
    
    _mulle__rangemap_reset(&map, NULL);
    
    // Fill map with ranges
    for (int i = 0; i < 5; i++) {
        range = mulle_range_make(i * 10, 5);
        result = _mulle__rangemap_insert(&map, range, (void *)(uintptr_t)(0x1000 + i), NULL);
        if (result != 0) {
            printf("Failed to insert range %d: %d\n", i, result);
            break;
        }
    }
    
    printf("Map count after filling: %zu\n", _mulle__rangemap_get_count(&map));
    
    // Test invalid indices
    range = _mulle__rangemap_get_range(&map, 100);
    printf("Get range at invalid index 100: location=%zu, length=%zu (expected invalid)\n", 
           range.location, range.length);
    
    value = _mulle__rangemap_get_value(&map, 100);
    printf("Get value at invalid index 100: %p (expected NULL)\n", value);
    
    // Test valid indices
    for (int i = 0; i < 5; i++) {
        range = _mulle__rangemap_get_range(&map, i);
        value = _mulle__rangemap_get_value(&map, i);
        printf("Index %d: range=%zu-%zu, value=%p\n", 
               i, range.location, range.location + range.length, value);
    }
    
    // Test 6: Enumeration
    printf("\nTest 6: Enumeration\n");
    
    struct mulle__rangemapenumerator rover;
    struct mulle_range enum_range;
    void *enum_value;
    int count = 0;
    
    rover = _mulle__rangemap_enumerate(&map);
    while (_mulle__rangemapenumerator_next(&rover, &enum_range, &enum_value)) {
        printf("Enumerated %d: range=%zu-%zu, value=%p\n", 
               count++, enum_range.location, 
               enum_range.location + enum_range.length, enum_value);
    }
    
    // Test 7: Search operations
    printf("\nTest 7: Search operations\n");
    
    // Search for existing location
    index = _mulle__rangemap_search(&map, 10);
    printf("Search for location 10: %zu\n", index);
    
    // Search for non-existing location
    index = _mulle__rangemap_search(&map, 7);
    printf("Search for location 7: %zu (expected %zu)\n", index, mulle_not_found_e);
    
    // Search at boundaries
    index = _mulle__rangemap_search(&map, 0);
    printf("Search for location 0: %zu\n", index);
    
    index = _mulle__rangemap_search(&map, 50);
    printf("Search for location 50: %zu\n", index);
    
    // Test 8: Reset operations
    printf("\nTest 8: Reset operations\n");
    
    printf("Count before reset: %zu\n", _mulle__rangemap_get_count(&map));
    _mulle__rangemap_reset(&map, NULL);
    printf("Count after reset: %zu\n", _mulle__rangemap_get_count(&map));
    
    // Test 9: Cleanup
    printf("\nTest 9: Cleanup\n");
    
    _mulle__rangemap_done(&map, NULL);
    printf("Cleanup completed successfully\n");
    
    printf("\nAll rangemap edge case tests completed\n");
    
    return 0;
}