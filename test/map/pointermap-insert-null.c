//
// Test that map insert correctly reports failure when inserting a duplicate key,
// even when the existing value is NULL.
// Regression test for: insert returned success (1) for duplicate key with NULL value
// because write_pair_generic returned old_value (NULL) which was indistinguishable
// from actual success (NULL).
//
#include <mulle-container/mulle-container.h>
#include <stdio.h>


static void   test_insert_duplicate_null_value( void)
{
   struct mulle__pointermap   map;
   int                        result;

   _mulle__pointermap_init( &map, 0, NULL);

   // Insert key=0x1 with value=NULL (using intptr callbacks where NULL is valid as value)
   result = _mulle__pointermap_insert( &map, (void *) 0x1, NULL, NULL);
   printf( "first insert: %s\n", result ? "ok" : "FAILED");

   // Try to insert same key again — should fail (return 0)
   result = _mulle__pointermap_insert( &map, (void *) 0x1, (void *) 0x2, NULL);
   printf( "duplicate insert: %s\n", result == 0 ? "rejected" : "WRONGLY_ACCEPTED");

   // Verify the original value is still NULL (not overwritten)
   void  *value = _mulle__pointermap_get( &map, (void *) 0x1);
   printf( "original value preserved: %s\n", value == NULL ? "YES" : "NO");

   _mulle__pointermap_done( &map, NULL);
}


static void   test_insert_duplicate_nonull_value( void)
{
   struct mulle__pointermap   map;
   int                        result;

   _mulle__pointermap_init( &map, 0, NULL);

   // Insert key=0x1 with value=0x42
   result = _mulle__pointermap_insert( &map, (void *) 0x1, (void *) 0x42, NULL);
   printf( "first insert nonull: %s\n", result ? "ok" : "FAILED");

   // Duplicate should still be rejected
   result = _mulle__pointermap_insert( &map, (void *) 0x1, (void *) 0x99, NULL);
   printf( "duplicate insert nonull: %s\n", result == 0 ? "rejected" : "WRONGLY_ACCEPTED");

   _mulle__pointermap_done( &map, NULL);
}


static void   test_register_null_value( void)
{
   struct mulle__pointermap   map;
   void                       *value;

   _mulle__pointermap_init( &map, 0, NULL);

   // Register key=0x1 with value=NULL
   value = _mulle__pointermap_register( &map, (void *) 0x1, NULL, NULL);
   printf( "register new: %s\n", value == NULL ? "got_null_value" : "WRONG");

   // Register same key again — should return existing value (NULL)
   value = _mulle__pointermap_register( &map, (void *) 0x1, (void *) 0x99, NULL);
   printf( "register existing: %s\n", value == NULL ? "got_existing_null" : "WRONG");

   _mulle__pointermap_done( &map, NULL);
}


int   main( void)
{
   test_insert_duplicate_null_value();
   test_insert_duplicate_nonull_value();
   test_register_null_value();
   return( 0);
}
