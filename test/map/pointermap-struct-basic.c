#include "include.h"

static void test_struct_pointermap_basics(void)
{
    struct mulle__pointermap  map;
    void                     *value;
    
    printf("Testing struct map init/done/reset\n");
    
    // Test initialization
    _mulle__pointermap_init(&map, 4, NULL);
    printf("Initialized map count: %ld\n", (long) _mulle__pointermap_get_count(&map));
    printf("Initialized map size: %ld\n", (long) _mulle__pointermap_get_size(&map));
    
    // Test basic operations
    mulle__pointermap_set(&map, "key1", "value1", NULL);
    printf("Count after set: %ld\n", (long) _mulle__pointermap_get_count(&map));
    
    value = mulle__pointermap_get(&map, "key1");
    printf("Retrieved value: %s\n", value ? (char*) value : "NULL");
    
    // Test state queries
    printf("Map is full: %s\n", _mulle__pointermap_is_full(&map) ? "true" : "false");
    printf("Map is sparse: %s\n", _mulle__pointermap_is_sparse(&map) ? "true" : "false");
    
    // Test reset
    _mulle__pointermap_reset(&map, NULL);
    printf("Count after reset: %ld\n", (long) _mulle__pointermap_get_count(&map));
    
    // Clean up
    _mulle__pointermap_done(&map, NULL);
}

static void test_struct_pointermap_create_destroy(void)
{
    struct mulle__pointermap  *map;
    
    printf("Testing struct map create/destroy\n");
    
    // Test creation with different parameters
    map = mulle__pointermap_create(8, 16, NULL);
    if( map)
    {
        printf("Created map with capacity 8, extra 16\n");
        printf("Map count: %ld, size: %ld\n", 
               (long) mulle__pointermap_get_count(map),
               (long) mulle__pointermap_get_size(map));
        
        mulle__pointermap_destroy(map, NULL);
        printf("Map destroyed\n");
    }
    else
    {
        printf("Failed to create map\n");
    }
}

static void test_struct_edge_cases(void)
{
    struct mulle__pointermap  *map;
    
    printf("Testing edge cases\n");
    
    // Test NULL safety
    mulle__pointermap_destroy(NULL, NULL);
    mulle__pointermap_reset(NULL, NULL);
    
    printf("NULL safety count: %ld\n", (long) mulle__pointermap_get_count(NULL));
    printf("NULL safety size: %ld\n", (long) mulle__pointermap_get_size(NULL));
    printf("NULL is full: %s\n", mulle__pointermap_is_full(NULL) ? "true" : "false");
    printf("NULL is sparse: %s\n", mulle__pointermap_is_sparse(NULL) ? "true" : "false");
    
    // Test zero capacity
    map = mulle__pointermap_create(0, 0, NULL);
    if( map)
    {
        printf("Created zero-capacity map\n");
        mulle__pointermap_destroy(map, NULL);
    }
}

int main(void)
{
    printf("Testing mulle--pointermap-struct.c functions\n");
    
    test_struct_pointermap_basics();
    test_struct_pointermap_create_destroy();
    test_struct_edge_cases();
    
    printf("All struct pointermap tests completed\n");
    return( 0);
}
