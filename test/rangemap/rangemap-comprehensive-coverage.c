#include "include.h"

static void test_rangemap_basic_operations(void)
{
    struct mulle__rangemap   map;
    struct mulle_range       range1 = { 10, 5 };   // 10-14
    struct mulle_range       range2 = { 20, 3 };   // 20-22
    struct mulle_range       range3 = { 5, 3 };    // 5-7
    uintptr_t               index;
    void                    *value;
    
    printf("Testing rangemap basic operations\n");
    
    _mulle__rangemap_init(&map, 4, NULL);
    printf("Initialized rangemap, count: %ld\n", (long) _mulle__rangemap_get_count(&map));
    
    // Test basic insert operations
    if( _mulle__rangemap_insert(&map, range1, "value1", NULL) == 0)
    {
        printf("Successfully inserted range1\n");
    }
    printf("After adding range1, count: %ld\n", (long) _mulle__rangemap_get_count(&map));
    
    if( _mulle__rangemap_insert(&map, range2, "value2", NULL) == 0)
    {
        printf("Successfully inserted range2\n");
    }
    printf("After adding range2, count: %ld\n", (long) _mulle__rangemap_get_count(&map));
    
    if( _mulle__rangemap_insert(&map, range3, "value3", NULL) == 0)
    {
        printf("Successfully inserted range3\n");
    }
    printf("After adding range3, count: %ld\n", (long) _mulle__rangemap_get_count(&map));
    
    // Test search operations
    index = _mulle__rangemap_search(&map, 12);
    if( index != mulle_not_found_e)
    {
        value = _mulle__rangemap_get_value(&map, index);
        printf("Search at location 12: %s\n", value ? (char*)value : "NULL");
    }
    else
    {
        printf("Search at location 12: not found\n");
    }
    
    index = _mulle__rangemap_search(&map, 21);
    if( index != mulle_not_found_e)
    {
        value = _mulle__rangemap_get_value(&map, index);
        printf("Search at location 21: %s\n", value ? (char*)value : "NULL");
    }
    else
    {
        printf("Search at location 21: not found\n");
    }
    
    // Test search in empty areas
    index = _mulle__rangemap_search(&map, 0);
    printf("Search at location 0: %s\n", index == mulle_not_found_e ? "not found" : "found");
    
    index = _mulle__rangemap_search(&map, 18);
    printf("Search at location 18 (gap): %s\n", index == mulle_not_found_e ? "not found" : "found");
    
    _mulle__rangemap_done(&map, NULL);
}

static void test_rangemap_remove_operations(void)
{
    struct mulle__rangemap   map;
    struct mulle_range       range1 = { 10, 5 };
    struct mulle_range       range2 = { 20, 5 };
    uintptr_t               index;
    void                    *value;
    
    printf("Testing rangemap remove operations\n");
    
    _mulle__rangemap_init(&map, 8, NULL);
    
    // Set up initial ranges
    if( _mulle__rangemap_insert(&map, range1, "range1", NULL) == 0)
    {
        printf("Inserted range1\n");
    }
    if( _mulle__rangemap_insert(&map, range2, "range2", NULL) == 0)
    {
        printf("Inserted range2\n");
    }
    
    printf("Initial setup, count: %ld\n", (long) _mulle__rangemap_get_count(&map));
    
    // Verify initial state
    index = _mulle__rangemap_search(&map, 12);
    if( index != mulle_not_found_e)
    {
        value = _mulle__rangemap_get_value(&map, index);
        printf("Before remove - search at 12: %s\n", value ? (char*)value : "NULL");
    }
    
    // Test remove operation
    if( _mulle__rangemap_remove(&map, range1, NULL) == 0)
    {
        printf("Successfully removed range1\n");
    }
    printf("After remove, count: %ld\n", (long) _mulle__rangemap_get_count(&map));
    
    // Verify state after remove
    index = _mulle__rangemap_search(&map, 12);
    printf("After remove - search at 12: %s\n", index == mulle_not_found_e ? "not found" : "found");
    
    _mulle__rangemap_done(&map, NULL);
}

static void test_rangemap_edge_cases(void)
{
    struct mulle__rangemap   map;
    struct mulle_range       zero_range = { 0, 0 };     // Empty range
    struct mulle_range       single = { 100, 1 };       // Single location
    struct mulle_range       large = { 1000, 1000 };    // Large range
    uintptr_t               index;
    void                    *value;
    
    printf("Testing rangemap edge cases\n");
    
    _mulle__rangemap_init(&map, 8, NULL);
    
    // Test empty range (should be handled gracefully)
    if( _mulle__rangemap_insert(&map, zero_range, "zero", NULL) == 0)
    {
        printf("Added zero range, count: %ld\n", (long) _mulle__rangemap_get_count(&map));
    }
    
    // Test single location range
    if( _mulle__rangemap_insert(&map, single, "single", NULL) == 0)
    {
        printf("Added single location, count: %ld\n", (long) _mulle__rangemap_get_count(&map));
    }
    
    // Test large range  
    if( _mulle__rangemap_insert(&map, large, "large", NULL) == 0)
    {
        printf("Added large range, count: %ld\n", (long) _mulle__rangemap_get_count(&map));
    }
    
    // Test searches
    index = _mulle__rangemap_search(&map, 0);
    if( index != mulle_not_found_e)
    {
        value = _mulle__rangemap_get_value(&map, index);
        printf("Search at 0: %s\n", value ? (char*)value : "NULL");
    }
    else
    {
        printf("Search at 0: not found\n");
    }
    
    index = _mulle__rangemap_search(&map, 100);
    if( index != mulle_not_found_e)
    {
        value = _mulle__rangemap_get_value(&map, index);
        printf("Search at 100: %s\n", value ? (char*)value : "NULL");
    }
    else
    {
        printf("Search at 100: not found\n");
    }
    
    index = _mulle__rangemap_search(&map, 1500);
    if( index != mulle_not_found_e)
    {
        value = _mulle__rangemap_get_value(&map, index);
        printf("Search at 1500: %s\n", value ? (char*)value : "NULL");
    }
    else
    {
        printf("Search at 1500: not found\n");
    }
    
    index = _mulle__rangemap_search(&map, 2500);
    printf("Search at 2500 (beyond): %s\n", index == mulle_not_found_e ? "not found" : "found");
    
    _mulle__rangemap_done(&map, NULL);
}

static void test_rangemap_boundary_conditions(void)
{
    struct mulle__rangemap   map;
    struct mulle_range       adjacent1 = { 10, 5 };     // 10-14
    struct mulle_range       adjacent2 = { 15, 5 };     // 15-19 (immediately adjacent)
    struct mulle_range       touching = { 5, 5 };       // 5-9 (touching adjacent1)
    uintptr_t               index;
    void                    *value;
    
    printf("Testing rangemap boundary conditions\n");
    
    _mulle__rangemap_init(&map, 8, NULL);
    
    // Add adjacent ranges
    _mulle__rangemap_insert(&map, adjacent1, "adj1", NULL);
    _mulle__rangemap_insert(&map, adjacent2, "adj2", NULL);
    _mulle__rangemap_insert(&map, touching, "touching", NULL);
    
    printf("Added adjacent ranges, count: %ld\n", (long) _mulle__rangemap_get_count(&map));
    
    // Test critical boundary points
    index = _mulle__rangemap_search(&map, 4);
    printf("Search at 4 (before): %s\n", index == mulle_not_found_e ? "not found" : "found");
    index = _mulle__rangemap_search(&map, 5);
    if( index != mulle_not_found_e)
    {
        value = _mulle__rangemap_get_value(&map, index);
        printf("Search at 5 (start): %s\n", value ? (char*)value : "NULL");
    }
    index = _mulle__rangemap_search(&map, 9);
    if( index != mulle_not_found_e)
    {
        value = _mulle__rangemap_get_value(&map, index);
        printf("Search at 9 (end): %s\n", value ? (char*)value : "NULL");
    }
    index = _mulle__rangemap_search(&map, 10);
    if( index != mulle_not_found_e)
    {
        value = _mulle__rangemap_get_value(&map, index);
        printf("Search at 10 (next start): %s\n", value ? (char*)value : "NULL");
    }
    index = _mulle__rangemap_search(&map, 14);
    if( index != mulle_not_found_e)
    {
        value = _mulle__rangemap_get_value(&map, index);
        printf("Search at 14 (end): %s\n", value ? (char*)value : "NULL");
    }
    index = _mulle__rangemap_search(&map, 15);
    if( index != mulle_not_found_e)
    {
        value = _mulle__rangemap_get_value(&map, index);
        printf("Search at 15 (adjacent start): %s\n", value ? (char*)value : "NULL");
    }
    index = _mulle__rangemap_search(&map, 19);
    if( index != mulle_not_found_e)
    {
        value = _mulle__rangemap_get_value(&map, index);
        printf("Search at 19 (final end): %s\n", value ? (char*)value : "NULL");
    }
    index = _mulle__rangemap_search(&map, 20);
    printf("Search at 20 (beyond): %s\n", index == mulle_not_found_e ? "not found" : "found");
    
    _mulle__rangemap_done(&map, NULL);
}

static void test_rangemap_overlapping_ranges(void)
{
    struct mulle__rangemap   map;
    struct mulle_range       base = { 10, 10 };     // 10-19
    struct mulle_range       overlap1 = { 5, 8 };   // 5-12 (overlaps start)
    struct mulle_range       overlap2 = { 15, 8 };  // 15-22 (overlaps end)
    struct mulle_range       inside = { 12, 3 };    // 12-14 (inside)
    struct mulle_range       outside = { 25, 5 };   // 25-29 (no overlap)
    uintptr_t               index;
    void                    *value;
    
    printf("Testing rangemap overlapping ranges\n");
    
    _mulle__rangemap_init(&map, 10, NULL);
    
    // Add base range first
    _mulle__rangemap_insert(&map, base, "base", NULL);
    printf("Added base range 10-19, count: %ld\n", (long) _mulle__rangemap_get_count(&map));
    
    // Test overlapping additions (behavior depends on implementation)
    _mulle__rangemap_insert(&map, overlap1, "overlap1", NULL);
    printf("Added overlap1 5-12, count: %ld\n", (long) _mulle__rangemap_get_count(&map));
    
    _mulle__rangemap_insert(&map, overlap2, "overlap2", NULL);
    printf("Added overlap2 15-22, count: %ld\n", (long) _mulle__rangemap_get_count(&map));
    
    _mulle__rangemap_insert(&map, inside, "inside", NULL);
    printf("Added inside 12-14, count: %ld\n", (long) _mulle__rangemap_get_count(&map));
    
    _mulle__rangemap_insert(&map, outside, "outside", NULL);
    printf("Added outside 25-29, count: %ld\n", (long) _mulle__rangemap_get_count(&map));
    
    // Test searches at key boundary points
    index = _mulle__rangemap_search(&map, 5);
    if( index != mulle_not_found_e)
    {
        value = _mulle__rangemap_get_value(&map, index);
        printf("Search at 5: %s\n", value ? (char*)value : "NULL");
    }
    index = _mulle__rangemap_search(&map, 10);
    if( index != mulle_not_found_e)
    {
        value = _mulle__rangemap_get_value(&map, index);
        printf("Search at 10: %s\n", value ? (char*)value : "NULL");
    }
    index = _mulle__rangemap_search(&map, 15);
    if( index != mulle_not_found_e)
    {
        value = _mulle__rangemap_get_value(&map, index);
        printf("Search at 15: %s\n", value ? (char*)value : "NULL");
    }
    index = _mulle__rangemap_search(&map, 27);
    if( index != mulle_not_found_e)
    {
        value = _mulle__rangemap_get_value(&map, index);
        printf("Search at 27: %s\n", value ? (char*)value : "NULL");
    }
    
    _mulle__rangemap_done(&map, NULL);
}

int main(void)
{
    printf("Testing mulle--rangemap comprehensive coverage\n");
    
    test_rangemap_basic_operations();
    test_rangemap_overlapping_ranges();
    test_rangemap_remove_operations();
    test_rangemap_edge_cases();
    test_rangemap_boundary_conditions();
    
    printf("All rangemap coverage tests completed\n");
    return( 0);
}
