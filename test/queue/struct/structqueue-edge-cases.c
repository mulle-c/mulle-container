#include "include.h"

struct TestStruct
{
   int   id;
   char  name[32];
   double value;
};

int   main( void)
{
   mulle_printf("Testing structqueue edge cases and boundary conditions\n");
   
   // Test 1: NULL parameter handling
   mulle_printf("Test 1: NULL parameter handling\n");
   {
      struct mulle_structqueue  queue;
      struct TestStruct         test_data = {1, "test", 3.14};
      
      // Test NULL queue parameter
      mulle_printf("  NULL queue parameter: ");
      mulle_structqueue_add(NULL, &test_data);
      mulle_printf("no crash\n");
      
      mulle_printf("  NULL item parameter: ");
      mulle_structqueue_add(&queue, NULL);
      mulle_printf("no crash\n");
   }
   
   // Test 2: Empty/uninitialized objects
   mulle_printf("Test 2: Empty/uninitialized objects\n");
   {
      struct mulle_structqueue  queue;
      
      // Initialize with zero bucket size
      mulle_structqueue_init(&queue, sizeof(struct TestStruct), 0, 0, NULL);
      mulle_printf("  Empty queue count: %zu\n", mulle_structqueue_get_count(&queue));
      
      // Try operations on empty queue
      struct TestStruct item = {0};
      mulle_structqueue_add(&queue, &item);
      mulle_printf("  Added to empty queue, count: %zu\n", 
                   mulle_structqueue_get_count(&queue));
      
      mulle_structqueue_done(&queue);
   }
   
   // Test 3: Boundary conditions
   mulle_printf("Test 3: Boundary conditions\n");
   {
      struct mulle_structqueue  queue;
      
      // Test with bucket size 1
      mulle_structqueue_init(&queue, sizeof(struct TestStruct), 1, 1, NULL);
      mulle_printf("  Queue with bucket size 1 - is full: %d\n", 
                   mulle_structqueue_is_full(&queue));
      
      struct TestStruct item = {42, "boundary", 99.9};
      mulle_structqueue_add(&queue, &item);
      mulle_printf("  Added item, is full now: %d\n", 
                   mulle_structqueue_is_full(&queue));
      
      // Test adding to full queue
      struct TestStruct item2 = {43, "overflow", 100.0};
      mulle_structqueue_add(&queue, &item2);
      mulle_printf("  Added to full queue, count: %zu\n", 
                   mulle_structqueue_get_count(&queue));
      
      mulle_structqueue_done(&queue);
   }
   
   // Test 4: Invalid parameters
   mulle_printf("Test 4: Invalid parameters\n");
   {
      struct mulle_structqueue  queue;
      
      // Test invalid bucket size
      mulle_structqueue_init(&queue, sizeof(struct TestStruct), 0, 0, NULL);
      mulle_printf("  Zero bucket size handled\n");
      
      // Test very large bucket size
      mulle_structqueue_init(&queue, sizeof(struct TestStruct), 1000000, 1000000, NULL);
      mulle_printf("  Large bucket size handled\n");
      
      mulle_structqueue_done(&queue);
   }
   
   // Test 5: Memory allocation failures
   mulle_printf("Test 5: Memory allocation failures\n");
   {
      struct mulle_structqueue  queue;
      
      // Test with very large capacity
      mulle_structqueue_init(&queue, sizeof(struct TestStruct), 1000000, 1000000, NULL);
      mulle_printf("  Large capacity queue size: %zu\n", 
                   mulle_structqueue_get_size(&queue));
      
      mulle_structqueue_done(&queue);
   }
   
   // Test 6: Error conditions
   mulle_printf("Test 6: Error conditions\n");
   {
      struct mulle_structqueue  queue;
      struct TestStruct         popped;
      
      // Test pop from empty queue
      mulle_structqueue_init(&queue, sizeof(struct TestStruct), 10, 10, NULL);
      
      int result = mulle_structqueue_pop(&queue, &popped);
      mulle_printf("  Pop from empty queue: %d\n", result);
      
      mulle_structqueue_done(&queue);
   }
   
   // Test 7: FIFO behavior
   mulle_printf("Test 7: FIFO behavior\n");
   {
      struct mulle_structqueue  queue;
      struct TestStruct         items[5];
      
      mulle_structqueue_init(&queue, sizeof(struct TestStruct), 3, 10, NULL);
      
      // Add test data
      for (int i = 0; i < 5; i++)
      {
         items[i].id = i;
         snprintf(items[i].name, sizeof(items[i].name), "item%d", i);
         items[i].value = i * 1.1;
         
         mulle_structqueue_add(&queue, &items[i]);
         mulle_printf("  Added item %d, count: %zu\n", i, 
                      mulle_structqueue_get_count(&queue));
      }
      
      // Test FIFO order
      mulle_printf("  Popping items in FIFO order:\n");
      struct TestStruct popped;
      while (mulle_structqueue_pop(&queue, &popped))
      {
         mulle_printf("    Popped: id=%d, name=%s, value=%.1f\n", 
                      popped.id, popped.name, popped.value);
      }
      
      mulle_structqueue_done(&queue);
   }
   
   // Test 8: Enumeration
   mulle_printf("Test 8: Enumeration\n");
   {
      struct mulle_structqueue  queue;
      struct mulle_structqueueenumerator rover;
      struct TestStruct *item;
      
      mulle_structqueue_init(&queue, sizeof(struct TestStruct), 5, 10, NULL);
      
      // Add test data
      struct TestStruct items[] = {
         {100, "enum1", 10.1},
         {200, "enum2", 20.2},
         {300, "enum3", 30.3}
      };
      
      for (int i = 0; i < 3; i++)
         mulle_structqueue_add(&queue, &items[i]);
      
      mulle_printf("  Enumerating queue items:\n");
      rover = mulle_structqueue_enumerate(&queue);
      while ((item = mulle_structqueueenumerator_next(&rover)))
         mulle_printf("    Item: id=%d, name=%s, value=%.1f\n", 
                      item->id, item->name, item->value);
      mulle_structqueueenumerator_done(&rover);
      
      mulle_structqueue_done(&queue);
   }
   
   // Test 9: Reset operations
   mulle_printf("Test 9: Reset operations\n");
   {
      struct mulle_structqueue  queue;
      
      mulle_structqueue_init(&queue, sizeof(struct TestStruct), 5, 10, NULL);
      
      // Add test data
      struct TestStruct items[] = {
         {1, "reset1", 1.1},
         {2, "reset2", 2.2},
         {3, "reset3", 3.3}
      };
      
      for (int i = 0; i < 3; i++)
         mulle_structqueue_add(&queue, &items[i]);
      
      mulle_printf("  Before reset, count: %zu\n", 
                   mulle_structqueue_get_count(&queue));
      
      mulle_structqueue_reset(&queue);
      mulle_printf("  After reset, count: %zu\n", 
                   mulle_structqueue_get_count(&queue));
      
      mulle_structqueue_done(&queue);
   }
   
   // Test 10: Grow and shrink operations
   mulle_printf("Test 10: Grow and shrink operations\n");
   {
      struct mulle_structqueue  queue;
      
      mulle_structqueue_init(&queue, sizeof(struct TestStruct), 2, 5, NULL);
      
      mulle_printf("  Initial size: %zu\n", mulle_structqueue_get_size(&queue));
      
      // Force growth
      struct TestStruct item = {999, "grow", 999.9};
      for (int i = 0; i < 10; i++)
         mulle_structqueue_add(&queue, &item);
      
      mulle_printf("  After growth, size: %zu\n", 
                   mulle_structqueue_get_size(&queue));
      
      // Remove all items
      struct TestStruct popped;
      while (mulle_structqueue_pop(&queue, &popped))
         ;
      
      // Force shrink
      mulle_structqueue_shrink(&queue);
      mulle_printf("  After shrink, size: %zu\n", 
                   mulle_structqueue_get_size(&queue));
      
      mulle_structqueue_done(&queue);
   }
   
   mulle_printf("All structqueue edge case tests completed\n");
   return(0);
}
