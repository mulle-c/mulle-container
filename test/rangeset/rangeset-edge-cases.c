#include "include.h"

int   main( void)
{
   mulle_printf("Testing rangeset edge cases and boundary conditions\n");
   
   // Test 1: NULL parameter handling
   mulle_printf("Test 1: NULL parameter handling\n");
   {
      struct mulle_rangeset  set;
      struct mulle_range     range = {0, 10};
      
      // Test NULL set parameter
      mulle_printf("  NULL set parameter: ");
      mulle_rangeset_add(NULL, range);
      mulle_printf("no crash\n");
      
      mulle_printf("  NULL range parameter: ");
      mulle_rangeset_add(&set, *(struct mulle_range *)NULL);
      mulle_printf("no crash\n");
   }
   
   // Test 2: Empty/uninitialized objects
   mulle_printf("Test 2: Empty/uninitialized objects\n");
   {
      struct mulle_rangeset  set;
      
      // Initialize empty set
      mulle_rangeset_init(&set, 0, NULL);
      mulle_printf("  Empty set count: %zu\n", mulle_rangeset_get_count(&set));
      
      // Test operations on empty set
      struct mulle_range range = {5, 15};
      mulle_rangeset_add(&set, range);
      mulle_printf("  Added to empty set, count: %zu\n", mulle_rangeset_get_count(&set));
      
      mulle_rangeset_done(&set);
   }
   
   // Test 3: Boundary conditions
   mulle_printf("Test 3: Boundary conditions\n");
   {
      struct mulle_rangeset  set;
      
      mulle_rangeset_init(&set, 10, NULL);
      
      // Test zero-length ranges
      struct mulle_range zero_range = {5, 5};
      mulle_rangeset_add(&set, zero_range);
      mulle_printf("  Zero-length range added, count: %zu\n", 
                   mulle_rangeset_get_count(&set));
      
      // Test very large ranges
      struct mulle_range large_range = {0, 1000000};
      mulle_rangeset_add(&set, large_range);
      mulle_printf("  Large range added, count: %zu\n", 
                   mulle_rangeset_get_count(&set));
      
      // Test adjacent ranges
      struct mulle_range range1 = {10, 20};
      struct mulle_range range2 = {20, 30};
      mulle_rangeset_add(&set, range1);
      mulle_rangeset_add(&set, range2);
      mulle_printf("  Adjacent ranges added, count: %zu\n", 
                   mulle_rangeset_get_count(&set));
      
      mulle_rangeset_done(&set);
   }
   
   // Test 4: Invalid ranges
   mulle_printf("Test 4: Invalid ranges\n");
   {
      struct mulle_rangeset  set;
      
      mulle_rangeset_init(&set, 10, NULL);
      
      // Test invalid range (end < start)
      struct mulle_range invalid_range = {20, 10};
      mulle_rangeset_add(&set, invalid_range);
      mulle_printf("  Invalid range (end < start) handled\n");
      
      // Test negative values (as unsigned)
      struct mulle_range negative_range = {(uintptr_t)-10, (uintptr_t)-5};
      mulle_rangeset_add(&set, negative_range);
      mulle_printf("  Large unsigned range handled\n");
      
      mulle_rangeset_done(&set);
   }
   
   // Test 5: Overlapping ranges
   mulle_printf("Test 5: Overlapping ranges\n");
   {
      struct mulle_rangeset  set;
      
      mulle_rangeset_init(&set, 10, NULL);
      
      // Test overlapping ranges
      struct mulle_range range1 = {10, 30};
      struct mulle_range range2 = {20, 40};
      struct mulle_range range3 = {25, 35};
      
      mulle_rangeset_add(&set, range1);
      mulle_rangeset_add(&set, range2);
      mulle_rangeset_add(&set, range3);
      
      mulle_printf("  Overlapping ranges merged, count: %zu\n", 
                   mulle_rangeset_get_count(&set));
      
      // Test enumeration
      struct mulle_rangesetenumerator rover;
      struct mulle_range *range;
      
      mulle_printf("  Final ranges:\n");
      rover = mulle_rangeset_enumerate(&set);
      while ((range = mulle_rangesetenumerator_next(&rover)))
         mulle_printf("    Range: [%zu, %zu)\n", range->location, 
                      range->location + range->length);
      mulle_rangesetenumerator_done(&rover);
      
      mulle_rangeset_done(&set);
   }
   
   // Test 6: Search operations
   mulle_printf("Test 6: Search operations\n");
   {
      struct mulle_rangeset  set;
      
      mulle_rangeset_init(&set, 10, NULL);
      
      // Add test ranges
      struct mulle_range ranges[] = {
         {10, 20},
         {30, 40},
         {50, 60}
      };
      
      for (int i = 0; i < 3; i++)
         mulle_rangeset_add(&set, ranges[i]);
      
      // Test contains
      struct mulle_range test_range = {15, 25};
      int contains = mulle_rangeset_contains(&set, test_range);
      mulle_printf("  Range [15,25) contained: %d\n", contains);
      
      // Test intersects
      int intersects = mulle_rangeset_intersects(&set, test_range);
      mulle_printf("  Range [15,25) intersects: %d\n", intersects);
      
      // Test location search
      uintptr_t location = 35;
      struct mulle_range *found = mulle_rangeset_get_range_at_location(&set, location);
      if (found)
         mulle_printf("  Location 35 found in range: [%zu, %zu)\n", 
                      found->location, found->location + found->length);
      else
         mulle_printf("  Location 35 not found in any range\n");
      
      mulle_rangeset_done(&set);
   }
   
   // Test 7: Remove operations
   mulle_printf("Test 7: Remove operations\n");
   {
      struct mulle_rangeset  set;
      
      mulle_rangeset_init(&set, 10, NULL);
      
      // Add test ranges
      struct mulle_range ranges[] = {
         {10, 30},
         {40, 60},
         {70, 90}
      };
      
      for (int i = 0; i < 3; i++)
         mulle_rangeset_add(&set, ranges[i]);
      
      mulle_printf("  Initial count: %zu\n", mulle_rangeset_get_count(&set));
      
      // Test remove range
      struct mulle_range remove_range = {15, 25};
      mulle_rangeset_remove(&set, remove_range);
      mulle_printf("  After remove [15,25), count: %zu\n", 
                   mulle_rangeset_get_count(&set));
      
      // Test remove non-existent range
      struct mulle_range nonexistent = {100, 110};
      mulle_rangeset_remove(&set, nonexistent);
      mulle_printf("  After remove non-existent, count: %zu\n", 
                   mulle_rangeset_get_count(&set));
      
      mulle_rangeset_done(&set);
   }
   
   // Test 8: Memory allocation failures
   mulle_printf("Test 8: Memory allocation failures\n");
   {
      struct mulle_rangeset  set;
      
      // Test with very large capacity
      mulle_rangeset_init(&set, 1000000, NULL);
      mulle_printf("  Large capacity set size: %zu\n", 
                   mulle_rangeset_get_size(&set));
      
      mulle_rangeset_done(&set);
   }
   
   // Test 9: Shift operations
   mulle_printf("Test 9: Shift operations\n");
   {
      struct mulle_rangeset  set;
      
      mulle_rangeset_init(&set, 10, NULL);
      
      // Add test ranges
      struct mulle_range ranges[] = {
         {10, 20},
         {30, 40},
         {50, 60}
      };
      
      for (int i = 0; i < 3; i++)
         mulle_rangeset_add(&set, ranges[i]);
      
      // Test shift
      intptr_t shift_amount = 100;
      mulle_rangeset_shift(&set, shift_amount);
      
      mulle_printf("  After shift by %ld:\n", shift_amount);
      struct mulle_rangesetenumerator rover;
      struct mulle_range *range;
      
      rover = mulle_rangeset_enumerate(&set);
      while ((range = mulle_rangesetenumerator_next(&rover)))
         mulle_printf("    Range: [%zu, %zu)\n", range->location, 
                      range->location + range->length);
      mulle_rangesetenumerator_done(&rover);
      
      mulle_rangeset_done(&set);
   }
   
   // Test 10: Reset operations
   mulle_printf("Test 10: Reset operations\n");
   {
      struct mulle_rangeset  set;
      
      mulle_rangeset_init(&set, 10, NULL);
      
      // Add test ranges
      struct mulle_range ranges[] = {
         {10, 20},
         {30, 40},
         {50, 60}
      };
      
      for (int i = 0; i < 3; i++)
         mulle_rangeset_add(&set, ranges[i]);
      
      mulle_printf("  Before reset, count: %zu\n", 
                   mulle_rangeset_get_count(&set));
      
      mulle_rangeset_reset(&set);
      mulle_printf("  After reset, count: %zu\n", 
                   mulle_rangeset_get_count(&set));
      
      mulle_rangeset_done(&set);
   }
   
   mulle_printf("All rangeset edge case tests completed\n");
   return(0);
}