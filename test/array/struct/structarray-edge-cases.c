#include "include.h"
#include <stdio.h>

struct TestStruct
{
   int   id;
   char  name[32];
   double value;
};

int   main( void)
{
   printf("Testing structarray edge cases and boundary conditions\n");
   
   // Test 1: NULL parameter handling
   printf("Test 1: NULL parameter handling\n");
   {
      struct mulle_structarray  array;
      struct TestStruct         test_data = {1, "test", 3.14};
      
      // Test NULL array parameter
      printf("  NULL array parameter: ");
      mulle_structarray_add(NULL, &test_data);
      printf("no crash\n");
      
      printf("  NULL item parameter: ");
      mulle_structarray_add(&array, NULL);
      printf("no crash\n");
   }
   
   // Test 2: Empty/uninitialized objects
   printf("Test 2: Empty/uninitialized objects\n");
   {
      struct mulle_structarray  array;
      
      // Initialize with zero capacity
      mulle_structarray_init(&array, sizeof(struct TestStruct), alignof(struct TestStruct), 0, NULL);
      printf("  Empty array count: %zu\n", mulle_structarray_get_count(&array));
      
      // Try operations on empty array
      struct TestStruct item = {0};
      mulle_structarray_add(&array, &item);
      printf("  Added to empty array, count: %zu\n", mulle_structarray_get_count(&array));
      
      mulle_structarray_done(&array);
   }
   
   // Test 3: Boundary conditions
   printf("Test 3: Boundary conditions\n");
   {
      struct mulle_structarray  array;
      
      // Test with size 1
      mulle_structarray_init(&array, sizeof(struct TestStruct), alignof(struct TestStruct), 1, NULL);
      
      struct TestStruct item = {42, "boundary", 99.9};
      mulle_structarray_add(&array, &item);
      printf("  Added item, count: %zu\n", mulle_structarray_get_count(&array));
      
      // Test adding to full array
      struct TestStruct item2 = {43, "overflow", 100.0};
      mulle_structarray_add(&array, &item2);
      printf("  Added to full array, count: %zu\n", mulle_structarray_get_count(&array));
      
      mulle_structarray_done(&array);
   }
   

   // Test 4: Memory allocation failures
   printf("Test 4: Memory allocation failures\n");
   {
      struct mulle_structarray  array;
      
      // Test with very large capacity
      mulle_structarray_init(&array, sizeof(struct TestStruct), alignof(struct TestStruct), 1000000, NULL);
      printf("  Large capacity array size: %zu\n", mulle_structarray_get_size(&array));
      
      mulle_structarray_done(&array);
   }
   
   // Test 5: Error conditions
   printf("Test 5: Error conditions\n");
   {
      struct mulle_structarray  array;
      
      // Test pop from empty array
      mulle_structarray_init(&array, sizeof(struct TestStruct), alignof(struct TestStruct), 10, NULL);
      
      struct TestStruct popped;
      int result = mulle_structarray_pop(&array, &popped);
      printf("  Pop from empty array: %d\n", result);
      
      mulle_structarray_done(&array);
   }
   
   // Test 6: Static storage initialization
   printf("Test 6: Static storage initialization\n");
   {
      struct mulle_structarray  array;
      char                      static_storage[1024];
      
      mulle_structarray_init_with_static_storage(&array, sizeof(struct TestStruct), 
                                                alignof(struct TestStruct), 10,
                                                static_storage, NULL);
      printf("  Static storage array capacity: %zu\n", mulle_structarray_get_size(&array));
      
      struct TestStruct item = {100, "static", 123.45};
      mulle_structarray_add(&array, &item);
      printf("  Added to static storage, count: %zu\n", mulle_structarray_get_count(&array));
      
      mulle_structarray_done(&array);
   }
   
   // Test 7: Copy operations
   printf("Test 7: Copy operations\n");
   {
      struct mulle_structarray  source_array;
      struct mulle_structarray  dest_array;
      
      mulle_structarray_init(&source_array, sizeof(struct TestStruct), alignof(struct TestStruct), 10, NULL);
      
      // Add some test data
      struct TestStruct items[] = {
         {1, "one", 1.1},
         {2, "two", 2.2},
         {3, "three", 3.3}
      };
      
      for (int i = 0; i < 3; i++)
         mulle_structarray_add(&source_array, &items[i]);
      
      printf("  Source array count: %zu\n", mulle_structarray_get_count(&source_array));
      
      // Test copy
      mulle_structarray_init(&dest_array, sizeof(struct TestStruct), alignof(struct TestStruct), 10, NULL);
      mulle_structarray_add_structarray(&dest_array, &source_array, mulle_range_make_all());
      printf("  Copied array count: %zu\n", mulle_structarray_get_count(&dest_array));
      
      // Test reset
      mulle_structarray_reset(&dest_array);
      printf("  After reset, dest count: %zu\n", mulle_structarray_get_count(&dest_array));
      
      mulle_structarray_done(&source_array);
      mulle_structarray_done(&dest_array);
   }
   
   // Test 8: Enumeration
   printf("Test 8: Enumeration\n");
   {
      struct mulle_structarray  array;
      struct mulle_structarrayenumerator rover;
      struct TestStruct *item;
      
      mulle_structarray_init(&array, sizeof(struct TestStruct), alignof(struct TestStruct), 10, NULL);
      
      // Add test data
      struct TestStruct items[] = {
         {10, "enum1", 10.1},
         {20, "enum2", 20.2},
         {30, "enum3", 30.3}
      };
      
      for (int i = 0; i < 3; i++)
         mulle_structarray_add(&array, &items[i]);
      
      printf("  Enumerating items:\n");
      rover = mulle_structarray_enumerate(&array);
      while (mulle_structarrayenumerator_next(&rover, (void **)&item))
         printf("    Item: id=%d, name=%s, value=%.1f\n", 
                      item->id, item->name, item->value);
      mulle_structarrayenumerator_done(&rover);
      
      mulle_structarray_done(&array);
   }
   
   printf("All structarray edge case tests completed\n");
   return(0);
}