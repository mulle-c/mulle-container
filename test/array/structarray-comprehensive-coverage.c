#include "include.h"

// Test structure for structarray tests
struct test_struct 
{
    int      id;
    char     name[32];
    double   value;
};

static int compare_test_structs(void *a, void *b, void *userinfo)
{
    struct test_struct *sa = (struct test_struct*)a;
    struct test_struct *sb = (struct test_struct*)b;
    
    (void)userinfo;
    return sa->id - sb->id;
}

static void test_structarray_basic_operations(void)
{
    struct mulle__structarray   *array;
    struct test_struct          item, *retrieved;
    size_t                      count, size;
    
    printf("Testing structarray basic operations\n");
    
    // Create and initialize array
    array = mulle__structarray_create(sizeof(struct test_struct), 
                                     alignof(struct test_struct), 
                                     4, NULL);
    if( ! array)
    {
        printf("Failed to create structarray\n");
        return;
    }
    
    count = _mulle__structarray_get_count(array);
    size = _mulle__structarray_get_size(array);
    printf("Initial array - count: %zu, size: %zu\n", count, size);
    printf("Element size: %zu\n", _mulle__structarray_get_element_size(array));
    
    // Test adding structs
    item = (struct test_struct){1, "first", 1.1};
    _mulle__structarray_add(array, &item, NULL);
    
    item = (struct test_struct){2, "second", 2.2};
    _mulle__structarray_add(array, &item, NULL);
    
    item = (struct test_struct){3, "third", 3.3};
    _mulle__structarray_add(array, &item, NULL);
    
    count = _mulle__structarray_get_count(array);
    printf("After adding 3 items - count: %zu\n", count);
    
    // Test getting structs
    retrieved = (struct test_struct*)_mulle__structarray_get(array, 0);
    if( retrieved)
    {
        printf("Retrieved item at index 0: id=%d, name=%s, value=%.1f\n",
               retrieved->id, retrieved->name, retrieved->value);
    }
    
    retrieved = (struct test_struct*)_mulle__structarray_get(array, 2);
    if( retrieved)
    {
        printf("Retrieved item at index 2: id=%d, name=%s, value=%.1f\n",
               retrieved->id, retrieved->name, retrieved->value);
    }
    
    // Test get first and last
    retrieved = (struct test_struct*)_mulle__structarray_get_first(array);
    if( retrieved)
    {
        printf("First item: id=%d, name=%s, value=%.1f\n",
               retrieved->id, retrieved->name, retrieved->value);
    }
    
    retrieved = (struct test_struct*)_mulle__structarray_get_last(array);
    if( retrieved)
    {
        printf("Last item: id=%d, name=%s, value=%.1f\n",
               retrieved->id, retrieved->name, retrieved->value);
    }
    
    mulle__structarray_destroy(array, NULL);
}

static void test_structarray_pop_and_set(void)
{
    struct mulle__structarray   *array;
    struct test_struct          item, popped;
    int                         success;
    
    printf("Testing structarray pop and set operations\n");
    
    array = mulle__structarray_create(sizeof(struct test_struct), 
                                     alignof(struct test_struct), 
                                     4, NULL);
    if( ! array)
    {
        printf("Failed to create structarray\n");
        return;
    }
    
    // Add some test data
    item = (struct test_struct){10, "ten", 10.0};
    _mulle__structarray_add(array, &item, NULL);
    
    item = (struct test_struct){20, "twenty", 20.0};
    _mulle__structarray_add(array, &item, NULL);
    
    item = (struct test_struct){30, "thirty", 30.0};
    _mulle__structarray_add(array, &item, NULL);
    
    printf("Initial count: %zu\n", _mulle__structarray_get_count(array));
    
    // Test pop operation
    success = _mulle__structarray_pop(array, &popped, NULL);
    if( success)
    {
        printf("Popped item: id=%d, name=%s, value=%.1f\n",
               popped.id, popped.name, popped.value);
    }
    else
    {
        printf("Pop failed\n");
    }
    
    printf("Count after pop: %zu\n", _mulle__structarray_get_count(array));
    
    // Test set operation
    item = (struct test_struct){99, "ninety-nine", 99.9};
    _mulle__structarray_set(array, 1, &item);
    
    struct test_struct *retrieved = (struct test_struct*)_mulle__structarray_get(array, 1);
    if( retrieved)
    {
        printf("After set at index 1: id=%d, name=%s, value=%.1f\n",
               retrieved->id, retrieved->name, retrieved->value);
    }
    
    // Test pop from empty array
    _mulle__structarray_reset(array);
    success = _mulle__structarray_pop(array, &popped, NULL);
    printf("Pop from empty array: %s\n", success ? "success" : "failed (expected)");
    
    mulle__structarray_destroy(array, NULL);
}

static void test_structarray_insert_and_remove(void)
{
    struct mulle__structarray   *array;
    struct test_struct          item;
    size_t                      count;
    
    printf("Testing structarray insert and remove operations\n");
    
    array = mulle__structarray_create(sizeof(struct test_struct), 
                                     alignof(struct test_struct), 
                                     4, NULL);
    if( ! array)
    {
        printf("Failed to create structarray\n");
        return;
    }
    
    // Add initial data
    item = (struct test_struct){1, "first", 1.0};
    _mulle__structarray_add(array, &item, NULL);
    
    item = (struct test_struct){3, "third", 3.0};
    _mulle__structarray_add(array, &item, NULL);
    
    printf("Initial count: %zu\n", _mulle__structarray_get_count(array));
    
    // Test insert in middle
    item = (struct test_struct){2, "second", 2.0};
    _mulle__structarray_insert(array, 1, &item, NULL);
    
    count = _mulle__structarray_get_count(array);
    printf("After insert at index 1 - count: %zu\n", count);
    
    // Print all items to verify order
    for( size_t i = 0; i < count; i++)
    {
        struct test_struct *retrieved = (struct test_struct*)_mulle__structarray_get(array, i);
        if( retrieved)
        {
            printf("Item %zu: id=%d, name=%s, value=%.1f\n", 
                   i, retrieved->id, retrieved->name, retrieved->value);
        }
    }
    
    // Test remove at index
    _mulle__structarray_remove_at_index(array, 1);
    count = _mulle__structarray_get_count(array);
    printf("After remove at index 1 - count: %zu\n", count);
    
    // Test remove last
    _mulle__structarray_remove_last(array);
    count = _mulle__structarray_get_count(array);
    printf("After remove last - count: %zu\n", count);
    
    mulle__structarray_destroy(array, NULL);
}

static void test_structarray_guarantee_and_reserve(void)
{
    struct mulle__structarray   *array;
    struct test_struct          item;
    void                        *reserved;
    size_t                      size_before, size_after;
    
    printf("Testing structarray guarantee and reserve operations\n");
    
    array = mulle__structarray_create(sizeof(struct test_struct), 
                                     alignof(struct test_struct), 
                                     2, NULL);  // Small initial size
    if( ! array)
    {
        printf("Failed to create structarray\n");
        return;
    }
    
    size_before = _mulle__structarray_get_size(array);
    printf("Initial size: %zu\n", size_before);
    
    // Test guarantee - ensure space for 10 more items
    reserved = _mulle__structarray_guarantee(array, 10, NULL);
    if( reserved)
    {
        printf("Guarantee succeeded\n");
        size_after = _mulle__structarray_get_size(array);
        printf("Size after guarantee: %zu\n", size_after);
    }
    
    // Test guaranteed add (no checks needed)
    item = (struct test_struct){100, "guaranteed", 100.0};
    _mulle__structarray_add_guaranteed(array, &item);
    printf("Added guaranteed item, count: %zu\n", _mulle__structarray_get_count(array));
    
    // Test reserve operation
    reserved = _mulle__structarray_reserve(array, NULL);
    if( reserved)
    {
        // We can now write directly to reserved space
        struct test_struct *direct = (struct test_struct*)reserved;
        *direct = (struct test_struct){200, "reserved", 200.0};
        printf("Reserved and wrote item directly, count: %zu\n", _mulle__structarray_get_count(array));
    }
    
    // Test advance
    reserved = _mulle__structarray_advance(array, 2, NULL);
    if( reserved)
    {
        printf("Advanced by 2, count: %zu\n", _mulle__structarray_get_count(array));
    }
    
    mulle__structarray_destroy(array, NULL);
}

static void test_structarray_static_storage(void)
{
    struct test_struct          static_storage[5];
    struct mulle__structarray   array;
    struct test_struct          item;
    size_t                      count;
    
    printf("Testing structarray with static storage\n");
    
    // Initialize with static storage
    _mulle__structarray_init_with_static_storage(&array,
                                                 sizeof(struct test_struct),
                                                 alignof(struct test_struct),
                                                 5,
                                                 static_storage);
    
    printf("Initialized with static storage, size: %zu\n", _mulle__structarray_get_size(&array));
    
    // Add some items
    item = (struct test_struct){1, "static1", 1.0};
    _mulle__structarray_add(&array, &item, NULL);
    
    item = (struct test_struct){2, "static2", 2.0};
    _mulle__structarray_add(&array, &item, NULL);
    
    count = _mulle__structarray_get_count(&array);
    printf("Count after adding 2 items: %zu\n", count);
    
    // Test getting items
    struct test_struct *retrieved = (struct test_struct*)_mulle__structarray_get(&array, 0);
    if( retrieved)
    {
        printf("Retrieved from static storage: id=%d, name=%s, value=%.1f\n",
               retrieved->id, retrieved->name, retrieved->value);
    }
    
    _mulle__structarray_done(&array, NULL);
}

static void test_structarray_sorting(void)
{
    struct mulle__structarray   *array;
    struct test_struct          item;
    size_t                      count;
    
    printf("Testing structarray sorting operations\n");
    
    array = mulle__structarray_create(sizeof(struct test_struct), 
                                     alignof(struct test_struct), 
                                     10, NULL);
    if( ! array)
    {
        printf("Failed to create structarray\n");
        return;
    }
    
    // Add items in random order
    item = (struct test_struct){30, "thirty", 30.0};
    _mulle__structarray_add(array, &item, NULL);
    
    item = (struct test_struct){10, "ten", 10.0};
    _mulle__structarray_add(array, &item, NULL);
    
    item = (struct test_struct){50, "fifty", 50.0};
    _mulle__structarray_add(array, &item, NULL);
    
    item = (struct test_struct){20, "twenty", 20.0};
    _mulle__structarray_add(array, &item, NULL);
    
    item = (struct test_struct){40, "forty", 40.0};
    _mulle__structarray_add(array, &item, NULL);
    
    count = _mulle__structarray_get_count(array);
    printf("Before sorting (count %zu):\n", count);
    for( size_t i = 0; i < count; i++)
    {
        struct test_struct *retrieved = (struct test_struct*)_mulle__structarray_get(array, i);
        printf("  Item %zu: id=%d, name=%s\n", i, retrieved->id, retrieved->name);
    }
    
    // Sort the array
    _mulle__structarray_qsort_r(array, compare_test_structs, NULL);
    
    printf("After sorting:\n");
    for( size_t i = 0; i < count; i++)
    {
        struct test_struct *retrieved = (struct test_struct*)_mulle__structarray_get(array, i);
        printf("  Item %zu: id=%d, name=%s\n", i, retrieved->id, retrieved->name);
    }
    
    mulle__structarray_destroy(array, NULL);
}

static void test_structarray_edge_cases(void)
{
    struct mulle__structarray   *array;
    struct test_struct          item;
    void                        *retrieved;
    
    printf("Testing structarray edge cases\n");
    
    array = mulle__structarray_create(sizeof(struct test_struct), 
                                     alignof(struct test_struct), 
                                     1, NULL);
    if( ! array)
    {
        printf("Failed to create structarray\n");
        return;
    }
    
    // Test operations on empty array
    retrieved = _mulle__structarray_get_first(array);
    printf("Get first from empty array: %s\n", retrieved ? "found" : "NULL (expected)");
    
    retrieved = _mulle__structarray_get_last(array);
    printf("Get last from empty array: %s\n", retrieved ? "found" : "NULL (expected)");
    
    retrieved = _mulle__structarray_get_lenient(array, 0);
    printf("Get lenient from empty array: %s\n", retrieved ? "found" : "NULL (expected)");
    
    // Add one item and test boundary conditions
    item = (struct test_struct){1, "boundary", 1.0};
    _mulle__structarray_add(array, &item, NULL);
    
    // Test lenient get with valid and invalid indices
    retrieved = _mulle__structarray_get_lenient(array, 0);
    printf("Get lenient at valid index 0: %s\n", retrieved ? "found" : "NULL");
    
    retrieved = _mulle__structarray_get_lenient(array, 10);
    printf("Get lenient at invalid index 10: %s\n", retrieved ? "found" : "NULL (expected)");
    
    // Test current pointer access
    retrieved = _mulle__structarray_get_current(array);
    printf("Get current pointer: %s\n", retrieved ? "found" : "NULL");
    
    // Test used as length vs count
    printf("Used as length: %zu\n", _mulle__structarray_get_used_as_length(array));
    printf("Size as length: %zu\n", _mulle__structarray_get_size_as_length(array));
    
    mulle__structarray_destroy(array, NULL);
}

static void test_structarray_zero_and_count_operations(void)
{
    struct mulle__structarray   *array;
    struct test_struct          item;
    size_t                      count;
    
    printf("Testing structarray zero and count operations\n");
    
    array = mulle__structarray_create(sizeof(struct test_struct), 
                                     alignof(struct test_struct), 
                                     5, NULL);
    if( ! array)
    {
        printf("Failed to create structarray\n");
        return;
    }
    
    // Add some initial data
    item = (struct test_struct){1, "first", 1.0};
    _mulle__structarray_add(array, &item, NULL);
    
    item = (struct test_struct){2, "second", 2.0};
    _mulle__structarray_add(array, &item, NULL);
    
    printf("Initial count: %zu\n", _mulle__structarray_get_count(array));
    
    // Test zero to count operation - this should zero additional elements
    _mulle__structarray_zero_to_count(array, 5, NULL);
    count = _mulle__structarray_get_count(array);
    printf("Count after zero_to_count(5): %zu\n", count);
    
    // Test set count operation
    count = _mulle__structarray_set_count(array, 3, NULL);
    printf("Count after set_count(3): %zu (returned: %zu)\n", 
           _mulle__structarray_get_count(array), count);
    
    // Test size to fit
    _mulle__structarray_size_to_fit(array, NULL);
    printf("Size after size_to_fit: %zu\n", _mulle__structarray_get_size(array));
    
    mulle__structarray_destroy(array, NULL);
}

int main(void)
{
    printf("Testing structarray comprehensive coverage\n");
    
    test_structarray_basic_operations();
    test_structarray_pop_and_set();
    test_structarray_insert_and_remove();
    test_structarray_guarantee_and_reserve();
    test_structarray_static_storage();
    test_structarray_sorting();
    test_structarray_edge_cases();
    test_structarray_zero_and_count_operations();
    
    printf("All structarray coverage tests completed\n");
    return( 0);
}
