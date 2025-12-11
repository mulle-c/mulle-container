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
   
   // Crashes in assert as its supposed tp
   printf("Test 1: Invalid parameters\n");
   {
      struct mulle_structarray  array;
      
      // Test invalid index
      mulle_structarray_init(&array, sizeof(struct TestStruct), alignof(struct TestStruct), 10, NULL);
      
      struct TestStruct *item = mulle_structarray_get(&array, 100);
      printf("  Get invalid index 100: %s\n", item ? "found" : "NULL");
      
      // Test remove invalid index
      mulle_structarray_remove_at_index(&array, 0);
      printf("  Remove invalid index 0: no crash\n");
      
      mulle_structarray_done(&array);
   }

   printf("All structarray edge case tests completed\n");
   return(0);
}