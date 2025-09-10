#include "include.h"

struct TestStruct
{
   int   id;
   char  name[32];
   double value;
};

int   main( void)
{
   mulle_printf("Testing pointerset-struct edge cases and boundary conditions\n");
   
   // Test 1: NULL parameter handling
   mulle_printf("Test 1: NULL parameter handling\n");
   {
      struct mulle__pointerset  set;
      struct TestStruct         test_data = {1, "test", 3.14};
      
      // Test NULL set parameter
      mulle_printf("  NULL set parameter: ");
      _mulle__pointerset_add(NULL, &test_data);
      mulle_printf("no crash\n");
      
      mulle_printf("  NULL item parameter: ");
      _mulle__pointerset_add(&set, NULL);
      mulle_printf("no crash\n");
   }
   
   // Test 2: Empty/uninitialized objects
   mulle_printf("Test 2: Empty/uninitialized objects\n");
   {
      struct mulle__pointerset  set;
      
      // Initialize with zero capacity
      _mulle__pointerset_init(&set, 0, NULL);
      mulle_printf("  Empty set count: %zu\n", _mulle__pointerset_get_count(&set));
      
      // Test operations on empty set
      struct TestStruct *result = _mulle__pointerset_get(&set, NULL);
      mulle_printf("  Get from empty set: %s\n", result ? "found" : "NULL");
      
      _mulle__pointerset_done(&set, NULL);
   }
   
   // Test 3: Boundary conditions
   mulle_printf("Test 3: Boundary conditions\n");
   {
      struct mulle__pointerset  set;
      
      // Test with capacity 1
      _mulle__pointerset_init(&set, 1, NULL);
      mulle_printf("  Set with capacity 1 - initial size: %zu\n", set._size);
      
      struct TestStruct item1 = {42, "boundary", 99.9};
      _mulle__pointerset_add(&set, &item1);
      mulle_printf("  Added item, count: %zu\n", _mulle__pointerset_get_count(&set));
      
      // Test adding duplicate
      _mulle__pointerset_add(&set, &item1);
      mulle_printf("  Added duplicate, count: %zu\n", 
                   _mulle__pointerset_get_count(&set));
      
      _mulle__pointerset_done(&set, NULL);
   }
   
   // Test 4: Invalid parameters
   mulle_printf("Test 4: Invalid parameters\n");
   {
      struct mulle__pointerset  set;
      
      // Test with very large capacity
      _mulle__pointerset_init(&set, 1000000, NULL);
      mulle_printf("  Large capacity set size: %zu\n", set._size);
      
      _mulle__pointerset_done(&set, NULL);
   }
   
   // Test 5: Memory allocation failures
   mulle_printf("Test 5: Memory allocation failures\n");
   {
      struct mulle__pointerset  set;
      
      // Test create with large capacity
      struct mulle__pointerset *large_set = 
         _mulle__pointerset_create(1000000, 0, NULL);
      mulle_printf("  Large set created: %s\n", large_set ? "success" : "failed");
      
      if (large_set)
         _mulle__pointerset_destroy(large_set, NULL);
   }
   
   // Test 6: Basic operations
   mulle_printf("Test 6: Basic operations\n");
   {
      struct mulle__pointerset  set;
      
      _mulle__pointerset_init(&set, 10, NULL);
      
      // Add test data
      struct TestStruct items[] = {
         {1, "one", 1.1},
         {2, "two", 2.2},
         {3, "three", 3.3}
      };
      
      for (int i = 0; i < 3; i++)
         _mulle__pointerset_add(&set, &items[i]);
      
      mulle_printf("  Added %zu items\n", _mulle__pointerset_get_count(&set));
      
      // Test retrieval
      for (int i = 0; i < 3; i++)
      {
         struct TestStruct *item = _mulle__pointerset_get(&set, &items[i]);
         if (item)
            mulle_printf("    Retrieved: %p -> id=%d, name=%s, value=%.1f\n", 
                         (void *)item, item->id, item->name, item->value);
      }
      
      // Test non-existent item
      struct TestStruct missing = {999, "missing", 0.0};
      struct TestStruct *result = _mulle__pointerset_get(&set, &missing);
      mulle_printf("  Missing item result: %s\n", result ? "found" : "NULL");
      
      _mulle__pointerset_done(&set, NULL);
   }
   
   // Test 7: Remove operations
   mulle_printf("Test 7: Remove operations\n");
   {
      struct mulle__pointerset  set;
      
      _mulle__pointerset_init(&set, 10, NULL);
      
      // Add test data
      struct TestStruct item = {100, "remove_test", 123.45};
      _mulle__pointerset_add(&set, &item);
      
      mulle_printf("  Before remove, count: %zu\n", 
                   _mulle__pointerset_get_count(&set));
      
      // Test remove
      int removed = _mulle__pointerset_remove(&set, &item, NULL);
      mulle_printf("  Remove result: %d\n", removed);
      mulle_printf("  After remove, count: %zu\n", 
                   _mulle__pointerset_get_count(&set));
      
      // Test remove non-existent
      struct TestStruct nonexistent = {999, "nonexistent", 0.0};
      removed = _mulle__pointerset_remove(&set, &nonexistent, NULL);
      mulle_printf("  Remove non-existent result: %d\n", removed);
      
      _mulle__pointerset_done(&set, NULL);
   }
   
   // Test 8: Enumeration
   mulle_printf("Test 8: Enumeration\n");
   {
      struct mulle__pointerset  set;
      struct mulle__pointersetenumerator rover;
      struct TestStruct *item;
      
      _mulle__pointerset_init(&set, 10, NULL);
      
      // Add test data
      struct TestStruct items[] = {
         {10, "enum1", 10.1},
         {20, "enum2", 20.2},
         {30, "enum3", 30.3}
      };
      
      for (int i = 0; i < 3; i++)
         _mulle__pointerset_add(&set, &items[i]);
      
      mulle_printf("  Enumerating set items:\n");
      rover = _mulle__pointerset_enumerate(&set);
      while ((item = _mulle__pointersetenumerator_next(&rover)))
         mulle_printf("    Item: %p -> id=%d, name=%s, value=%.1f\n", 
                      (void *)item, item->id, item->name, item->value);
      _mulle__pointersetenumerator_done(&rover);
      
      _mulle__pointerset_done(&set, NULL);
   }
   
   // Test 9: Reset operations
   mulle_printf("Test 9: Reset operations\n");
   {
      struct mulle__pointerset  set;
      
      _mulle__pointerset_init(&set, 10, NULL);
      
      // Add test data
      struct TestStruct items[] = {
         {1, "reset1", 1.1},
         {2, "reset2", 2.2},
         {3, "reset3", 3.3}
      };
      
      for (int i = 0; i < 3; i++)
         _mulle__pointerset_add(&set, &items[i]);
      
      mulle_printf("  Before reset, count: %zu\n", 
                   _mulle__pointerset_get_count(&set));
      
      _mulle__pointerset_reset(&set, NULL);
      mulle_printf("  After reset, count: %zu\n", 
                   _mulle__pointerset_get_count(&set));
      
      _mulle__pointerset_done(&set, NULL);
   }
   
   // Test 10: Copy operations
   mulle_printf("Test 10: Copy operations\n");
   {
      struct mulle__pointerset  source_set;
      struct mulle__pointerset  *copied_set;
      
      _mulle__pointerset_init(&source_set, 10, NULL);
      
      // Add test data
      struct TestStruct items[] = {
         {100, "copy1", 100.1},
         {200, "copy2", 200.2}
      };
      
      for (int i = 0; i < 2; i++)
         _mulle__pointerset_add(&source_set, &items[i]);
      
      mulle_printf("  Source set count: %zu\n", 
                   _mulle__pointerset_get_count(&source_set));
      
      // Test copy
      copied_set = _mulle__pointerset_copy(&source_set, NULL);
      if (copied_set)
      {
         mulle_printf("  Copied set count: %zu\n", 
                      _mulle__pointerset_get_count(copied_set));
         
         // Verify copy
         for (int i = 0; i < 2; i++)
         {
            struct TestStruct *item = _mulle__pointerset_get(copied_set, &items[i]);
            if (item)
               mulle_printf("    Verified copy: %p -> id=%d\n", 
                            (void *)item, item->id);
         }
         
         _mulle__pointerset_destroy(copied_set, NULL);
      }
      
      _mulle__pointerset_done(&source_set, NULL);
   }
   
   // Test 11: Membership testing
   mulle_printf("Test 11: Membership testing\n");
   {
      struct mulle__pointerset  set;
      
      _mulle__pointerset_init(&set, 10, NULL);
      
      // Add test data
      struct TestStruct items[] = {
         {1, "member1", 1.1},
         {2, "member2", 2.2}
      };
      
      for (int i = 0; i < 2; i++)
         _mulle__pointerset_add(&set, &items[i]);
      
      // Test membership
      for (int i = 0; i < 2; i++)
      {
         int is_member = _mulle__pointerset_member(&set, &items[i]);
         mulle_printf("  Item %d membership: %d\n", items[i].id, is_member);
      }
      
      // Test non-member
      struct TestStruct non_member = {999, "non_member", 0.0};
      int is_member = _mulle__pointerset_member(&set, &non_member);
      mulle_printf("  Non-member membership: %d\n", is_member);
      
      _mulle__pointerset_done(&set, NULL);
   }
   
   mulle_printf("All pointerset-struct edge case tests completed\n");
   return(0);
}