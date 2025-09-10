#include "include.h"

struct TestStruct
{
   int   id;
   char  name[32];
   double value;
};

int   main( void)
{
   mulle_printf("Testing pointermap-struct edge cases and boundary conditions\n");
   
   // Test 1: NULL parameter handling
   mulle_printf("Test 1: NULL parameter handling\n");
   {
      struct mulle__pointermap  map;
      struct TestStruct         test_data = {1, "test", 3.14};
      
      // Test NULL map parameter
      mulle_printf("  NULL map parameter: ");
      _mulle__pointermap_set(NULL, "key", &test_data, NULL);
      mulle_printf("no crash\n");
      
      mulle_printf("  NULL key parameter: ");
      _mulle__pointermap_set(&map, NULL, &test_data, NULL);
      mulle_printf("no crash\n");
      
      mulle_printf("  NULL value parameter: ");
      _mulle__pointermap_set(&map, "key", NULL, NULL);
      mulle_printf("no crash\n");
   }
   
   // Test 2: Empty/uninitialized objects
   mulle_printf("Test 2: Empty/uninitialized objects\n");
   {
      struct mulle__pointermap  map;
      
      // Initialize with zero capacity
      _mulle__pointermap_init(&map, 0, NULL);
      mulle_printf("  Empty map count: %zu\n", _mulle__pointermap_get_count(&map));
      
      // Test operations on empty map
      struct TestStruct *result = _mulle__pointermap_get(&map, "test_key");
      mulle_printf("  Get from empty map: %s\n", result ? "found" : "NULL");
      
      _mulle__pointermap_done(&map, NULL);
   }
   
   // Test 3: Boundary conditions
   mulle_printf("Test 3: Boundary conditions\n");
   {
      struct mulle__pointermap  map;
      
      // Test with capacity 1
      _mulle__pointermap_init(&map, 1, NULL);
      mulle_printf("  Map with capacity 1 - initial size: %zu\n", map._size);
      
      struct TestStruct item1 = {42, "boundary", 99.9};
      _mulle__pointermap_set(&map, "key1", &item1, NULL);
      mulle_printf("  Added item, count: %zu\n", _mulle__pointermap_get_count(&map));
      
      // Test collision handling
      struct TestStruct item2 = {43, "collision", 100.0};
      _mulle__pointermap_set(&map, "key2", &item2, NULL);
      mulle_printf("  Added second item, count: %zu\n", 
                   _mulle__pointermap_get_count(&map));
      
      _mulle__pointermap_done(&map, NULL);
   }
   
   // Test 4: Invalid parameters
   mulle_printf("Test 4: Invalid parameters\n");
   {
      struct mulle__pointermap  map;
      
      // Test with very large capacity
      _mulle__pointermap_init(&map, 1000000, NULL);
      mulle_printf("  Large capacity map size: %zu\n", map._size);
      
      _mulle__pointermap_done(&map, NULL);
   }
   
   // Test 5: Memory allocation failures
   mulle_printf("Test 5: Memory allocation failures\n");
   {
      struct mulle__pointermap  map;
      
      // Test create with large capacity
      struct mulle__pointermap *large_map = 
         mulle__pointermap_create(1000000, 0, NULL);
      mulle_printf("  Large map created: %s\n", large_map ? "success" : "failed");
      
      if (large_map)
         mulle__pointermap_destroy(large_map, NULL);
   }
   
   // Test 6: Basic operations
   mulle_printf("Test 6: Basic operations\n");
   {
      struct mulle__pointermap  map;
      
      _mulle__pointermap_init(&map, 10, NULL);
      
      // Add test data
      struct TestStruct items[] = {
         {1, "one", 1.1},
         {2, "two", 2.2},
         {3, "three", 3.3}
      };
      
      const char *keys[] = {"key1", "key2", "key3"};
      
      for (int i = 0; i < 3; i++)
         _mulle__pointermap_set(&map, keys[i], &items[i], NULL);
      
      mulle_printf("  Added %zu items\n", _mulle__pointermap_get_count(&map));
      
      // Test retrieval
      for (int i = 0; i < 3; i++)
      {
         struct TestStruct *item = _mulle__pointermap_get(&map, keys[i]);
         if (item)
            mulle_printf("    Retrieved: %s -> id=%d, name=%s, value=%.1f\n", 
                         keys[i], item->id, item->name, item->value);
      }
      
      // Test non-existent key
      struct TestStruct *missing = _mulle__pointermap_get(&map, "missing");
      mulle_printf("  Missing key result: %s\n", missing ? "found" : "NULL");
      
      _mulle__pointermap_done(&map, NULL);
   }
   
   // Test 7: Remove operations
   mulle_printf("Test 7: Remove operations\n");
   {
      struct mulle__pointermap  map;
      
      _mulle__pointermap_init(&map, 10, NULL);
      
      // Add test data
      struct TestStruct item = {100, "remove_test", 123.45};
      _mulle__pointermap_set(&map, "remove_key", &item, NULL);
      
      mulle_printf("  Before remove, count: %zu\n", 
                   _mulle__pointermap_get_count(&map));
      
      // Test remove
      int removed = _mulle__pointermap_remove(&map, "remove_key", NULL);
      mulle_printf("  Remove result: %d\n", removed);
      mulle_printf("  After remove, count: %zu\n", 
                   _mulle__pointermap_get_count(&map));
      
      // Test remove non-existent
      removed = _mulle__pointermap_remove(&map, "nonexistent", NULL);
      mulle_printf("  Remove non-existent result: %d\n", removed);
      
      _mulle__pointermap_done(&map, NULL);
   }
   
   // Test 8: Enumeration
   mulle_printf("Test 8: Enumeration\n");
   {
      struct mulle__pointermap  map;
      struct mulle__pointermapenumerator rover;
      void *key;
      struct TestStruct *value;
      
      _mulle__pointermap_init(&map, 10, NULL);
      
      // Add test data
      struct TestStruct items[] = {
         {10, "enum1", 10.1},
         {20, "enum2", 20.2},
         {30, "enum3", 30.3}
      };
      
      const char *keys[] = {"enum_key1", "enum_key2", "enum_key3"};
      
      for (int i = 0; i < 3; i++)
         _mulle__pointermap_set(&map, keys[i], &items[i], NULL);
      
      mulle_printf("  Enumerating map items:\n");
      rover = _mulle__pointermap_enumerate(&map);
      while (_mulle__pointermapenumerator_next(&rover, &key, (void **)&value))
         mulle_printf("    %s -> id=%d, name=%s, value=%.1f\n", 
                      (char *)key, value->id, value->name, value->value);
      _mulle__pointermapenumerator_done(&rover);
      
      _mulle__pointermap_done(&map, NULL);
   }
   
   // Test 9: Reset operations
   mulle_printf("Test 9: Reset operations\n");
   {
      struct mulle__pointermap  map;
      
      _mulle__pointermap_init(&map, 10, NULL);
      
      // Add test data
      struct TestStruct items[] = {
         {1, "reset1", 1.1},
         {2, "reset2", 2.2},
         {3, "reset3", 3.3}
      };
      
      const char *keys[] = {"reset_key1", "reset_key2", "reset_key3"};
      
      for (int i = 0; i < 3; i++)
         _mulle__pointermap_set(&map, keys[i], &items[i], NULL);
      
      mulle_printf("  Before reset, count: %zu\n", 
                   _mulle__pointermap_get_count(&map));
      
      _mulle__pointermap_reset(&map, NULL);
      mulle_printf("  After reset, count: %zu\n", 
                   _mulle__pointermap_get_count(&map));
      
      _mulle__pointermap_done(&map, NULL);
   }
   
   // Test 10: Copy operations
   mulle_printf("Test 10: Copy operations\n");
   {
      struct mulle__pointermap  source_map;
      struct mulle__pointermap  *copied_map;
      
      _mulle__pointermap_init(&source_map, 10, NULL);
      
      // Add test data
      struct TestStruct items[] = {
         {100, "copy1", 100.1},
         {200, "copy2", 200.2}
      };
      
      const char *keys[] = {"copy_key1", "copy_key2"};
      
      for (int i = 0; i < 2; i++)
         _mulle__pointermap_set(&source_map, keys[i], &items[i], NULL);
      
      mulle_printf("  Source map count: %zu\n", 
                   _mulle__pointermap_get_count(&source_map));
      
      // Test copy
      copied_map = _mulle__pointermap_copy(&source_map, NULL);
      if (copied_map)
      {
         mulle_printf("  Copied map count: %zu\n", 
                      _mulle__pointermap_get_count(copied_map));
         
         // Verify copy
         for (int i = 0; i < 2; i++)
         {
            struct TestStruct *item = _mulle__pointermap_get(copied_map, keys[i]);
            if (item)
               mulle_printf("    Verified copy: %s -> id=%d\n", keys[i], item->id);
         }
         
         mulle__pointermap_destroy(copied_map, NULL);
      }
      
      _mulle__pointermap_done(&source_map, NULL);
   }
   
   // Test 11: Shrink operations
   mulle_printf("Test 11: Shrink operations\n");
   {
      struct mulle__pointermap  map;
      
      _mulle__pointermap_init(&map, 100, NULL);
      
      // Add many items to force growth
      struct TestStruct item = {999, "shrink", 999.9};
      for (int i = 0; i < 50; i++)
      {
         char key[32];
         snprintf(key, sizeof(key), "shrink_key_%d", i);
         _mulle__pointermap_set(&map, key, &item, NULL);
      }
      
      mulle_printf("  Before shrink, size: %zu\n", map._size);
      
      // Remove most items
      for (int i = 0; i < 45; i++)
      {
         char key[32];
         snprintf(key, sizeof(key), "shrink_key_%d", i);
         _mulle__pointermap_remove(&map, key, NULL);
      }
      
      _mulle__pointermap_shrink_if_needed(&map, NULL);
      mulle_printf("  After shrink, size: %zu\n", map._size);
      
      _mulle__pointermap_done(&map, NULL);
   }
   
   mulle_printf("All pointermap-struct edge case tests completed\n");
   return(0);
}